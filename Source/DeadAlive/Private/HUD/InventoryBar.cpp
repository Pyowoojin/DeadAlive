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
	WeaponArray[Pointer-1].Weapon = Weapon;
	WeaponArray[Pointer-1].WeaponSlot->RefreshSlot(Weapon);
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
			WeaponArray[i].Weapon = nullptr;
			SlotCheck[i] = true;
		}
	}
}

// 해당 슬롯을 비워준다.
void UInventoryBar::ClearSlot()
{
	WeaponArray[Pointer-1].WeaponSlot->Init();
	WeaponArray[Pointer-1].Weapon = nullptr;
}

// 플레이어의 입력에 따라 (1,2,3...) 무기 슬롯을 가리키는 포인터의 위치를 지정해준다.
void UInventoryBar::SetPointerLocation(int32 Location)
{
	TextSlotArray[Pointer-1]->SetColorAndOpacity(FLinearColor::White);
	Pointer = FMath::Clamp(Location, 0, MaxSlotSize);
	
	TextSlotArray[Pointer-1]->SetColorAndOpacity(FLinearColor::Red);
	UE_LOG(LogTemp, Warning, TEXT(" 포인터의 위치는 = %d"), Pointer);
}

// 해당 포인터 슬롯에 무기가 있다면 True, 없다면 False 반환
bool UInventoryBar::IsWeaponInThisSlot()
{
	if(WeaponArray[Pointer-1].Weapon == nullptr)
		return false;
	return true;
}

void UInventoryBar::IPickUpItem(ABaseWeapon* Item)
{
	IPickupInterface::IPickUpItem(Item);
	UE_LOG(LogTemp, Warning, TEXT("InventoryBar 진입!"));
}
