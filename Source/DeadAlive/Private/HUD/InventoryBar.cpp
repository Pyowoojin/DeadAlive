// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/InventoryBar.h"
#include "Components/TextBlock.h"
#include "HUD/WeaponSlot.h"

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
	// 슬롯 사이즈가 최대 개수보다 작다면 그냥 집어넣음
	if(SlotArray.Num() < MaxSlotSize)
	{
		for(int i = 0; i < MaxSlotSize; i++)
		{
			// SlotArray.Add(Weapon);
		}
	}
	// 무기 칸이 꽉 찼다면 포인터가 가리키는 슬롯의 무기와 교체해준다.
	else if(SlotArray.Num() == MaxSlotSize)
	{
		
	}
}

void UInventoryBar::InitSlot(const UInventoryBar* InventoryBar)
{
	// overlay 슬롯들 매핑시켜주기
	
	DefaultWeaponSlott = Cast<UWeaponSlot>(InventoryBar->GetWidgetFromName(TEXT("DefaultWeaponSlot")));
	SlotArray.Add(DefaultWeaponSlott);
	WeaponSlot01 = Cast<UWeaponSlot>(InventoryBar->GetWidgetFromName(TEXT("WeaponSlot1")));
	SlotArray.Add(WeaponSlot01);
	WeaponSlot02 = Cast<UWeaponSlot>(InventoryBar->GetWidgetFromName(TEXT("WeaponSlot2")));
	SlotArray.Add(WeaponSlot02);
	WeaponSlot03 = Cast<UWeaponSlot>(InventoryBar->GetWidgetFromName(TEXT("WeaponSlot3")));
	SlotArray.Add(WeaponSlot03);
	WeaponSlot04 = Cast<UWeaponSlot>(InventoryBar->GetWidgetFromName(TEXT("WeaponSlot4")));
	SlotArray.Add(WeaponSlot04);
	WeaponSlot05 = Cast<UWeaponSlot>(InventoryBar->GetWidgetFromName(TEXT("WeaponSlot5")));
	SlotArray.Add(WeaponSlot05);
	
	TextSlotArray.Add(Weapon1Text);
	TextSlotArray.Add(Weapon2Text);
	TextSlotArray.Add(Weapon3Text);
	TextSlotArray.Add(Weapon4Text);
	TextSlotArray.Add(Weapon5Text);
	
	for(int i = 0; i < MaxSlotSize; i++)
	{
		if(i == Pointer)
		{
			TextSlotArray[i]->SetColorAndOpacity(FLinearColor::Red);
		}
		
		if(SlotArray[i])
		{
			SlotArray[i]->Init();
			SlotCheck[i] = true;
		}
	}
}

void UInventoryBar::RefreshSlot()
{
	
}
