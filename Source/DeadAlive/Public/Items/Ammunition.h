#pragma once

#include "CoreMinimal.h"
#include "Items/BaseItem.h"
#include "Ammunition.generated.h"

class UWeaponInfoWidget;

UCLASS()
class DEADALIVE_API AAmmunition : public ABaseItem
{
	GENERATED_BODY()
	public :
		AAmmunition();
	virtual void Tick(float DeltaSeconds) override;
	void SetItemNameForWidget() const;
	virtual void PickUpItem(APlayerCharacter* Player) override;

protected:
	virtual void BeginPlay() override;

	private :
		UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UWeaponAttributes* ItemAttributes = nullptr;
};
