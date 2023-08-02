#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Obstacles.generated.h"

class UStaticMeshComponent;

UCLASS()
class DEADALIVE_API AObstacles : public APawn
{
	GENERATED_BODY()
	
public:	
	AObstacles();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private :
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

};
