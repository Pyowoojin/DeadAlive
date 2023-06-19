#include "Items/Ammunition.h"
#include "Attributes/CharacterAttribute.h"
#include "HUD/WeaponInfoWidget.h"
#include "Attributes/WeaponAttributes.h"
#include "PlayerCharacter.h"

AAmmunition::AAmmunition()
{
	Skm->SetSimulatePhysics(true);
	ItemAttributes = CreateDefaultSubobject<UWeaponAttributes>("ItemAttributes");
}

void AAmmunition::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

// 위젯에 아이템 이름 설정
void AAmmunition::SetItemNameForWidget() const
{
	if(WeaponInfo && ItemAttributes)
	{
		WeaponInfo->SetItemName(ItemAttributes->GetItemName());
	}
}

void AAmmunition::PickUpItem(APlayerCharacter* Player)
{
	Super::PickUpItem(Player);
	Player->GetCharAttribute()->SetAmmoCountForAmmoType(ItemAttributes->GetWeaponType(), ItemAttributes->GetAmmoCount());
	
	Destroy();
}

void AAmmunition::BeginPlay()
{
	Super::BeginPlay();
	SetItemNameForWidget();
}
