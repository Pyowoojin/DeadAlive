// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_Aiming UMETA(DisplayName = "Aiming"),
	EOS_Hip UMETA(DisplayName = "Hip"),
	EOS_Reloading UMETA(DisplayName = "Reloading"),
	EOS_InAir UMETA(DisplayName = "InAir"),
	EOS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class DEADALIVE_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public :

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float Direction;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly)
	class APlayerCharacter* PlayerCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* PlayerCharacterMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = true))
	float MovementOffsetYaw;

	// 이전 프레임의 방향을 저장
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = true))
	float LastMovementDir;

protected:
	
	void TurnInPlace();

	// 달리는 동안 기울임 감지
	void Lean(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = true))
	bool bAiming = false;

	// 이번 프레임 캐릭터의 Yaw값 저장하는 변수 -> Turn In Place / 가만히 서있을때만 업데이트
	float TIPCharacterYaw = 0.f;

	// 이전 프레임의 캐릭터 Yaw 값 -> Turn In Place / 가만히 서있을 때만 업데이트
	float TIPCharacterYawLastFrame = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float RootYawOffset = 0.f;

	// 이번 프레임의 회전값
	float RotationCurve = 0.f;

	// 이 전 프레임의 회전값
	float RotationCurveLastFrame = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float Pitch = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	bool bReloading = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	EOffsetState OffsetState = EOffsetState::EOS_Hip;

	FRotator CharacterRotation = FRotator(0.f);

	FRotator CharacterRotationLastFrame = FRotator(0.f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Lean, meta = (AllowPrivateAccess = "true"))
	float YawDelta;
};
