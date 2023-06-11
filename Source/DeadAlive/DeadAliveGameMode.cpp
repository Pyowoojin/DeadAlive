// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeadAliveGameMode.h"
#include "DeadAliveCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADeadAliveGameMode::ADeadAliveGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
