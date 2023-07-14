#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Navigation/PathFollowingComponent.h"
#include "EnemyCharacter.generated.h"

class UEnemyAttribute;
class UBaseCharacterAttribute;
class AEnemyWeapon;
class UPawnSensingComponent;
class AAIController;
class UParticleSystem;
class USoundBase;
class UNiagaraSystem;
class UAnimMontage;

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Idle UMETA(DisplayName = "Idle"),
	EES_Combat UMETA(DisplayName = "Combat"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Patrol UMETA(DisplayName = "Patrol"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Dead UMETA(DisplayName = "Dead"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMultiDele);

UCLASS()
class DEADALIVE_API AEnemyCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemyCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit(const FVector& ImpactPoint, AActor* Hitter, const float TakenDamage) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	FRotator ReturnRandomRotation() const;
	void MoveToPoint(AActor* GoalActor);
	void EnemyInitialize();
	void CreateNewPatrolJob();
	void CreateController();

	UFUNCTION(BlueprintCallable)
	void AttackStart();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION()
	void EnemyMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	void PlayAnimation(UAnimMontage* AnimMontage);
	void StopMovement() const;
	bool GetIsDead() const;
	FORCEINLINE void TempFunc() { Destroy(); }

	void DelayTimerFunction(const float DelayTimer);

	void SpawnDefaultWeapon();
	
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Custom")
	FDynamicMultiDele Fuc_DeleMulti;
	FTimerHandle PatrolTimer;
	

protected:
	virtual void BeginPlay() override;

private :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Health = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsDead = false;

	FTimerHandle DestroyTimer;
	FTimerHandle AttackTimer;
	bool bAttackTimer = false;
	void AttackTimerEnd();
	void AttackTimerStart();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Custom, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* HitParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Custom, meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* BloodParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Custom, meta = (AllowPrivateAccess = "true"))
	USoundBase* HitSound;

	// 애니메이션 관련

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 HitCount = 0;
	
	UPROPERTY(EditAnywhere, Category = Custom, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathAnimMontage;

	UPROPERTY(EditAnywhere, Category = Custom, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitAnimMontage;

	UPROPERTY(EditAnywhere, Category = Custom, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = Custom, meta = (AllowPrivateAccess = "true"))
	AAIController* AIController;
	
	UPROPERTY(EditAnywhere, Category = Custom, meta = (AllowPrivateAccess = "true"))
	AActor* TargetPoint;

	// AI 관련
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensingComponent;

	UFUNCTION()
	void PlayerDetected(APawn* TargetActor);

	void ChangeState(EEnemyState State);

	UFUNCTION()
	bool CanAttack(AActor* TargetActor);

	bool TargetIsInRange(AActor* TargetActor, double Radius);

	UFUNCTION()
	void ChangeTarget(APawn* TargetActor);

	void ChangeSpeed(const float Speed) const;
	void DecreaseSpeed(float DelayTime) const;
	
	FVector CalcNextMovementLocation(FNavLocation& DestLocation);

	UPROPERTY(VisibleAnywhere, Category = Custom, meta = (AllowPrivateAccess = "true"))
	float AcceptanceRadiusMax = 15.f;

	UPROPERTY(VisibleAnywhere, Category = Custom, meta = (AllowPrivateAccess = "true"))
	float PatrolRange = 3000.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Custom, meta = (AllowPrivateAccess = "true"))
	EEnemyState EnemyState = EEnemyState::EES_Idle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category = Custom, meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category = Custom, meta = (AllowPrivateAccess = "true"))
	float RunSpeed = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category = Custom, meta = (AllowPrivateAccess = "true"))
	float RemainedDelayTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category = Custom, meta = (AllowPrivateAccess = "true"))
	float MaxDelayTime = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category = Custom, meta = (AllowPrivateAccess = "true"))
	double AttackRange = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category = Custom, meta = (AllowPrivateAccess = "true"))
	float AttackDelay = 1.2f;


	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category = Custom, meta = (AllowPrivateAccess = "true"))
	UEnemyAttribute* CharAttribute;

	
};
