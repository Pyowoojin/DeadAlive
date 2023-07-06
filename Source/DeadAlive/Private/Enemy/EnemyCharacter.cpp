#include "Enemy/EnemyCharacter.h"
#include "AIController.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

}
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	EnemyInitialize();
	MoveToPoint(TargetPoint);
	
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

FRotator AEnemyCharacter::ReturnRandomRotation() const
{
	const int32 TempX = FMath::RandRange(1, 359);
	const int32 TempY = FMath::RandRange(1, 359);
	const int32 TempZ = FMath::RandRange(1, 359);
	return FRotator(TempX, TempY, TempZ);
}

void AEnemyCharacter::MoveToPoint(AActor* GoalActor)
{
	if(AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(GoalActor);
		AIController->MoveTo(MoveRequest);
	}
}

void AEnemyCharacter::EnemyInitialize()
{
	AIController = Cast<AAIController>(GetController());
	
}

void AEnemyCharacter::PlayAnimation(UAnimMontage* AnimMontage)
{
	if(UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(AnimMontage);
		AnimInstance->Montage_JumpToSection(FName("Dying3"));
	}
}

void AEnemyCharacter::StopMovement() const
{
	// GetCharacterMovement()->MaxWalkSpeed = 0.f;
	GetController()->StopMovement();
}

bool AEnemyCharacter::GetIsDead() const
{
	return IsDead;
}

void AEnemyCharacter::GetHit(const FVector& ImpactPoint, AActor* Hitter, const float TakenDamage)
{
	const FDamageEvent DamageEvent;

	if(HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation(), GetActorRotation(), 0.35f);
	}

	if(BloodParticle)
	{
		// 랜덤 X, Y, Z 회전을 구하여 파티클로 생성해줄 것임.
		const FRotator RandRotator = ReturnRandomRotation();

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BloodParticle, ImpactPoint, RandRotator);
	}
	Health -= TakeDamage(TakenDamage, DamageEvent, Hitter->GetInstigatorController(), Hitter);
	if(Health <= 0.f && !IsDead)
	{
		PlayAnimation(DeathAnimMontage);
		IsDead = true;
		StopMovement();
		GetWorldTimerManager().SetTimer(DestroyTimer, this, &AEnemyCharacter::TempFunc, 1.4f);
	}
}
