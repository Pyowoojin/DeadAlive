#pragma once
UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_AssaultRifle UMETA(DisplayName = "5.56mm"),
	EAT_CombatRifle UMETA(DisplayName = "7.62mm"),
	EAT_MAX UMETA(DisplayName = "DefaultMax")
};