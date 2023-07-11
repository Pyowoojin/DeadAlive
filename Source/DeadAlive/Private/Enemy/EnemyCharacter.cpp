#include "Enemy/EnemyCharacter.h"
#include "AIController.h"
#include "NiagaraFunctionLibrary.h"
#include "Items/EnemyWeapon.h"
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
	
	PawnSensingComponent->SetPeripheralVisionAngle(37.5f);
	PawnSensingComponent->SightRadius = 2300.f;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	
}
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	EnemyInitialize();
	MoveToPoint(nullptr);
	SpawnDefaultWeapon();
}

void AEnemyCharacter::AttackTimerEnd()
{
	bAttackTimer = false;
}

void AEnemyCharacter::AttackTimerStart()
{
	bAttackTimer = true;
	GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AEnemyCharacter::AttackTimerEnd, AttackDelay);
}

void AEnemyCharacter::EnemyInitialize()
{
	AIController = Cast<AAIController>(GetController());
	
	if(PawnSensingComponent)
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemyCharacter::PlayerDetected);
		AIController->ReceiveMoveCompleted.AddDynamic(this, &AEnemyCharacter::EnemyMoveCompleted);
		// AIController->OnMoveCompleted.AddDynamic(this, &AEnemyCharacter::EnemyMoveCompleted);
	}
}

void AEnemyCharacter::CreateNewPatrolJob()
{
	if(EEnemyState::EES_Attacking == EnemyState) return;
	
	FNavLocation DestNavLocation;
	const FVector Dest = CalcNextMovementLocation(DestNavLocation);
	ChangeState(EEnemyState::EES_Patrol);
	AIController->MoveToLocation(Dest, AcceptanceRadiusMax);
	ChangeSpeed(WalkSpeed);
	DrawDebugSphere(GetWorld(), Dest, 64.f, 32, FColor::Blue, false, 5.f);
}

void AEnemyCharacter::EnemyMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if(Result == EPathFollowingResult::Success)
	{
		if(AIController)
		{
			if(EnemyState == EEnemyState::EES_Combat)
			{
				UE_LOG(LogTemp, Warning, TEXT("전투모드!"));
				if (CanAttack(TargetPoint))
				{
					UE_LOG(LogTemp, Warning, TEXT("We are under attack!"));
					PlayAnimation(AttackMontage);
					AttackTimerStart();
				}
				else
				{
					// MoveToPoint(TargetPoint);
				}
			}
			else
			{
				const float RandomTime = FMath::RandRange(1.f, 5.f);
				GetWorld()->GetTimerManager().SetTimer(PatrolTimer, this, &AEnemyCharacter::CreateNewPatrolJob, RandomTime);
			}
		}
	}
}

void AEnemyCharacter::PlayerDetected(APawn* TargetActor)
{
	if(EnemyState == EEnemyState::EES_Dead || EnemyState == EEnemyState::EES_Chasing) return;
	UE_LOG(LogTemp, Warning, TEXT("플레이어 디텍트"));

	TargetPoint = TargetActor;
	ChangeSpeed(RunSpeed);
	ChangeState(EEnemyState::EES_Combat);
	
	AIController->MoveToActor(TargetActor, AcceptanceRadiusMax);
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
	const float RealDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
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
		UE_LOG(LogTemp, Warning, TEXT("무브 투 탈겟"));
		// 목표가 없으면 그냥 정찰 위치로 이동
		if(GoalActor == nullptr)
		{
			CreateNewPatrolJob();
			// AIController->MoveTo(MoveRequest);
		}

		// 목표가 있으면 목표를 따라감
		else
		{
			FAIMoveRequest MoveRequest;
			MoveRequest.SetAcceptanceRadius(AcceptanceRadiusMax);
			MoveRequest.SetGoalActor(GoalActor);
			AIController->MoveToActor(GoalActor);
			//어디해야하냐면 이제 플레이어한테 붙으면 공격하는거 만들어야함
		}
		
	}
}

void AEnemyCharacter::PlayAnimation(UAnimMontage* AnimMontage)
{
	if(AnimMontage == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("애니메이션이 없습니다."));
		return;
	}
	if(UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		UE_LOG(LogTemp, Warning, TEXT("애니메이션 실행"));
		int32 JumpSection = FMath::RandRange(1, AnimMontage->CompositeSections.Num());
		FName SectionName = FName(*FString::Printf(TEXT("%d"), JumpSection));
		AnimInstance->Montage_Play(AnimMontage);
		AnimInstance->Montage_JumpToSection(SectionName);
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
void AEnemyCharacter::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if(World && WeaponClass)
	{
		AEnemyWeapon* DefaultWeaponLeft = World->SpawnActor<AEnemyWeapon>(WeaponClass);
		AEnemyWeapon* DefaultWeaponRight = World->SpawnActor<AEnemyWeapon>(WeaponClass);
		DefaultWeaponLeft->Equip(GetMesh(), FName("LeftHandWeaponSocket"), this, this);
		DefaultWeaponRight->Equip(GetMesh(), FName("RightHandWeaponSocket"), this, this);
		
	}
}

void AEnemyCharacter::GetHit(const FVector& ImpactPoint, AActor* Hitter, const float TakenDamage)
{
	const FDamageEvent DamageEvent;
	
	if(const auto Target = Cast<APawn>(Hitter))
	{
		PlayerDetected(Target);
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
		ChangeState(EEnemyState::EES_Dead);
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

bool AEnemyCharacter::CanAttack(AActor* TargetActor)
{
	if(EEnemyState::EES_Dead == EnemyState || EEnemyState::EES_Attacking == EnemyState || bAttackTimer) return false;

	if(TargetIsInRange(TargetActor, AttackRange))
	{
		return true;
	}
	return false;
}

bool AEnemyCharacter::TargetIsInRange(AActor* TargetActor, double Radius)
{
	if(TargetActor == nullptr) return false;
	
	const double DistanceToTarget = (TargetActor->GetActorLocation() - GetActorLocation()).Size();
	UE_LOG(LogTemp, Warning, TEXT("액터와의 거리 : %f"), DistanceToTarget);
	return DistanceToTarget <= Radius;
}