// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WeaponSlot.h"

#include "Attributes/WeaponAttributes.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "HUD/InventoryBar.h"
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
	// BackGroundImage->Brush = Weapon->GetWeaponIcon();

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

	

	UE_LOG(LogTemp, Warning, TEXT("Refresh"));
	
	/*
	if(BackGroundImage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Image 형식으로 캐스팅 성공!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Image 형식으로 캐스팅 실패.."));
	}*/
}

void UWeaponSlot::ClearSlot()
{
	
}
