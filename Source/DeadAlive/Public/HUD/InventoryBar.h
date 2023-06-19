// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryBar.generated.h"

class ABaseWeapon;
class UWeaponSlot;
/**
 * 
 */
UCLASS()
class DEADALIVE_API UInventoryBar : public UUserWidget
{
	GENERATED_BODY()
public :
	UInventoryBar(const FObjectInitializer &ObjectInitializer);
	void InsertWeapon(ABaseWeapon* Weapon);
	void InitSlot(const UInventoryBar* InventoryBar);
	void RefreshSlot();

private :
	int32 MaxSlotSize = 6;
	UPROPERTY(VisibleAnywhere, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	TArray<UWeaponSlot*> SlotArray;
	bool *SlotCheck = new bool[MaxSlotSize];

	UPROPERTY(VisibleAnywhere, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	UWeaponSlot* DefaultWeaponSlott;

	UPROPERTY(VisibleAnywhere, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	UWeaponSlot* WeaponSlot01;

	UPROPERTY(VisibleAnywhere, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	UWeaponSlot* WeaponSlot02;
	
	UPROPERTY(VisibleAnywhere, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	UWeaponSlot* WeaponSlot03;

	UPROPERTY(VisibleAnywhere, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	UWeaponSlot* WeaponSlot04;
	
	UPROPERTY(VisibleAnywhere, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	UWeaponSlot* WeaponSlot05;
};
