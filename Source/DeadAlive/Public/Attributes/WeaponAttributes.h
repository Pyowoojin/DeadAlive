// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AmmoType.h"
#include "WeaponAttributes.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADALIVE_API UWeaponAttributes : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWeaponAttributes();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FORCEINLINE float GetFireRate() const { return AutomaticFireRate; }
	FORCEINLINE FString GetItemName() { return ItemName; }
	FORCEINLINE void SetRemainedAmmoCount(int32 RefillAmmoCount) { RemainedBulletCount += RefillAmmoCount; } 
	FORCEINLINE int32 GetAmmoCount() const { return RemainedBulletCount; }
	// 최대 총알 - 현재 총알
	FORCEINLINE int32 GetNeedAmmoCount() const { return MaxBulletCount - RemainedBulletCount; }
	// 총 종류를 Return하는 함수
	FORCEINLINE EAmmoType GetWeaponType() const { return AmmoType; }
	void DecreaseAmmoCount();

	// 현재 총알이 최대 총알 개수인지 반환하는 함수 -> 최대라면 True 반환
	FORCEINLINE bool IsFullAmmo() const { return MaxBulletCount == RemainedBulletCount; }
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	private :
		float GunDamage = 5.f;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Atrributes", meta = (AllowPrivateAccess = "true"))
	int32 RemainedBulletCount = 30;
	int32 MaxBulletCount = 30;
	float AutomaticFireRate = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Atrributes", meta = (AllowPrivateAccess = "true"))
	FString ItemName = FString(TEXT("Default"));

	// 웨폰 타입에 따라서 다른 총알을 넣어줄 것임. AssaultRifle = 5.56탄, BattleRifle = 7.62탄 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Atrributes", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType = EAmmoType::EAT_AssaultRifle;
	
};
