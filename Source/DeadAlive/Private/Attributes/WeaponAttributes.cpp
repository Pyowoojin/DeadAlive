// Fill out your copyright notice in the Description page of Project Settings.


#include "Attributes/WeaponAttributes.h"

// Sets default values for this component's properties
UWeaponAttributes::UWeaponAttributes() : ItemName("Default")
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UWeaponAttributes::DecreaseAmmoCount()
{
	// --RemainedBulletCount;
	RemainedBulletCount = FMath::Clamp(--RemainedBulletCount, 0, MaxBulletCount);
	
}

// Called when the game starts
void UWeaponAttributes::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UWeaponAttributes::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

