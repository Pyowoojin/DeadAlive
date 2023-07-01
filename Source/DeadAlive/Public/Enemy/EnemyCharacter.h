#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "EnemyCharacter.generated.h"

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
	EES_Dead UMETA(DisplayName = "Dead"),
};

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

	void PlayAnimation(UAnimMontage* AnimMontage);
	void StopMovement() const;
	bool GetIsDead() const;
	FORCEINLINE void TempFunc() { Destroy(); }
	

protected:
	virtual void BeginPlay() override;

private :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Health = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool IsDead = false;

	FTimerHandle DestroyTimer;
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Custom, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* HitParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Custom, meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* BloodParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Custom, meta = (AllowPrivateAccess = "true"))
	USoundBase* HitSound;

	// 애니메이션 관련

	UPROPERTY(EditAnywhere, Category = Custom, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathAnimMontage;

};
