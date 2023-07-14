// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponSlot.generated.h"

/**
 * 
 */

class UTextBlock;
class UImage;

UCLASS()
class DEADALIVE_API UWeaponSlot : public UUserWidget
{
	GENERATED_BODY()

public :
	/*FORCEINLINE int8 GetUniqueNumber() const { return UniqueNum; }
	FORCEINLINE void SetUniqueNumber(int8 SetNum) { UniqueNum = SetNum; }*/

	void Init();
	void RefreshSlot(class ABaseWeapon* Weapon);
	void RefreshAmmoCount(const int32 Num);
	void ClearSlot();
	
private :
	UPROPERTY(VisibleAnywhere, Category = Custom, meta = (AllowPrivateAccess = "true"))
	int8 SlotPointer = 0;

	// 무기 배경 이미지 -> 등급을 나타냄
	UPROPERTY(meta =(BindWidget))
	UImage* BackGroundImage;

	// 무기 이미지 -> 해당 무기의 이미지를 나타냄
	UPROPERTY(meta =(BindWidget))
	UImage* WeaponIcon;

	// 해당 무기의 총알 개수
	UPROPERTY(meta =(BindWidget))
	UTextBlock* WeaponAmmoCount;

	// 탄피 종류 이미지
	UPROPERTY(meta =(BindWidget))
	UImage* AmmoIcon;
	
};
