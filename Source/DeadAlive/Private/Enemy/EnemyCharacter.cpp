#include "Enemy/EnemyCharacter.h"
#include "AIController.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/DamageEvents.h"
#include "Perception/PawnSensingComponent.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComponent");
	
	PawnSensingComponent->SetPeripheralVisionAngle(35.f);
	PawnSensingComponent->SightRadius = 2500.f;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	
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
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemyCharacter::PlayerDetected);
		AIController->ReceiveMoveCompleted.AddDynamic(this, &AEnemyCharacter::DeleTest);
	}
	// 추격중인 상태일때는 다시 추격을 하지 않는다
}

void AEnemyCharacter::CreateNewPatrolJob()
{
	UE_LOG(LogTemp, Warning, TEXT("실행되었읍니다/."));
	FNavLocation DestNavLocation;
	const FVector Dest = CalcNextMovementLocation(DestNavLocation);
	ChangeState(EEnemyState::EES_Patrol);
	AIController->MoveToLocation(Dest, AcceptanceRadiusMax);
	ChangeSpeed(WalkSpeed);
	DrawDebugSphere(GetWorld(), Dest, 64.f, 32, FColor::Blue, false, 5.f);
}

void AEnemyCharacter::DeleTest(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if(Result == EPathFollowingResult::Success)
	{
		if(AIController)
		{
			GetWorld()->GetTimerManager().SetTimer(PatrolTimer, this, &AEnemyCharacter::CreateNewPatrolJob, 5.f);
			// CreateNewPatrolJob();
		}
	}
}

void AEnemyCharacter::PlayerDetected(APawn* TargetActor)
{
	if(EnemyState == EEnemyState::EES_Dead || EnemyState == EEnemyState::EES_Chasing) return;
	UE_LOG(LogTemp, Warning, TEXT("플레이어 디텍트"));
	
	AIController->MoveToActor(TargetActor, AcceptanceRadiusMax);
	ChangeSpeed(RunSpeed);
	ChangeState(EEnemyState::EES_Chasing);
}

void AEnemyCharacter::ChangeTarget(APawn* TargetActor)
{
	if(AIController && EnemyState != EEnemyState::EES_Dead)
	{
		TargetPoint = TargetActor;
		AIController->MoveToActor(TargetActor);
		ChangeState(EEnemyState::EES_Chasing);
	}
}

void AEnemyCharacter::ChangeSpeed(const float Speed) const
{
	this->GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void AEnemyCharacter::DecreaseSpeed(float DelayTime) const
{
	
}

FVector AEnemyCharacter::CalcNextMovementLocation(FNavLocation& DestLocation)
{
	const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	// const bool Success = UNavigationSystemV1::GetRandomPointInNavigableRadius(
	// const bool Success = NavSystem->GetRandomPointInNavigableRadius(
	const bool Success = NavSystem->GetRandomReachablePointInRadius(
		GetActorLocation(),
		PatrolRange,
		DestLocation);

	if(Success)
		return DestLocation.Location;

	return GetActorLocation();
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
	UE_LOG(LogTemp, Warning, TEXT("Move to Point 실행"));
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
	}
}

void AEnemyCharacter::StopMovement() const
{
	GetController()->StopMovement();
}

bool AEnemyCharacter::GetIsDead() const
{
	return bIsDead;
}

void AEnemyCharacter::DelayTimerFunction(const float DelayTimer)
{
}

void AEnemyCharacter::GetHit(const FVector& ImpactPoint, AActor* Hitter, const float TakenDamage)
{
	const FDamageEvent DamageEvent;
	
	if(const auto Target = Cast<APawn>(Hitter))
	{
		PlayerDetected(Target);
		// ChangeTarget(Target);
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
	
	Health -= TakeDamage(TakenDamage, DamageEvent, Hitter->GetInstigatorController(), Hitter);
	
	if(Health <= 0.f && !bIsDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("사망"));
		EnemyState = EEnemyState::EES_Dead;
		PlayAnimation(DeathAnimMontage);
		bIsDead = true;
		StopMovement();
		GetWorldTimerManager().SetTimer(DestroyTimer, this, &AEnemyCharacter::TempFunc, 1.4f);
	}
	else if (Health > 0.f)
	{
		if(HitAnimMontage)
		{
			PlayAnimation(HitAnimMontage);
		}
	}
}

void AEnemyCharacter::ChangeState(EEnemyState State)
{
	EnemyState = State;
}