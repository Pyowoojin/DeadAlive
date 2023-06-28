#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/BaseWeapon.h"
#include "AmmoType.h"
#include "CharacterAttribute.generated.h"

class ABaseItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADALIVE_API UCharacterAttribute : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterAttribute();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 아이템에 관한 함수들
	void SetOverlapItem(ABaseItem* Item);
	FORCEINLINE void SetEquippedWeapon(ABaseWeapon* Item) { EquippedWeapon = Item; }
	FORCEINLINE ABaseWeapon* GetEquippedWeapon() const { return EquippedWeapon;}
	ABaseItem* GetOverlappedItem();

	// 총알에 관한 함수들 Start
	// 모든 총알 개수 리턴
	FORCEINLINE int32 GetAllOfAmmoCount() const { return Ammo556 + Ammo762; }
	FORCEINLINE void SetAmmoCount(int32 AmmoCount) { Ammo556 += AmmoCount;}
	// 특정 총알의 개수를 늘리거나 줄임
	void SetAmmoCountForAmmoType(EAmmoType AmmoType, int32 Count);
	// 특정 총알 타입이 몇개 있는지 Return
	int32 GetAmmoCountByAmmoType(EAmmoType AmmoType);
	void RefreshAllTypeOfAmmo();
	bool HasAmmo(EAmmoType AmmoType);
	void InitializeAmmo();
	
protected:
	virtual void BeginPlay() override;

private :
	/* 캐릭터 스탯? */
	UPROPERTY(EditAnywhere, Category = "Character Attributes")
	float Health = 100.f;
	UPROPERTY(EditAnywhere, Category = "Character Attributes")
	float MaxHealth = 100.f;
	UPROPERTY(EditAnywhere, Category = "Character Attributes")
	float Stamina = 100.f;

	UPROPERTY(VisibleAnywhere)
	ABaseItem* OverlappedItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	ABaseWeapon* EquippedWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	int32 Ammo762 = 60;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	int32 Ammo556 = 60;
};
