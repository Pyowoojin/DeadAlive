#include "Items/Obstacles.h"
#include "Components/StaticMeshComponent.h"

AObstacles::AObstacles()
{
	PrimaryActorTick.bCanEverTick = true;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Object"));
	SetRootComponent(StaticMesh);
	// StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}
void AObstacles::BeginPlay()
{
	Super::BeginPlay();
	
}

void AObstacles::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

