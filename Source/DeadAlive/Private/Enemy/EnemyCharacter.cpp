#include "Enemy/EnemyCharacter.h"
#include "AIController.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/DamageEvents.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/GameplayStatics.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComponent");
	
	PawnSensingComponent->SetPeripheralVisionAngle(65.f);
	PawnSensingComponent->SightRadius = 6000.f;
	
}
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	EnemyInitialize();
	MoveToPoint(TargetPoint);
}

void AEnemyCharacter::EnemyInitialize()
{
	AIController = Cast<AAIController>(GetController());
	if(PawnSensingComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("초기화 완료"));
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemyCharacter::PlayerDetected);
	}
	
}

void AEnemyCharacter::PlayerDetected(APawn* TargetActor)
{
	if(EnemyState == EEnemyState::EES_Dead) return;
		AIController->MoveToActor(TargetActor, AcceptanceRadiusMax);
}

void AEnemyCharacter::ChangeTarget(APawn* TargetActor)
{
	if(AIController)
	{
		// 현재 움직임을 멈추고 타겟을 변경함
		AIController->StopMovement();
		TargetPoint = TargetActor;
		AIController->MoveToActor(TargetActor);
	}
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
	// UE_LOG(LogTemp, Warning, TEXT("Move to Point 실행"));
	if(AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(GoalActor);
		MoveRequest.SetAcceptanceRadius(AcceptanceRadiusMax);
		AIController->MoveTo(MoveRequest);
	}
}

void AEnemyCharacter::PlayAnimation(UAnimMontage* AnimMontage)
{
	if(UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(AnimMontage);
		// AnimInstance->Montage_JumpToSection(FName("Dying3"));
	}
}

void AEnemyCharacter::StopMovement() const
{
	// GetCharacterMovement()->MaxWalkSpeed = 0.f;
	/*UE_LOG(LogTemp, Warning, TEXT("멈춰라"));
	if(AIController)
		AIController->StopMovement();*/
	GetController()->StopMovement();
}

bool AEnemyCharacter::GetIsDead() const
{
	return bIsDead;
}

void AEnemyCharacter::GetHit(const FVector& ImpactPoint, AActor* Hitter, const float TakenDamage)
{
	const FDamageEvent DamageEvent;

	
	if(const auto Target = Cast<APawn>(Hitter))
	{
		ChangeTarget(Target);
	}
	
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

	if(HitAnimMontage)
	{
		PlayAnimation(HitAnimMontage);
	}
	
	Health -= TakeDamage(TakenDamage, DamageEvent, Hitter->GetInstigatorController(), Hitter);
	if(Health <= 0.f && !bIsDead)
	{
		EnemyState = EEnemyState::EES_Dead;
		PlayAnimation(DeathAnimMontage);
		bIsDead = true;
		StopMovement();
		GetWorldTimerManager().SetTimer(DestroyTimer, this, &AEnemyCharacter::TempFunc, 1.4f);
	}
}
