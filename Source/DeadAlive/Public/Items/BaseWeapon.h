#pragma once

#include "CoreMinimal.h"
#include "Interfaces/PickupInterface.h"
#include "Items/BaseItem.h"
#include "BaseWeapon.generated.h"

class UTextBlock;
class UImage;
class UWeaponSlot;
class UWeaponInfoWidget;
class UPrimitiveComponent;
class UWeaponAttributes;
class USoundBase;
class UGameplayStatics;
class UParticleSystem;

UCLASS()
class DEADALIVE_API ABaseWeapon : public ABaseItem // IPickUpInterface
{
	GENERATED_BODY()

public :

	//virtual void IPickUpItem(ABaseWeapon* Weapon) override;
	
	ABaseWeapon();
	virtual void BeginPlay() override;
	void PlayFireSound();
	void PlayFireMuzzleEffect();
	void HitAndEffects(const FHitResult& HitResult, AActor* Player);
	void FireEffectPlay(const FHitResult& HitResult);
	void GunFire(const FHitResult &HitResult, AActor* Player);
	void DecreaseBulletCount();
	FORCEINLINE UWeaponAttributes* GetWeaponAttributes() const { return WeaponAttributes; }
	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; }

	void ItemAcquisition(USceneComponent* InParent, AActor* NewOwner);
	void ItemDivestiture();
	void SetCollisionWhenItemDivestiture();
	void SetCollisionWhenItemChangeON();
	void SetCollisionWhenItemChangeOFF();
	FORCEINLINE FName GetClipBoneName() const { return ClipBoneName; }
	FORCEINLINE void SetMovingClip(bool Move) { bMovingClip = Move; }
	FORCEINLINE int32 GetWeaponDamage() const { return WeaponDamage; }

	// Slot HUD를 위한 함수

	FORCEINLINE UTexture2D* GetBackgroundImage() const { return BackgroundImage; }
	FORCEINLINE UTexture2D* GetAmmoImage() const { return AmmoImage; }
	FORCEINLINE UTexture2D* GetWeaponIcon() const { return WeaponIcon; }
	FORCEINLINE FName GetAmmoCount() const { return AmmoCount; }

protected:
	
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* FirePoint;

	UPROPERTY(VisibleAnywhere)
	UWeaponAttributes* WeaponAttributes;

	UPROPERTY(EditAnywhere, Category = "Gun Properties")
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ShootParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticle;
	
	virtual void NoticeRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
// 	virtual void PickUpItem(APlayerCharacter* Player) override;
	virtual void IPickUpItem(ABaseItem* Item, APlayerCharacter* Player) override;

	// 총 발사 관련 변수
	FVector FirePointVector;
	FVector FireStartPoint;
	FVector FireEndPoint;

	UPROPERTY(EditAnywhere, Category = VFX)
	class UNiagaraSystem* FireFlashMuzzle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FName ReloadMontageSection = "ReloadAR4";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FName ClipBoneName = TEXT("b_gun_mag");

	// 움직이고 있는지를 확인하는 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess ="true"))
	bool bMovingClip = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 WeaponDamage = 15;

	// WeaponSlot을 위한 변수들

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	UTexture2D* WeaponIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	UTexture2D* BackgroundImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	FName AmmoCount;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoImage;
};