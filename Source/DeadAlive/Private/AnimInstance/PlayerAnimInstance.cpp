#include "AnimInstance/PlayerAnimInstance.h"
#include "PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	// 방향과 속도를 계산하기 위해 캐릭터의 무브먼트 컴포넌트를 받아온다.
	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
	if (PlayerCharacter) {
		PlayerCharacterMovement = PlayerCharacter->GetCharacterMovement();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	// 캐릭터 무브먼트 컴포넌트로 매 프레임 방향과 속도를 계산한다.
	if (PlayerCharacterMovement) {
		// if문 대신 한 거 인듯?
		bReloading = PlayerCharacter->GetCombatState() == ECombatState::ECS_Reloading;
		
		GroundSpeed = UKismetMathLibrary::VSizeXY(PlayerCharacterMovement->Velocity);
		Direction = CalculateDirection(PlayerCharacterMovement->Velocity, PlayerCharacter->GetActorForwardVector().Rotation());
		FString Dir = FString::SanitizeFloat(Direction);
		bIsInAir = PlayerCharacterMovement->IsFalling();

		// 플레이어가 움직이고 있을 때에만 업데이트함
		if(PlayerCharacter->GetVelocity().Size() > 0.f)
			LastMovementDir = Direction;

		bAiming = PlayerCharacter->GetAiming();
		if(GEngine)
		{
			if(bAiming)
				GEngine->AddOnScreenDebugMessage(7678, -1, FColor::Yellow, FString::Printf(TEXT("조준하는 중")));
			else
				GEngine->AddOnScreenDebugMessage(7678, -1, FColor::Yellow, FString::Printf(TEXT("조준 안 하는 중")));
		}

		if(bReloading)
		{
			OffsetState = EOffsetState::EOS_Reloading;
		}
		else if(bIsInAir)
		{
			OffsetState = EOffsetState::EOS_InAir;
		}
		else if(PlayerCharacter->GetAiming())
		{
			OffsetState = EOffsetState::EOS_Aiming;
		}
		else
		{
			OffsetState = EOffsetState::EOS_Hip;
		}
	}

	TurnInPlace();
	Lean(DeltaTime);
}

void UPlayerAnimInstance::TurnInPlace()
{
	if(PlayerCharacter == nullptr) return;

	Pitch = PlayerCharacter->GetBaseAimRotation().Pitch;

	

	if(GroundSpeed > 0 || bIsInAir)
	{
		RootYawOffset = 0.f;
		TIPCharacterYaw = PlayerCharacter->GetActorRotation().Yaw;
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		RotationCurveLastFrame = 0.f;
		RotationCurve = 0.f;
	}
	else
	{
		// 이전 프레임의 Yaw값을 저장한다
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		TIPCharacterYaw = PlayerCharacter->GetActorRotation().Yaw;
		const float TIPYawDelta { TIPCharacterYaw - TIPCharacterYawLastFrame };

		// RootYawOffset [-180 ~ 180]으로 값 고정
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TIPYawDelta);

		// 1.0는 회전, 0.0은 회전 아님
		const float Turning { GetCurveValue(TEXT("Turning"))};
		if(Turning > 0)
		{
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			const float DeltaRotation { RotationCurve - RotationCurveLastFrame };

			// RootYawOffset > 0 -> 왼쪽으로 도는 중... 그리고 RootYawOffset < 0 -> 오른쪽으로 도는 중 ...
			// 0보다 크면 델타값 빼주고, 작다면 더해주기
			RootYawOffset > 0 ? RootYawOffset -=DeltaRotation : RootYawOffset += DeltaRotation;

			const float ABSRootYawOffset { FMath::Abs(RootYawOffset) };
			if(ABSRootYawOffset > 90.f)
			{
				const float YawExcess { ABSRootYawOffset - 90.f };
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
	}
}

void UPlayerAnimInstance::Lean(float DeltaTime)
{
	if(PlayerCharacter == nullptr) return;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = PlayerCharacter->GetActorRotation();

	const FRotator Delta {UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame)};
	
	const float Target = (Delta.Yaw / DeltaTime);
	const float Interp{FMath::FInterpTo(YawDelta, Target, DeltaTime, 6.f)};

	YawDelta = FMath::Clamp(Interp, -90.f, 90.f);
}