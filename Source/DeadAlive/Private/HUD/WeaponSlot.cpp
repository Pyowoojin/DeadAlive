// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WeaponSlot.h"

#include "Attributes/WeaponAttributes.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Items/BaseWeapon.h"
#include "Styling/SlateBrush.h"

void UWeaponSlot::Init()
{
	if(WeaponAmmoCount)
	{
		FText TextString = FText::FromString(FString::Printf(TEXT("0")));
		WeaponAmmoCount->SetText(TextString);
	}

	if(BackGroundImage)
	{
		BackGroundImage->SetVisibility(ESlateVisibility::Hidden);
	}
	if(WeaponIcon)
	{
		WeaponIcon->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UWeaponSlot::RefreshSlot(ABaseWeapon* Weapon)
{
	FSlateBrush Brush;
	Brush.SetResourceObject(Weapon->GetBackgroundImage());
	BackGroundImage->SetBrush(Brush);
	BackGroundImage->SetVisibility(ESlateVisibility::Visible);

	Brush.SetResourceObject(Weapon->GetWeaponIcon());
	WeaponIcon->SetBrush(Brush);
	WeaponIcon->SetVisibility(ESlateVisibility::Visible);

	Brush.SetResourceObject(Weapon->GetAmmoImage());
	AmmoIcon->SetBrush(Brush);
	AmmoIcon->SetVisibility(ESlateVisibility::Visible);


	const int32 WeaponAmmo = Weapon->GetWeaponAttributes()->GetAmmoCount();
	const FText TextAmount = FText::FromString(FString::Printf(TEXT("%d"), WeaponAmmo));
	WeaponAmmoCount->SetText(TextAmount);
}

void UWeaponSlot::RefreshAmmoCount(const int32 Num)
{
	const FText TextAmount = FText::FromString(FString::Printf(TEXT("%d"), Num));
	WeaponAmmoCount->SetText(TextAmount);
}

void UWeaponSlot::ClearSlot()
{
	
}
