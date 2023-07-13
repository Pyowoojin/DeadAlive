#include "Attributes/CharacterAttribute.h"

UCharacterAttribute::UCharacterAttribute()
{
	PrimaryComponentTick.bCanEverTick = false;
	Health = 100.f;
	MaxHealth = 100.f;
}
void UCharacterAttribute::BeginPlay()
{
	Super::BeginPlay();
	
}
void UCharacterAttribute::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCharacterAttribute::SetOverlapItem(ABaseItem* Item)
{
	OverlappedItem = Item;
}

ABaseItem* UCharacterAttribute::GetOverlappedItem()
{
	return OverlappedItem;
}

void UCharacterAttribute::SetAmmoCountForAmmoType(EAmmoType AmmoType, int32 Count)
{
	if(AmmoMap.Contains(AmmoType))
	{
		AmmoMap[AmmoType] += Count;
	}
}

int32 UCharacterAttribute::GetAmmoCountByAmmoType(EAmmoType AmmoType)
{
	if(AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType];
	}
	return 0;
}

// 모든 종류의 총알 개수를 갱신함
void UCharacterAttribute::RefreshAllTypeOfAmmo()
{
	for(auto& Element : AmmoMap)
	{
		
	}
}

bool UCharacterAttribute::HasAmmo(EAmmoType AmmoType) const
{
	if(AmmoMap.Contains(AmmoType) && AmmoMap[AmmoType] > 0)
		return true;
	return false;
}

void UCharacterAttribute::InitializeAmmo()
{
	AmmoMap.Add(EAmmoType::EAT_AssaultRifle, Ammo556);
	AmmoMap.Add(EAmmoType::EAT_CombatRifle, Ammo762);
}

