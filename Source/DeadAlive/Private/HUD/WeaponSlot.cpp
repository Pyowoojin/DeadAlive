// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WeaponSlot.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

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

void UWeaponSlot::RefreshSlot()
{
}
