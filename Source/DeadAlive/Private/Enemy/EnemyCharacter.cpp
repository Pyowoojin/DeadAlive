#include "Enemy/EnemyCharacter.h"
#include "AIController.h"
#include "NiagaraFunctionLibrary.h"
#include "Items/EnemyWeapon.h"
#include "Engine/DamageEvents.h"
#include "Perception/PawnSensingComponent.h"
#include "NavigationSystem.h"
#include "Attributes/EnemyAttribute.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComponent");
	CharAttribute = CreateDefaultSubobject<UEnemyAttribute>("CharAttribute");
	PawnSensingComponent->SetPeripheralVisionAngle(42.5f);
	PawnSensingComponent->SightRadius = 2300.f;
	PawnSensingComponent->bOnlySensePlayers = false;

	this->bUseControllerRotationYaw = true;
	this->BaseEyeHeight = 32.f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	
}
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	PawnSensingComponent->SetSensingInterval(0.7f);
	AIController = Cast<AAIController>(GetController());
	if(AIController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("컨트롤러 미생성"));
	}

	CreateController();
	SpawnDefaultWeapon();
	EnemyInitialize();
	MoveToPoint(nullptr);

	if(GetController())
	{
		UE_LOG(LogTemp, Warning, TEXT("HELLOO"));
	}
}

void AEnemyCharacter::AttackTimerEnd()
{
	bAttackTimer = false;
	ChangeState(EEnemyState::EES_Combat);
	// FAIRequestID 
	EnemyMoveCompleted(0, EPathFollowingResult::Success);
	// CreateNewPatrolJob();
}

void AEnemyCharacter::AttackTimerStart()
{
	bAttackTimer = true;
	GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AEnemyCharacter::AttackTimerEnd, AttackDelay);
}

void AEnemyCharacter::EnemyInitialize()
{
	if(AIController)
	{
		AIController->ReceiveMoveCompleted.AddDynamic(this, &AEnemyCharacter::EnemyMoveCompleted);
	}
	if(PawnSensingComponent)
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemyCharacter::PlayerDetected);
	}
}

void AEnemyCharacter::CreateNewPatrolJob()
{
	if(EEnemyState::EES_Attacking == EnemyState) return;
	
	FNavLocation DestNavLocation;
	const FVector Dest = CalcNextMovementLocation(DestNavLocation);
	ChangeState(EEnemyState::EES_Patrol);
	if(AIController)
		AIController->MoveToLocation(Dest, AcceptanceRadiusMax);
	ChangeSpeed(WalkSpeed);
	DrawDebugSphere(GetWorld(), Dest, 64.f, 32, FColor::Blue, false, 5.f);
}

void AEnemyCharacter::CreateController()
{
	if(GetController() == nullptr)
	{
		this->SpawnDefaultController();
	}

	if(AIController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("생성완료"));
		AIController = Cast<AAIController>(GetController());
	}
}

void AEnemyCharacter::AttackStart()
{
	if(CharAttribute->GetEquippedWeapon() != nullptr)
	{
		CharAttribute->GetEquippedWeapon()->SetWeaponCollision();
	}
}

void AEnemyCharacter::AttackEnd()
{
	if(CharAttribute->GetEquippedWeapon() != nullptr)
	{
		CharAttribute->GetEquippedWeapon()->SetWeaponNoCollision();
	}
}

void AEnemyCharacter::EnemyMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if(Result == EPathFollowingResult::Success)
	{
		if(AIController)
		{
			if(EnemyState != EEnemyState::EES_Attacking)
			{
				// UE_LOG(LogTemp, Warning, TEXT("전투모드!"));

				// 타겟 폰이 없는게 아니라면 공격할 수 있는지 확인한다. 
				if(TargetPawn == nullptr)
				{
					// 다시 패트롤 실행
					ChangeSpeed(WalkSpeed);
					const float RandomTime = FMath::RandRange(1.f, 5.f);
					GetWorld()->GetTimerManager().SetTimer(PatrolTimer, this, &AEnemyCharacter::CreateNewPatrolJob, RandomTime);
				}
				else if(!CanAttack(TargetPawn))
				{
					// 적 추적 시행
					ChangeSpeed(RunSpeed);

					// 모든 것을 Pawn으로 바꿔야함 Actor는 옳지 않아요
					

					// Problem 문제 -> 여러번 실행되는 이유가 뭐지? AIMoveTo가 계속 완수가 됨. 이걸 해결하기 위해서는?
					// ChangeTarget(NewPawn);
					GetWorld()->GetTimerManager().SetTimer(ChaseTimer, this, &AEnemyCharacter::ChaseTarget, 1.f);
					// ChaseTarget(TargetPawn);
				}
				else if(CanAttack(TargetPawn))
				{
					// 공격 실행
					FVector DirectionToPlayer = TargetPawn->GetActorLocation() - this->GetActorLocation();
					
					DrawDebugSphere(GetWorld(), DirectionToPlayer, 30.f, 12, FColor::Blue, false, 3.f);
					DirectionToPlayer.Normalize();

					DrawDebugLine(GetWorld(), this->GetActorLocation(), this->GetActorLocation() + DirectionToPlayer * 50.f, FColor::Red, false, 3.f);
					DrawDebugSphere(GetWorld(), this->GetActorLocation() + DirectionToPlayer * 50, 30.f, 12, FColor::Red, false, 3.f);
					// UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), DirectionToPlayer.X, DirectionToPlayer.Y, DirectionToPlayer.Z);

					const FRotator NewRotation = DirectionToPlayer.Rotation();

					UE_LOG(LogTemp, Warning, TEXT(" 원래 방향 : %s"), *this->GetActorRotation().ToString());
					this->FaceRotation(NewRotation, 0);
					// this->SetActorRotation(NewRotation);
					UE_LOG(LogTemp, Warning, TEXT(" 조정 후  방향 : %s"), *NewRotation.ToString());
					
					
					PlayAnimation(AttackMontage);
					AttackTimerStart();
				}
				
				/*if (CanAttack(TargetPawn))
				{
					ChangeState(EEnemyState::EES_Attacking);
					// UE_LOG(LogTemp, Warning, TEXT("We are under attack!"));


					FVector DirectionToPlayer = TargetPawn->GetActorLocation() - this->GetActorLocation();
					
					DrawDebugSphere(GetWorld(), DirectionToPlayer, 30.f, 12, FColor::Blue, false, 3.f);
					DirectionToPlayer.Normalize();

					DrawDebugLine(GetWorld(), this->GetActorLocation(), this->GetActorLocation() + DirectionToPlayer * 50.f, FColor::Red, false, 3.f);
					DrawDebugSphere(GetWorld(), this->GetActorLocation() + DirectionToPlayer * 50, 30.f, 12, FColor::Red, false, 3.f);
					// UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), DirectionToPlayer.X, DirectionToPlayer.Y, DirectionToPlayer.Z);

					const FRotator NewRotation = DirectionToPlayer.Rotation();

					UE_LOG(LogTemp, Warning, TEXT(" 원래 방향 : %s"), *this->GetActorRotation().ToString());
					this->FaceRotation(NewRotation, 0);
					// this->SetActorRotation(NewRotation);
					UE_LOG(LogTemp, Warning, TEXT(" 조정 후  방향 : %s"), *NewRotation.ToString());
					
					
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
			}*/
			}
		}
	}
}

void AEnemyCharacter::PlayerDetected(APawn* TargetActor)
{
	// if(EnemyState == EEnemyState::EES_Dead || EnemyState == EEnemyState::EES_Chasing) return;
	if(EnemyState == EEnemyState::EES_Dead) return;
	UE_LOG(LogTemp, Warning, TEXT("플레이어 디텍트 이름 : %s"), *TargetActor->GetName());

	// auto TP = Cast<AActor>(TargetActor);
		
	TargetPawn = ChooseTargetActor(TargetActor);
	ChangeSpeed(RunSpeed);
	ChangeState(EEnemyState::EES_Chasing);

	if(AIController)
	{
		// AIController->StopMovement();
		AIController->MoveToActor(TargetPawn, AcceptanceRadiusMax);
	}
	else
	{
		AIController = Cast<AAIController>(GetController());
	}
}

void AEnemyCharacter::ChaseTarget()
{
	if(AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("체이스 타겟"));
		AIController->MoveToActor(TargetPawn);
	}
}

void AEnemyCharacter::ChangeTarget(APawn* TargetActor)
{
	if(AIController && EnemyState != EEnemyState::EES_Dead)
	{
		TargetPawn = TargetActor;

		// problem /  문제
		AIController->MoveToActor(TargetActor);
		ChangeSpeed(RunSpeed);
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
		UE_LOG(LogTemp, Warning, TEXT("애니메이션없음"));
		return;
	}
	if(UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		// UE_LOG(LogTemp, Warning, TEXT("애니메이션 실행"));
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
		AEnemyWeapon* DefaultWeapon = World->SpawnActor<AEnemyWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("ZombieWeaponSocket"), this, this);
		CharAttribute->SetEquippedWeapon(DefaultWeapon);
	}
}

void AEnemyCharacter::GetHit(const FVector& ImpactPoint, AActor* Hitter, const float TakenDamage)
{
	const FDamageEvent DamageEvent;
	
	if(const auto Target = Cast<APawn>(Hitter))
	{
		//PlayerDetected(Target);
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

	CharAttribute->SetHealth(-TakeDamage(TakenDamage, DamageEvent, Hitter->GetInstigatorController(), Hitter));
	
	if(CharAttribute->GetCurrentHealth() <= 0.f && !bIsDead)
	{
		ChangeState(EEnemyState::EES_Dead);
		PlayAnimation(DeathAnimMontage);
		bIsDead = true;
		StopMovement();

		CharAttribute->GetEquippedWeapon()->Destroy();
		GetWorldTimerManager().SetTimer(DestroyTimer, this, &AEnemyCharacter::TempFunc, 1.4f);
	}
	else if (CharAttribute->GetCurrentHealth() > 0.f)
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

APawn* AEnemyCharacter::ChooseTargetActor(APawn* NewTarget)
{
	const FVector ThisActorLocation = this->GetActorLocation();

	// 현재 타겟보다 새로운 타겟의 위치가 가깝다면 NewTargetReturn
	if(TargetPawn == nullptr || (ThisActorLocation - NewTarget->GetActorLocation()).Length() < (ThisActorLocation - TargetPawn->GetActorLocation()).Length() )
	{
		return NewTarget;
	}
	return TargetPawn;
}

bool AEnemyCharacter::CanAttack(APawn* TargetActor)
{
	if(TargetPawn == nullptr || EEnemyState::EES_Dead == EnemyState || EEnemyState::EES_Attacking == EnemyState || bAttackTimer) return false;

	if(TargetIsInRange(TargetActor, AttackRange))
	{
		return true;
	}
	return false;
}

bool AEnemyCharacter::TargetIsInRange(APawn* TargetActor, double Radius)
{
	if(TargetActor == nullptr) return false;
	
	const double DistanceToTarget = (TargetActor->GetActorLocation() - GetActorLocation()).Size();
	UE_LOG(LogTemp, Warning, TEXT("액터와의 거리 : %f, AttackRadiance : %f"), DistanceToTarget, AttackRange);
	return DistanceToTarget <= Radius;
}