#pragma once

#include "CoreMinimal.h"
#include "AmmoType.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Interfaces/PickupInterface.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UCharacterAttribute;
class UAnimMontage;
class ABaseWeapon;
class UPlayerWeaponHUDWidget;
class UUserWidget;
class USoundWave;
class UWeaponSlot;
class UInventoryBar;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_Max UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class DEADALIVE_API APlayerCharacter : public ACharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	FORCEINLINE bool GetAiming() const { return bAiming; }
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	
	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;
	void CalculateCrosshairSpread(float DeltaTime);
	bool TraceUnderCrosshairs(FHitResult& HitResult);
	
	// 주변에 있는 아이템의 개수를 구하여 0개라면 매 프레임 돌아가는 LineTrace 실행을 멈출 것이다.
	void IncrementOverlappedItemCount(int8 Amount);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<ABaseWeapon> DefaultWeaponClass;

	void FireMontage();

	// Getter함수 //
	FORCEINLINE UCharacterAttribute* GetCharAttribute() const { return CharAttribute; }
	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

protected:
	virtual void BeginPlay() override;
	void TraceForItems();
	virtual void OverlappingItem(class ABaseItem* Item) override;
	// virtual void OverlappingItem_Implementation(class ABaseItem* Item) override;
	void CameraAiming(float DeltaTime);

	/* 입력 콜백 함수 */
	void CharMove(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void RefreshAllTypeOfAmmoWidget();
	void EKeyPressed();
	void RunKeyPressed(const FInputActionValue& Value);
	void AimKeyPressed(const FInputActionValue& Value);
	void ShotKeyPressed(const FInputActionValue& Value);
	void NumKey1Pressed();
	void NumKey2Pressed();
	void NumKey3Pressed();
	void NumKey4Pressed();
	void NumKey5Pressed();
	
	/* 입력 함수 END*/

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* MySpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* MainCam;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCharacterAttribute* CharAttribute;

	// 마지막 프레임에 도달한 항목을 참조하는 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	class ABaseItem* TraceHitLastFrame;

	void StartCrosshairBulletFire();
	
	UFUNCTION()
	void FinishCrosshairBulletFire();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState = ECombatState::ECS_Unoccupied;
	
	bool CanIShoot() const;
	void SetWeapon(ABaseWeapon* Item);
	bool SpeedLimitFunction();
	void GetViewPortSize();
	bool GetScreenCenterToConvertWorld();
	void FireButtonPressed();
	void FireButtonReleased();
	void StartFireTimer();
	UFUNCTION()
	void AutoFireReset();
	void SpawnDefaultWeapon();

	void ShootingModeChange();

	// 재장전 함수
	void ReloadButtonPressed();
	void ReloadWeapon();
	int32 HowManyNeedBulletToReload(EAmmoType AmmoType, int32 NeedAmmo);
	void RefreshTheCurrentAmmoWidget();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	// 탄창 애니메이션을 위한 함수
	UFUNCTION(BlueprintCallable)
	void GrabClip();

	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

	// 탄창 애니메이션을 위한 함수 END

	UPROPERTY(EditAnywhere, Category = "Movement")
	float LimitedSpeed;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float GroundSpeed;

	/* 입력 매핑 컨텍스트 & 입력 액션*/
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InputMove;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InputEquip;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InputJump;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InputLook;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InputRun;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InputAim;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InputShot;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InputShotChange;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InputReload;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* NumKey1;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* NumKey2;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* NumKey3;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* NumKey4;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* NumKey5;

	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* InputMapping;

	// 애니메이션 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HipFireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;

	/* 총 발사 관련 변수 및 함수들 */
	FVector2D ViewportSize;
	bool bScreenToWorld = false;
	FVector CrossHairWorldPosition;
	FVector CrossHairWorldDirection;
	float WeaponRange = 50000.f;
	
	bool bAiming = false;
	bool bFireButtonPressed = false;
	bool bFireButtonReleased = true;
	bool bShouldFire = true;
	// 단발, 연사 조절 Bool 값, false일 경우 조정간 단발, true일경우 조정간 자동
	bool bShootingMode = true;

	// 탄창의 처음 위치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform ClipTransform;

	// 장전 할 때 동안 손에 부착할 씬 컴포넌트 -> 탄창을 부착할 위치임
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;

	// 카메라 기본 필드 오브 뷰
	float DefaultCameraFOV = 0.f; // set in BeginPlay
	// 줌 인 카메라 필드 오브 뷰
	float ZoomedCameraFOV = 45.f; //

	// 캐릭터 기본 마우스 감도와 조준(오른쪽버튼)시의 마우스 감도를 조절하기 위한 변수
	UPROPERTY(VisibleAnywhere)
	float BaseTurnRate = 1.f;
	UPROPERTY(VisibleAnywhere)
	float AimTurnRate = 0.35f;
	float DefaultTurnRate = 1.f;

	// 이번 프레임의 줌 위치
	float CurrentCameraFOV = 0.f;

	// 줌 속도 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed;

	// 크로스헤어 스프레드 관련
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshair",meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshair",meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshair",meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshair",meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor = 0.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshair",meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor = 0.f;

	int8 OverlappedItemCount = 0;
	bool bShouldTraceItemInfo = false;
	
	float ShootTimeDuration = 0.05f;
	bool bFiringBullet = false;
	FTimerHandle CrosshairShootTimer;
	FTimerHandle AutoFireTimer;

	// 무기를 습득할 때 인벤토리 바로 슬롯 정보를 보내기 위한 델리게이트
	UPROPERTY(BlueprintAssignable, Category = Delegates, meta = (AllowPrivateAccess = "true"))
	FEquipItemDelegate EquipItemDelegate;

	// HUD에 대한 레퍼런스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SLATE_CULL_WIDGETS, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> HUDOverlayClass;

	// HUD Overlay를 생성한 뒤 화면에 유지함
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SLATE_CULL_WIDGETS, meta = (AllowPrivateAccess = "true"))
	UUserWidget* HUDOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SLATE_CULL_WIDGETS, meta = (AllowPrivateAccess = "true"))
	UPlayerWeaponHUDWidget* WeaponHUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SLATE_CULL_WIDGETS, meta = (AllowPrivateAccess = "true"))
	UInventoryBar* InventoryBar;
};