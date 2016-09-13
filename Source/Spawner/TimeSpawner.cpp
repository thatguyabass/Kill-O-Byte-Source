// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "TimeSpawner.h"

ATimeSpawner::ATimeSpawner(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	MinSpawnTime = 10.0f;
	MaxSpawnTime = 20.0f;

	SpawnProgress = 0.0f;

	bSpawnEnabled = true;
}

void ATimeSpawner::BeginPlay()
{
	Super::BeginPlay();
	SetSpawnTime();
}

void ATimeSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bSpawnEnabled)
	{
		SpawnProgress += DeltaTime;
		if (SpawnProgress >= SpawnTime)
		{
			Super::Spawn();
			//SpawnProgress is reset in SetSpawnTime()
			SetSpawnTime();
		}
	}
}

void ATimeSpawner::SetSpawnTime()
{
	SpawnProgress -= SpawnTime;
	SpawnTime = FMath::FRandRange(MinSpawnTime, MaxSpawnTime);
}

void ATimeSpawner::Reset()
{
	bSpawnEnabled = true;
	SpawnProgress = 0;
}