#include "HUD/PlayerWeaponHUDWidget.h"
#include "Components/TextBlock.h"

void UPlayerWeaponHUDWidget::SetWeaponAmmoCount(int32 MaxAmmoCount)
{
	const FText TextAmount = FText::FromString(FString::Printf(TEXT("%d"), MaxAmmoCount));
	if(WeaponAmmoText)
		WeaponAmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d"), MaxAmmoCount)));
}

void UPlayerWeaponHUDWidget::SetCarriedAssaultAmmoText(int32 CurAmmoCount)
{

	const FText TextAmount = FText::FromString(FString::Printf(TEXT("%d"), CurAmmoCount));
	if(AssaultRifleText)
		AssaultRifleText->SetText(TextAmount);
}

void UPlayerWeaponHUDWidget::SetCarriedCombatAmmoText(int32 CurAmmoCount)
{
	const FText TextAmount = FText::FromString(FString::Printf(TEXT("%d"), CurAmmoCount));
	if(CombatRifleText)
		CombatRifleText->SetText(TextAmount);
}

void UPlayerWeaponHUDWidget::SetCurrentAmmoNameText(FString AmmoName)
{
	if(EquippedWeaponNameText)
		EquippedWeaponNameText->SetText(FText::FromString(AmmoName));
}
