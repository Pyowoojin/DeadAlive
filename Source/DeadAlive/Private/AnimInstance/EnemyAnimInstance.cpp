// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/EnemyAnimInstance.h"
#include "Enemy/EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	EnemyCharacter = Cast<AEnemyCharacter>(TryGetPawnOwner());
	if(EnemyCharacter)
	{
		CharMovement = EnemyCharacter->GetCharacterMovement();
	}
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if(CharMovement)
	{
		Speed = CharMovement->Velocity.Size();
	}
}
