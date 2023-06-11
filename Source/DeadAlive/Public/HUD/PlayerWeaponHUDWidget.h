#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWeaponHUDWidget.generated.h"

class UTextBlock;
UCLASS()
class DEADALIVE_API UPlayerWeaponHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetWeaponAmmoCount(int32 MaxAmmoCount);
	void SetCarriedAssaultAmmoText(int32 CurAmmoCount);
	void SetCarriedCombatAmmoText(int32 CurAmmoCount);
	void SetCurrentAmmoNameText(FString AmmoName);
	
	private :
	UPROPERTY(meta =(BindWidget))
	UTextBlock* WeaponAmmoText;

	UPROPERTY(meta =(BindWidget))
	UTextBlock* AssaultRifleText;

	UPROPERTY(meta =(BindWidget))
	UTextBlock* CombatRifleText;
	
	UPROPERTY(meta =(BindWidget))
	UTextBlock* EquippedWeaponNameText;

};