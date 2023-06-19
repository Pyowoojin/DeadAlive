// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/BaseItem.h"
#include "BaseWeapon.generated.h"

class UWeaponSlot;
class UWeaponInfoWidget;
class UPrimitiveComponent;
class UWeaponAttributes;
class USoundBase;
class UGameplayStatics;
class UParticleSystem;

UCLASS()
class DEADALIVE_API ABaseWeapon : public ABaseItem
{
	GENERATED_BODY()

public :
	ABaseWeapon();
	virtual void BeginPlay() override;
	void PlayFireSound();
	void PlayFireMuzzleEffect();
	void FireEffectPlay(const FHitResult& HitResult);
	void GunFire(const FHitResult &HitResult);
	void DecreaseBulletCount();
	FORCEINLINE UWeaponAttributes* GetWeaponAttributes() const { return WeaponAttributes; }
	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; }

	void ItemAcquisition(USceneComponent* InParent, AActor* NewOwner);
	void ItemDivestiture();
	void SetCollisionWhenItemDivestiture();
	void SetCollisionWhenItemChange();
	FORCEINLINE FName GetClipBoneName() const { return ClipBoneName; }
	FORCEINLINE void SetMovingClip(bool Move) { bMovingClip = Move; }

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
	virtual void PickUpItem(APlayerCharacter* Player) override;

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
	UWeaponSlot* WeaponSlot;
};