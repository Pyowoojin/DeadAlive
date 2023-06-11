#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

UCLASS()
class DEADALIVE_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()
	public :
		AShooterPlayerController();

protected:
	virtual void BeginPlay() override;

	private :
		// HUD에 대한 레퍼런스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SLATE_CULL_WIDGETS, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> HUDOverlayClass;

	// HUD Overlay를 생성한 뒤 화면에 유지함
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SLATE_CULL_WIDGETS, meta = (AllowPrivateAccess = "true"))
	UUserWidget* HUDOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SLATE_CULL_WIDGETS, meta = (AllowPrivateAccess = "true"))
	class UPlayerWeaponHUDWidget* WeaponHUDWidget;
};
