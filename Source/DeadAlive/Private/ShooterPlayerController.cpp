#include "ShooterPlayerController.h"
#include "HUD/PlayerWeaponHUDWidget.h"
#include "Blueprint/UserWidget.h"

AShooterPlayerController::AShooterPlayerController()
{
	
}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if(HUDOverlayClass)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayClass);
		if(HUDOverlay)
		{
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
			WeaponHUDWidget = Cast<UPlayerWeaponHUDWidget>(HUDOverlay->GetWidgetFromName(TEXT("BP_AmmoCount2")));
			if(WeaponHUDWidget)
			{
				UE_LOG(LogTemp, Warning, TEXT("캐스팅 성공"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("캐스팅 실패"));
			}
			
		}
	}
}