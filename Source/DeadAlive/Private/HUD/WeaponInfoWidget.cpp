#include "HUD/WeaponInfoWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UWeaponInfoWidget::SetItemName(const FString& Name)
{
	if(ItemName)
	{
		ItemName->SetText(FText::FromString(Name));
	}
}

void UWeaponInfoWidget::SetStarIcon(int8 StarCount)
{
	switch(StarCount)
	{
	case 1:
		Star1Icon->SetOpacity(1);
		break;
	case 2:
		Star1Icon->SetOpacity(1);
		Star2Icon->SetOpacity(1);
		break;
	case 3:
		Star1Icon->SetOpacity(1);
		Star2Icon->SetOpacity(1);
		Star3Icon->SetOpacity(1);
		break;
	case 4:
		Star1Icon->SetOpacity(1);
		Star2Icon->SetOpacity(1);
		Star3Icon->SetOpacity(1);
		Star4Icon->SetOpacity(1);
		break;
	case 5:
		Star1Icon->SetOpacity(1);
		Star2Icon->SetOpacity(1);
		Star3Icon->SetOpacity(1);
		Star4Icon->SetOpacity(1);
		Star5Icon->SetOpacity(1);
		break;
	}
}

void UWeaponInfoWidget::SetAmmo(int8 Amount)
{
	if(Ammo)
	{
		const FText TextAmount = FText::FromString(FString::Printf(TEXT("%d"), Amount));
		Ammo->SetText(TextAmount);
	}
}
