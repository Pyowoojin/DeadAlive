// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/PickupInterface.h"
#include "InventoryBar.generated.h"

class ABaseWeapon;
class UWeaponSlot;
class UTextBlock;

USTRUCT(Atomic)
struct FArrayStruct
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	UWeaponSlot* WeaponSlot = nullptr;
	UPROPERTY(VisibleAnywhere)
	ABaseWeapon* Weapon = nullptr;
};

UCLASS()
class DEADALIVE_API UInventoryBar : public UUserWidget, public IPickupInterface
{
	GENERATED_BODY()
public :
	UInventoryBar(const FObjectInitializer &ObjectInitializer);
	void InsertWeapon(ABaseWeapon* Weapon);
	void InitSlot(const UInventoryBar* InventoryBar);
	void ClearSlot();
	void SetPointerLocation(int32 Location);
	FORCEINLINE int32 GetPointerLocation() const { return Pointer; }
	virtual void IPickUpItem(ABaseWeapon* Weapon) override;

private :
	UPROPERTY(VisibleAnywhere, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	int32 Pointer = 1;
	int32 MaxSlotSize = 5;
	
	UPROPERTY(VisibleAnywhere, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	TArray<UWeaponSlot*> SlotArray;

	UPROPERTY(VisibleAnywhere, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	TArray<FArrayStruct> WeaponArray;

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
