// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "CollectionSpawner.h"

ACollectionSpawner::ACollectionSpawner(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

void ACollectionSpawner::BeginPlay()
{
	Super::BeginPlay();
	Super::Spawn();
}

void ACollectionSpawner::Tick(float DeltaTime)
{
	if (Role == ROLE_Authority)
	{
		APickup* Pickup = SpawnedActor ? Cast<APickup>(SpawnedActor) : nullptr;
		if (Pickup && Pickup->bCollected)
		{
			Super::Spawn();
		}
	}
}