#include "Items/BaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Particles/ParticleSystem.h"
#include "Attributes/CharacterAttribute.h"
#include "Particles/ParticleSystemComponent.h"
#include "Attributes/WeaponAttributes.h"
#include "PlayerCharacter.h"
#include "Components/Image.h"
#include "Enemy/EnemyCharacter.h"
#include "Engine/DamageEvents.h"
#include "HUD/WeaponInfoWidget.h"

/*
void ABaseWeapon::IPickUpItem(ABaseWeapon* Weapon)
{
	IPickupInterface::IPickUpItem(Weapon);
	UE_LOG(LogTemp, Warning, TEXT("그거받음ㅋ"));
}
*/

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	
	FirePoint = CreateDefaultSubobject<USceneComponent>(TEXT("FirePoint"));
	FirePoint->SetupAttachment(GetRootComponent());
	WeaponAttributes = CreateDefaultSubobject<UWeaponAttributes>(TEXT("WeaponAttributes"));
	
	SetCollisionWhenItemDivestiture();
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	// 아이템 이름 초기화
	if(WeaponInfo && WeaponAttributes)
	{
		WeaponInfo->SetItemName(WeaponAttributes->GetItemName());
	}
	// DisableCustomDepth();
}

// 아이템을 버림
void ABaseWeapon::ItemDivestiture()
{
	SetCollisionWhenItemDivestiture();
	const FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, false);
	this->DetachFromActor(DetachmentTransformRules);

	// 형광 효과 다시 활성화
	// EnableCustomDepth();
}

// 아이템을 플레이어가 주울 때, 콜리전과 시뮬레이트 비활성화, 소켓에 붙임, 아이템 상태 변경 ★ NewWeapon ★
void ABaseWeapon::ItemAcquisition(USceneComponent* InParent, AActor* NewOwner)
{
	// 바닥에서 습득한 무기라면 캐릭터 소켓에 부착해야함
	if(this->Owner == nullptr)
	{
		// 캐릭터 무기 소켓에 부착하기
		const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		this->AttachToComponent(InParent, AttachmentRules, FName("RightHandWeaponSocket"));
	}

	DisableCustomDepth();
	// 아이템 피직스와 콜라이더 설정 변경
	this->Owner = NewOwner;
	SetCollisionWhenItemChangeOFF();

	SKM->SetVisibility(true);
	SetItemState(EItemState::EIS_Equipped);
}

// 아이템을 다시 떨어트릴 때 -> 콜리전, 시뮬레이션을 모아놓은 함수
void ABaseWeapon::SetCollisionWhenItemDivestiture()
{
	if(SKM && SphereCollision && NoticeSphereCollision)
	{
		this->Owner = nullptr;
		SKM->SetSimulatePhysics(true);
		SKM->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		SphereCollision->SetCollisionResponseToAllChannels(ECR_Overlap);
		SphereCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		NoticeSphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SetItemState(EItemState::EIS_Free);
		EnableCustomDepth();
	}
}

// 변경될 무기 (Old Weapon) : 슬롯에 들어갈 무기를 말함 ★ Old Weapon ★
void ABaseWeapon::SetCollisionWhenItemChangeOFF()
{
	// 물리 효과, OverlapSphere들 전부 끄고
	SKM->SetSimulatePhysics(false);
	SKM->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SKM->SetVisibility(false);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NoticeSphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 아이템 상태 변경하기 Free -> Equipped
	SetItemState(EItemState::EIS_UnEquipped);
	
}

// 변경된 무기 (New Weapon) : 이제 바꾸는 무기를 말함
void ABaseWeapon::SetCollisionWhenItemChangeON()
{
	// 물리, OverlapSphere들 다 켜고
	//SetCollisionWhenItemDivestiture();

	// 매쉬까지 다시 보이게 하기
	SKM->SetVisibility(true);
	
}

void ABaseWeapon::NoticeRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::NoticeRangeBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	// 이게 맞는지 잘 모르겠음... 부모 변수 그대로 끌어다 쓰는건데 이게 맞음?
	if(WeaponInfo && WeaponAttributes)
	{
		WeaponInfo->SetAmmo(WeaponAttributes->GetAmmoCount());
	}
}

void ABaseWeapon::IPickUpItem(ABaseItem* Item, APlayerCharacter* Player)
{
	if(Player->GetCharAttribute()->GetEquippedWeapon()) return;
	Super::IPickUpItem(Item, Player);
		
	Player->GetCharAttribute()->SetEquippedWeapon(this);
	ItemAcquisition(Player->GetMesh(), Player);
	DisableCustomDepth();
}

void ABaseWeapon::PlayFireSound()
{
	if(FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, FirePointVector);
	}
}

void ABaseWeapon::PlayFireMuzzleEffect()
{
	if(FireFlashMuzzle)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FireFlashMuzzle, FirePointVector);
	}
}

void ABaseWeapon::HitAndEffects(const FHitResult& HitResult, AActor* Player)
{
	// 피격 대상이 적이면 데미지를 입힌다.
	if(HitResult.bBlockingHit && HitResult.GetActor()->ActorHasTag(FName("Enemy")))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *HitResult.GetActor()->GetName());
		
		if(AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(HitResult.GetActor()))
		{
			const FDamageEvent DamageEvent;
			Enemy->GetHit(HitResult.ImpactPoint, Player, WeaponDamage);
		}
	}
	
	// 총 소리 플레이, 총구 화염, 총탄과 비행운 생성
	PlayFireSound();
	PlayFireMuzzleEffect();
	FireEffectPlay(HitResult);
}

// Play FireSound and FireFlash 
void ABaseWeapon::GunFire(const FHitResult &HitResult, AActor* Player)
{
	if(!this->GetWeaponAttributes()->GetAmmoCount()) return;
	// 총구 위치 구하기
	FirePointVector = FirePoint->GetComponentLocation();


	// 총구와 사격 지점 사이에 물체가 있는지 확인하는 LineTrace
	FHitResult WeaponHitResult;
	FireStartPoint = FirePointVector;
	FireEndPoint = HitResult.Location;


	// SlaveAI 블루프린트를 위한 Case  
	if(!HitResult.bBlockingHit)
	{
		FireEndPoint = FireStartPoint + GetActorForwardVector() * 1500.f;
	}

	// 총구 위치 - 사격 End Point 사이에 물체가 있으면 WeaponHitResult에 찍히게 될 것임.
	// 크로스헤어에서 발사한 LineTrace와 총구에서 발사한 LineTrace의 ImpactPoint가 같지 않다면, 최종 LineTrace의 도착 위치를 총구에서 발사한 ImpactPoint로 바꿔주어야함!
	GetWorld()->LineTraceSingleByChannel(WeaponHitResult, FireStartPoint, FireEndPoint, ECC_Visibility);

	// 총구와 사격 지점사이에 물체가 있다면, TargetPoint를 바꿔주고 그렇지 않다면 원래 위치에 사격 효과를 준다.

	// 둘 사이에 물체가 없다면 기존 HitResult를 넘기고, 물체가 있다면 WeaponHitResult를 넘김 
	if(WeaponHitResult.bBlockingHit && WeaponHitResult.ImpactPoint != HitResult.ImpactPoint)
	{
		FireEndPoint = WeaponHitResult.ImpactPoint;
		HitAndEffects(WeaponHitResult, Player);
		UE_LOG(LogTemp, Error, TEXT("물체가 있음. 그 물체의 이름은 : %s"), *WeaponHitResult.GetActor()->GetName());
	}
	else
	{
		HitAndEffects(HitResult, Player);
	}

	DrawDebugLine(GetWorld(), FireStartPoint, FireEndPoint, FColor::Blue, false, 1.5f);

	// 총알 개수 감소
	DecreaseBulletCount();
	
}

void ABaseWeapon::GunFireForAI(FHitResult& HitResult, AActor* Shooter, FVector AimingPoint)
{
	FireStartPoint = FirePoint->GetComponentLocation();
	const FVector TargetPoint = (AimingPoint - FireStartPoint).GetSafeNormal();
	GetWorld()->LineTraceSingleByChannel(HitResult, FireStartPoint, AimingPoint, ECC_Visibility);
	DrawDebugLine(GetWorld(), FireStartPoint, AimingPoint, FColor::Orange, false, 1.5f);

	if(HitResult.bBlockingHit)
		HitAndEffects(HitResult, Shooter);
}

void ABaseWeapon::FireEffectPlay(const FHitResult& HitResult)
{
	/*if(HitResult.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *HitResult.GetActor()->GetName());
	}*/
	
	// 적이 아닌 물체에 총알이 부딪혔을 때만 Particle 생성.
	if(ShootParticle && HitResult.bBlockingHit)
	{
		if(HitResult.GetActor() && HitResult.GetActor()->ActorHasTag(TEXT("Enemy")))
		{
			// DrawDebugSphere(GetWorld(), HitResult.Location, 15.f, 16, FColor::Red, false, 5.f);
		}
		else
		{
			// UGameplayStatics::SpawnEmitterAtLocation(this, ShootParticle, FireEndPoint);
			UGameplayStatics::SpawnEmitterAtLocation(this, ShootParticle, HitResult.ImpactPoint);
			/*const FString ActorName = HitResult.GetActor()->GetName();
			UE_LOG(LogTemp, Error, TEXT("%s"), *ActorName);*/
			// DrawDebugSphere(GetWorld(), HitResult.Location, 15.f, 16, FColor::Blue, false, 5.f);
		}
	}
	else if(!HitResult.bBlockingHit)
	{
		UE_LOG(LogTemp, Error, TEXT("저는 부딪힌 적이 없습니다."));
	}
	
	if(BeamParticle && HitResult.bBlockingHit)
	{
		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticle, FirePointVector);
		if(Beam)
		{
			Beam->SetVectorParameter(FName("Target"), FireEndPoint);
		}
	}
}

void ABaseWeapon::DecreaseBulletCount()
{
	if(WeaponAttributes)
	{
		// 총알 한 개 감소
		WeaponAttributes->DecreaseAmmoCount();
	}
}