// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHitInterface : public UInterface
{
	GENERATED_BODY()
};

class DEADALIVE_API IHitInterface
{
	GENERATED_BODY()

	// 데미지를 받는 Interface함수. Enemy와 Player 둘 다 사용할 것임 추후 가스통 같은 아이템 액터도 고려
	// virtual void GetHit(const FVector& ImpactPoint, AActor* Hitter) = 0;
	virtual void GetHit(const FVector& ImpactPoint, AActor* Hitter, const float TakenDamage);
public:
};
