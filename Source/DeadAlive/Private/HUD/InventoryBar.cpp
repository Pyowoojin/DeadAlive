// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/InventoryBar.h"

#include "Attributes/WeaponAttributes.h"
#include "Components/TextBlock.h"
#include "HUD/WeaponSlot.h"
#include "Items/BaseWeapon.h"

UInventoryBar::UInventoryBar(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
	// 슬롯 체크 배열 초기화
	for(int i = 0; i< MaxSlotSize; i++)
	{
		SlotCheck[i] = false;
	}
}

void UInventoryBar::InsertWeapon(ABaseWeapon* Weapon)
{
	WeaponArray[Pointer].Weapon = Weapon;
	WeaponArray[Pointer].WeaponSlot->RefreshSlot(Weapon);
	RefreshSlot(Pointer);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Weapon->GetWeaponAttributes()->GetItemName());
}

void UInventoryBar::InitSlot(const UInventoryBar* InventoryBar)
{
	// overlay 슬롯들 매핑시켜주기
	
	DefaultWeaponSlott = Cast<UWeaponSlot>(InventoryBar->GetWidgetFromName(TEXT("DefaultWeaponSlot")));
	WeaponSlot01 = Cast<UWeaponSlot>(InventoryBar->GetWidgetFromName(TEXT("WeaponSlot1")));
	WeaponSlot02 = Cast<UWeaponSlot>(InventoryBar->GetWidgetFromName(TEXT("WeaponSlot2")));
	WeaponSlot03 = Cast<UWeaponSlot>(InventoryBar->GetWidgetFromName(TEXT("WeaponSlot3")));
	WeaponSlot04 = Cast<UWeaponSlot>(InventoryBar->GetWidgetFromName(TEXT("WeaponSlot4")));
	WeaponSlot05 = Cast<UWeaponSlot>(InventoryBar->GetWidgetFromName(TEXT("WeaponSlot5")));

	WeaponArray.Add({WeaponSlot01, nullptr});
	WeaponArray.Add({WeaponSlot02, nullptr});
	WeaponArray.Add({WeaponSlot03, nullptr});
	WeaponArray.Add({WeaponSlot04, nullptr});
	WeaponArray.Add({WeaponSlot05, nullptr});
	

	UE_LOG(LogTemp, Warning, TEXT("%d"), WeaponArray.Num());
	
	TextSlotArray.Add(Weapon1Text);
	TextSlotArray.Add(Weapon2Text);
	TextSlotArray.Add(Weapon3Text);
	TextSlotArray.Add(Weapon4Text);
	TextSlotArray.Add(Weapon5Text);
	TextSlotArray[0]->SetColorAndOpacity(FLinearColor::Red);
	
	for(int i = 0; i < WeaponArray.Num(); i++)
	{
		if(WeaponArray[i].WeaponSlot != nullptr)
		{
			WeaponArray[i].WeaponSlot->Init();
			SlotCheck[i] = true;
		}
	}
}

void UInventoryBar::SetPointerLocation(const int32 Location)
{
	TextSlotArray[Pointer-1]->SetColorAndOpacity(FLinearColor::White);
	Pointer = FMath::Clamp(Location, 0, MaxSlotSize-1);
	
	TextSlotArray[Pointer-1]->SetColorAndOpacity(FLinearColor::Red);
}

void UInventoryBar::IPickUpItem(ABaseWeapon* Item)
{
	IPickupInterface::IPickUpItem(Item);
	UE_LOG(LogTemp, Warning, TEXT("InventoryBar 진입!"));
}

void UInventoryBar::RefreshSlot(int32 num)
{
	
}
