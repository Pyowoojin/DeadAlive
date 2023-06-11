#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponInfoWidget.generated.h"

class UTextBlock;
class UImage;
UCLASS()
class DEADALIVE_API UWeaponInfoWidget : public UUserWidget
{
	GENERATED_BODY()
public :
	void SetAmmo(int8 Amount);
	void SetItemName(const FString& Name);
	void SetStarIcon(int8 StarCount);

private :
	UPROPERTY(meta =(BindWidget))
	UTextBlock* ItemName;

	UPROPERTY(meta =(BindWidget))
	UTextBlock* Ammo;

	UPROPERTY(meta =(BindWidget))
	UImage* Star1Icon;

	UPROPERTY(meta =(BindWidget))
	UImage* Star2Icon;

	UPROPERTY(meta =(BindWidget))
	UImage* Star3Icon;

	UPROPERTY(meta =(BindWidget))
	UImage* Star4Icon;

	UPROPERTY(meta =(BindWidget))
	UImage* Star5Icon;
};
