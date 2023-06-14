#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Items/BaseItem.h"
#include "Items/BaseWeapon.h"
#include "Attributes/CharacterAttribute.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputSubsystems.h"
#include "Attributes/WeaponAttributes.h"
#include "Components/WidgetComponent.h"
#include "Engine/World.h"
#include "HUD/PlayerWeaponHUDWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

/*if(GEngine)
{
	GEngine->Add	
}
*/

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	/*카메라, 스프링 암, 캐릭터 Attributes 생성*/
	MySpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	MainCam = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCam"));
	MySpringArm->SetupAttachment(GetRootComponent());
	MainCam->SetupAttachment(MySpringArm);
	MySpringArm->TargetArmLength = 500.f;
	MySpringArm->SocketOffset = FVector(0, 70.f, 100.f);

	CharAttribute = CreateDefaultSubobject<UCharacterAttribute>(TEXT("CharacterAttributes"));

	// 최초 속도 조정
	GetCharacterMovement()->MaxWalkSpeed = 600;

	// 컨트롤러가 카메라에게만 영향을 미칠 수 있게 함
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	this->bUseControllerRotationYaw = true;

	// 마우스 말고 캐릭터가 이동하는 (키보드) 방향으로 회전하게 한다.
	GetCharacterMovement()->bOrientRotationToMovement = false;
	// 회전 속도
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.5f;

	// 핸드 씬 컴포넌트 생성
	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComponent"));
}

void APlayerCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if(OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceItemInfo = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceItemInfo = true;
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(MainCam)
	{
		DefaultCameraFOV = MainCam->FieldOfView;
		CurrentCameraFOV = DefaultCameraFOV;
	}
	
	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}

	if(CharAttribute)
	{
		CharAttribute->InitializeAmmo();
	}

	if(HUDOverlayClass)
	{
		HUDOverlay = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(this, 0), HUDOverlayClass);
		if(HUDOverlay)
		{
			HUDOverlay->AddToViewport(0);
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
			WeaponHUDWidget = Cast<UPlayerWeaponHUDWidget>(HUDOverlay->GetWidgetFromName(TEXT("BP_AmmoCount2")));
			RefreshAllTypeOfAmmoWidget();
		}
	}

	// 뷰포트의 사이즈와 스크린의 가운데 위치를 미리 구함
	GetViewPortSize();
	bScreenToWorld = GetScreenCenterToConvertWorld();
}

void APlayerCharacter::TraceForItems()
{
	if(bShouldTraceItemInfo)
	{
		FHitResult ItemTraceResult;
		TraceUnderCrosshairs(ItemTraceResult);
		if(ItemTraceResult.bBlockingHit)
		{
			ABaseItem* HitItem = Cast<ABaseItem>(ItemTraceResult.GetActor());
			if(GEngine)
			{
				const FString Name = ItemTraceResult.GetActor()->GetName();
				GEngine->AddOnScreenDebugMessage(173, 0.1f, FColor::Black, Name, false);
			}
			if(HitItem && HitItem->GetPickupWidget())
			{
				
				// 위젯 활성화
				HitItem->GetPickupWidget()->SetVisibility(true);
				HitItem->EnableCustomDepth();
			}
			/* 다음 프레임을 위한 HitItem 레퍼런스 저장
			 * 만약, 아이템에 CrossHair를 올려 무기의 정보를 얻어오는 위젯을 발동시켰다면, HitItem은 그 아이템으로 저장이 될 것이다.
			 * 그 후 마우스 커서를 움직여 조준하고 있는 Item을 변경하거나 아이템이 없는 곳으로 커서를 이동시킬 경우
			 * TraceHitLastFrame에 있는 아이템의 Info Visibility를 False로 바꾸어 위젯을 종료시킨다.
			 */
			if(TraceHitLastFrame)
			{
				// 다른 아이템으로 바뀌었거나, nullptr이 될 경우 Setting the Visibility 
				if(HitItem != TraceHitLastFrame)
				{
					TraceHitLastFrame->GetPickupWidget()->SetVisibility(false);
					TraceHitLastFrame->DisableCustomDepth();
				}
			}
			TraceHitLastFrame = HitItem;
		}
	}
	// 아이템이 주변에 없을 경우에도 모든 Widget을 꺼주어야 한다.
	else if(TraceHitLastFrame)
	{
		// No longer overlapping any Items
		TraceHitLastFrame->GetPickupWidget()->SetVisibility(false);
		TraceHitLastFrame->DisableCustomDepth();
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CameraAiming(DeltaTime);

	CalculateCrosshairSpread(DeltaTime);

	// 주변에 아이템이 있을 때만 CrossHair LineTrace Enable!
	TraceForItems();
}

void APlayerCharacter::CharMove(const FInputActionValue& Value)
{
	const FRotator ControllerRotation = GetControlRotation();
	const FRotator YawRot(0.f, ControllerRotation.Yaw, 0.f);
	const FVector FrontDirection = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	const FVector2D MovementVector = Value.Get<FVector2D>();

	AddMovementInput(FrontDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void APlayerCharacter::Look(const FInputActionValue& Value){
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerYawInput(-LookAxisVector.X * DefaultTurnRate);
	AddControllerPitchInput(LookAxisVector.Y * DefaultTurnRate);
}

// Input Action 바인딩 함수 - E Key
void APlayerCharacter::EKeyPressed()
{
	if(CombatState != ECombatState::ECS_Unoccupied) return;
	// 오버랩된 아이템이 있으면 실행 && 손에 든 총이 없으면 실행 (X), WeaponHUD 갱신
	// 아이템일 경우와, 총일 경우를 나누어야 함
	if(CharAttribute && CharAttribute->GetOverlappedItem())
	{
		//ABaseWeapon* Weapon = Cast<ABaseWeapon>(CharAttribute->GetOverlappedItem());
		ABaseItem* Item = Cast<ABaseItem>(CharAttribute->GetOverlappedItem());
		Item->PickUpItem(this);
		RefreshAllTypeOfAmmoWidget();
		
		// 무기의 이름과 그 무기가 가진 총알 개수를 갱신한다.
		if(WeaponHUDWidget && CharAttribute->GetEquippedWeapon() != nullptr)
		{
			RefreshTheCurrentAmmoWidget();
			WeaponHUDWidget->SetCurrentAmmoNameText(CharAttribute->GetEquippedWeapon()->GetWeaponAttributes()->GetItemName());
			RefreshAllTypeOfAmmoWidget();
		}

	}

	// 손에 들고 있는 무기가 있다면, 무기를 내려놓음
	else if(CharAttribute && CharAttribute->GetEquippedWeapon())
	{
		CharAttribute->GetEquippedWeapon()->ItemDivestiture();
		CharAttribute->GetEquippedWeapon()->ThrowTheWeapon();
		CharAttribute->SetEquippedWeapon(nullptr);

		// 플레이어 HUD 위젯 갱신
		if(WeaponHUDWidget)
		{
			WeaponHUDWidget->SetWeaponAmmoCount(0);
			WeaponHUDWidget->SetCurrentAmmoNameText(" ");
		}
	}
}

void APlayerCharacter::RunKeyPressed(const FInputActionValue& Value)
{
	if(Value.Get<bool>() == true)
	{
		GetCharacterMovement()->MaxWalkSpeed = 1000;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 600;
	}
}

// 오른쪽 마우스 버튼 : 줌, 에임 레이트 조절
void APlayerCharacter::AimKeyPressed(const FInputActionValue& Value)
{
	if(CharAttribute->GetEquippedWeapon() == nullptr) return;
	bAiming = !bAiming;
	if(bAiming)
	{
		MainCam->SetFieldOfView(ZoomedCameraFOV);
		DefaultTurnRate = AimTurnRate;
	}
	else
	{
		MainCam->SetFieldOfView(DefaultCameraFOV);
		DefaultTurnRate = BaseTurnRate;
	}
	
}

// 뷰포트의 사이즈 구하기!
void APlayerCharacter::GetViewPortSize()
{
	if(GEngine && GEngine->GameViewport)
	{
		// CallByReference로 받기 때문에 ViewportSize 변수 자체의 값이 바뀜
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
}

bool APlayerCharacter::GetScreenCenterToConvertWorld()
{
	// 스크린 중앙 위치 구하기
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	// 주어진 2D 화면 공간 좌표를 3D세계 공간 위치와 방향으로 변환
	return UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
	    CrosshairLocation,
	    CrossHairWorldPosition,
	    CrossHairWorldDirection);
}

void APlayerCharacter::FireButtonPressed()
{
	// 행동이 자유롭지 못한 상태라면 발사하면 안 된다.
	if(CombatState != ECombatState::ECS_Unoccupied || CharAttribute->GetEquippedWeapon() == nullptr) return;
	
	bFireButtonPressed = true;
	if(CombatState == ECombatState::ECS_Unoccupied && !bShootingMode && bShouldFire)
	{
		bShouldFire = false;
		ShotKeyPressed(true);
	}
	// 행동이 자유로울 때 발사 가능, 조정간 자동일 때 AutoFire
	else if(CombatState == ECombatState::ECS_Unoccupied && bShootingMode)
	{
		StartFireTimer();
	}
	
}

void APlayerCharacter::FireButtonReleased()
{
	bShouldFire = true;
	bFireButtonPressed = false;
	CombatState = ECombatState::ECS_Unoccupied;
}

void APlayerCharacter::StartFireTimer()
{
	if(bShouldFire)
	{
		CombatState = ECombatState::ECS_FireTimerInProgress;
		ShotKeyPressed(true);
		bShouldFire = false;
		GetWorldTimerManager().SetTimer(AutoFireTimer,
	this,
		&APlayerCharacter::AutoFireReset,
		// 캐릭터가 장착하고 있는 무기의 속성 중 FireRate를 불러와서 연사 속도를 설정한다.
		CharAttribute->GetEquippedWeapon()->GetWeaponAttributes()->GetFireRate());
	}
}

void APlayerCharacter::AutoFireReset()
{
	CombatState = ECombatState::ECS_Unoccupied;
	if(bFireButtonPressed)
	{
		bShouldFire = true;
		StartFireTimer();
	}
}

// 초기 아이템 설정
void APlayerCharacter::SpawnDefaultWeapon()
{
	if(DefaultWeaponClass)
	{
		ABaseWeapon* DefaultWeapon = GetWorld()->SpawnActor<ABaseWeapon>(DefaultWeaponClass, this->GetActorLocation(), this->GetActorRotation());
		SetWeapon(DefaultWeapon);
	}
}

void APlayerCharacter::ShootingModeChange()
{
	bShootingMode = !bShootingMode;
}

void APlayerCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void APlayerCharacter::ReloadWeapon()
{

	// 무언가를 하고 있는 중 일때, 무기가 없을때, 총알이 없을때 바로 return
	if(CombatState != ECombatState::ECS_Unoccupied || CharAttribute->GetEquippedWeapon() == nullptr || !CharAttribute->HasAmmo(CharAttribute->GetEquippedWeapon()->GetWeaponAttributes()->GetWeaponType())) return;
	// 현재 갖고 있는 총알이 최대 개수일 때 바로 return
	if(CharAttribute->GetEquippedWeapon()->GetWeaponAttributes()->IsFullAmmo()) return;
	
	CombatState = ECombatState::ECS_Reloading;
	// 총알을 채우기 위해 현재 장착 중인 총알의 타입을 구함
	const auto AmmoType = CharAttribute->GetEquippedWeapon()->GetWeaponAttributes()->GetWeaponType();

	// 총을 꽉 채우는데 필요한 총알의 개수를 구함
	const int32 NeedAmmo = CharAttribute->GetEquippedWeapon()->GetWeaponAttributes()->GetNeedAmmoCount();
	// 남은 총알을 확인한 후 총알을 채울 수 있는 개수를 구함
	const int32 LoadedBullet = HowManyNeedBulletToReload(AmmoType, NeedAmmo);

	CharAttribute->GetEquippedWeapon()->GetWeaponAttributes()->SetRemainedAmmoCount(LoadedBullet);
	CharAttribute->SetAmmoCountForAmmoType(AmmoType, -LoadedBullet);

	RefreshTheCurrentAmmoWidget();
	RefreshAllTypeOfAmmoWidget();
	
	if(ReloadMontage)
	{
		// 무기 종류에 따른 ENUM Class 만들기
		// 무기 종류에 따른 switch 문 작성
		if(UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(CharAttribute->GetEquippedWeapon()->GetReloadMontageSection());
		}
	}
}

// 전체 총알 종류를 갱신 해줄 것임
void APlayerCharacter::RefreshAllTypeOfAmmoWidget()
{
	if(WeaponHUDWidget)
	{
		WeaponHUDWidget->SetCarriedAssaultAmmoText(CharAttribute->GetAmmoCountByAmmoType(EAmmoType::EAT_AssaultRifle));
		WeaponHUDWidget->SetCarriedCombatAmmoText(CharAttribute->GetAmmoCountByAmmoType(EAmmoType::EAT_CombatRifle));
	}
}

void APlayerCharacter::RefreshTheCurrentAmmoWidget()
{
	if(WeaponHUDWidget)
	{
		WeaponHUDWidget->SetWeaponAmmoCount(CharAttribute->GetEquippedWeapon()->GetWeaponAttributes()->GetAmmoCount());
	}
}

int32 APlayerCharacter::HowManyNeedBulletToReload(EAmmoType AmmoType, int32 NeedAmmo)
{
	// 총알이 필요한 양보다 더 많다면 필요한 양 그대로 장전한다.
	if(CharAttribute->GetAmmoCountByAmmoType(AmmoType) - NeedAmmo >= 0)
	{
		return NeedAmmo;
	}
	// 현재 가지고 있는 총알이 필요한 양보다 적다면 가지고 있는 양만큼만 충전함
	return CharAttribute->GetAmmoCountByAmmoType(AmmoType);
}

// 할 일 : FinishReloading에 Widget 갱신 코드들 옮기기
void APlayerCharacter::FinishReloading()
{
	// CombatState = ECombatState::ECS_Reloading;
	CombatState = ECombatState::ECS_Unoccupied;
}

void APlayerCharacter::GrabClip()
{
	if(CharAttribute->GetEquippedWeapon() == nullptr) return;
	if(HandSceneComponent == nullptr) return;

	// 장착된 무기의 탄창 본 인덱스 구하기 
	const int32 ClipBoneIndex { CharAttribute->GetEquippedWeapon()->GetSkeletalMesh()->GetBoneIndex(CharAttribute->GetEquippedWeapon()->GetClipBoneName())};
	ClipTransform = CharAttribute->GetEquippedWeapon()->GetSkeletalMesh()->GetBoneTransform(ClipBoneIndex);

	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("hand_l")));
	HandSceneComponent->SetWorldTransform(ClipTransform);

	CharAttribute->GetEquippedWeapon()->SetMovingClip(true);
}

void APlayerCharacter::ReleaseClip()
{
	CharAttribute->GetEquippedWeapon()->SetMovingClip(false);
}

// 왼쪽 마우스 버튼 클릭 시
void APlayerCharacter::ShotKeyPressed(const FInputActionValue& Value)
{
	if(CanIShoot())
	{
		// 스크린의 중심 위치를 구함
		GetScreenCenterToConvertWorld();
		if(bScreenToWorld)
		{
			FHitResult ScreenTraceHit;
			const FVector FireStartPoint = CrossHairWorldPosition;
			const FVector FireEndPoint = CrossHairWorldPosition + CrossHairWorldDirection * WeaponRange;

			if(GEngine && ScreenTraceHit.GetActor() != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(29, 2.f, FColor::Turquoise, ScreenTraceHit.GetActor()->GetName());
				UE_LOG(LogTemp, Warning, TEXT("TASD!@#!"));
			}
			
			// 크로스헤어에서 위치에서 라인 트레이스 발사
			GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, FireStartPoint, FireEndPoint, ECC_Visibility);
			// 총 발사 함수 호출
			CharAttribute->GetEquippedWeapon()->GunFire(ScreenTraceHit);
			
			
			RefreshTheCurrentAmmoWidget();

			// 크로스헤어를 위한 총 발사 타이머 실행
			StartCrosshairBulletFire();
		}

		FireMontage();
	}
}

void APlayerCharacter::FireMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

void APlayerCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;

	// [ 타이머 핸들러, 델리게이트* , 콜백함수, 타이머 시간 ] 
	GetWorldTimerManager().SetTimer(CrosshairShootTimer, this, &APlayerCharacter::FinishCrosshairBulletFire, ShootTimeDuration);
}

// 크로스헤어 함수들
void APlayerCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

bool APlayerCharacter::CanIShoot() const
{
	// 장착된 무기가 있고, 총알이 1개 이상일때만 발사할거임
	if(CharAttribute->GetEquippedWeapon())
	{
		if(CharAttribute->GetEquippedWeapon()->GetWeaponAttributes()->GetAmmoCount() > 0)
		{
			return true;
		}
	}
	return false;
}

float APlayerCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void APlayerCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange {0.f, 600.f};
	FVector2D VelocityMultiplierRange{0.f,1.f};
	FVector Velocity { GetVelocity()};
	Velocity.Z = 0.f;

	// 공중에 있을때 크로스헤어 계산
	if(GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else // 캐릭터가 땅에 있을 때
		{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
		if(bAiming)
			CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.5f, DeltaTime, 10.f);
		else
			CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 10.f);
		}

	// 총을 계속 쏠 경우, 크로스헤어를 산개시킴
	if(bFiringBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 60.f);
	}
	// 총을 더이상 쏘지 않을경우 크로스헤어를 빠르게 수렴시킴
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 60.f);
	}
	
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());
	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;
}

bool APlayerCharacter::TraceUnderCrosshairs(FHitResult& HitResult)
{
	// 뷰포트 크기 구하기
	GetViewPortSize();
	bScreenToWorld = GetScreenCenterToConvertWorld();
	if(bScreenToWorld)
	{
		const FVector Start = CrossHairWorldPosition;
		const FVector End = Start + CrossHairWorldDirection * 50000.f;
		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(12321, 1.f, FColor::Cyan, TEXT("VIEWIEWE"));
		}
		if(HitResult.bBlockingHit)
			return true;
	}
	return false;
	
}

// 실제 아이템 장착 함수
void APlayerCharacter::SetWeapon(ABaseWeapon* Item)
{
	CharAttribute->SetEquippedWeapon(Item);
	CharAttribute->GetEquippedWeapon()->ItemAcquisition(this->GetMesh(), this);
}

bool APlayerCharacter::SpeedLimitFunction()
{
	return true;
}

// 주변에 있는 아이템 탐색
void APlayerCharacter::OverlappingItem(ABaseItem* Item)
{
	if(CharAttribute)
	{
		CharAttribute->SetOverlapItem(Item);
	}
}

// 줌 인 - 아웃시 카메라 부드럽게 하기
void APlayerCharacter::CameraAiming(float DeltaTime)
{
	if(bAiming)
	{
		CurrentCameraFOV = FMath::FInterpTo(CurrentCameraFOV, ZoomedCameraFOV, DeltaTime, ZoomInterpSpeed);
	}
	else
	{
		CurrentCameraFOV = FMath::FInterpTo(CurrentCameraFOV, DefaultCameraFOV, DeltaTime, ZoomInterpSpeed);
	}
	MainCam->SetFieldOfView(CurrentCameraFOV);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	Input->BindAction(InputMove, ETriggerEvent::Triggered, this, &APlayerCharacter::CharMove);
	Input->BindAction(InputJump, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	Input->BindAction(InputLook, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	Input->BindAction(InputEquip, ETriggerEvent::Completed, this, &APlayerCharacter::EKeyPressed);
	Input->BindAction(InputRun, ETriggerEvent::Triggered, this, &APlayerCharacter::RunKeyPressed);
	Input->BindAction(InputRun, ETriggerEvent::None, this, &APlayerCharacter::RunKeyPressed);
	Input->BindAction(InputReload, ETriggerEvent::Triggered, this, &APlayerCharacter::ReloadButtonPressed);
	// Input->BindAction(InputShot, ETriggerEvent::Started, this, &APlayerCharacter::ShotKeyPressed);
	Input->BindAction(InputShot, ETriggerEvent::Triggered, this, &APlayerCharacter::FireButtonPressed);
	Input->BindAction(InputShot, ETriggerEvent::Completed, this, &APlayerCharacter::FireButtonReleased);
	Input->BindAction(InputShotChange, ETriggerEvent::Started, this, &APlayerCharacter::ShootingModeChange);
	Input->BindAction(InputAim, ETriggerEvent::Started, this, &APlayerCharacter::AimKeyPressed);
	Input->BindAction(InputAim, ETriggerEvent::Canceled, this, &APlayerCharacter::AimKeyPressed);
}
