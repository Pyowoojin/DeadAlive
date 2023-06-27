#include "Enemy/EnemyCharacter.h"

#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	const float RealDamage =  Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	return RealDamage;
	
}

void AEnemyCharacter::GetHit(const FVector& ImpactPoint, AActor* Hitter, const float TakenDamage)
{
	const FDamageEvent DamageEvent;
	UE_LOG(LogTemp, Error, TEXT("%f"), ImpactPoint.X);
	
	Health -= TakeDamage(TakenDamage, DamageEvent, Hitter->GetInstigatorController(), Hitter);
	if(Health <= 0.f)
	{
		Destroy();
	}
}
