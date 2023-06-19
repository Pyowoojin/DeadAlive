// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryBar.generated.h"

class ABaseWeapon;
class UWeaponSlot;
class UTextBlock;
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
	int32 Pointer = 0;
	int32 MaxSlotSize = 6;
	
	UPROPERTY(VisibleAnywhere, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	TArray<UWeaponSlot*> SlotArray;

	UPROPERTY(VisibleAnywhere, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	TArray<UTextBlock*> TextSlotArray;
	
	bool *SlotCheck = new bool[MaxSlotSize];

	UPROPERTY(VisibleAnywhere, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	UWeaponSlot* DefaultWeaponSlott;

	UPROPERTY(VisibleAnywhere, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	UWeaponSlot* WeaponSlot01;

	UPROPERTY(meta =(BindWidget))
	UTextBlock* Weapon1Text;

	UPROPERTY(VisibleAnywhere, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	UWeaponSlot* WeaponSlot02;

	UPROPERTY(meta =(BindWidget))
	UTextBlock* Weapon2Text;
	
	UPROPERTY(VisibleAnywhere, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	UWeaponSlot* WeaponSlot03;

	UPROPERTY(meta =(BindWidget))
	UTextBlock* Weapon3Text;

	UPROPERTY(VisibleAnywhere, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	UWeaponSlot* WeaponSlot04;

	UPROPERTY(meta =(BindWidget))
	UTextBlock* Weapon4Text;
	
	UPROPERTY(VisibleAnywhere, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	UWeaponSlot* WeaponSlot05;

	UPROPERTY(meta =(BindWidget))
	UTextBlock* Weapon5Text;
};
