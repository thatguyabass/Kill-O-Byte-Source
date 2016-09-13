// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BotSpawnerWave.h"
#include "Bots/Bot/Bot.h"

// Sets default values
ABotSpawnerWave::ABotSpawnerWave()
	: ASingleBotSpawner()
{
	TotalBotCount = 0;
	MaxAliveBotCount = 5;
	MaxBotCount = 10;

	SpawnDelay = 0.5f;
	bSpawnAtStart = true;
	bDestroyAfterSpawn = false;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called every frame
void ABotSpawnerWave::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	for (int32 c = 0; c < AliveBots.Num(); c++)
	{
		if (AliveBots[c]->IsDead())
		{
			AliveBotCount--;
			AliveBots.RemoveAt(c);
			c--;
		}
	}
}

void ABotSpawnerWave::LatestBot(ABot* Bot)
{
	Super::LatestBot(Bot);

	if (Bot)
	{
		AliveBots.Add(Bot);

		AliveBotCount++;
		TotalBotCount++;
	}
}

bool ABotSpawnerWave::CanSpawn()
{
	if (TotalBotCount >= MaxBotCount || AliveBotCount >= MaxAliveBotCount)
	{
		return false;
	}
	
	return true;
}

void ABotSpawnerWave::StartSpawning()
{
	GetWorldTimerManager().SetTimer(Spawn_TimerHandle, this, &ABotSpawnerWave::SpawnBot, SpawnDelay, true);
}

void ABotSpawnerWave::MarkFinished()
{
	if (TotalBotCount >= MaxBotCount)
	{
		GetWorldTimerManager().ClearTimer(Spawn_TimerHandle);
		bFinished = true;
	}
}

void ABotSpawnerWave::SoftReset()
{
	Super::SoftReset();

	GetWorldTimerManager().ClearTimer(Spawn_TimerHandle);
}

void ABotSpawnerWave::Reset()
{
	Super::Reset();

	AliveBots.Empty();

	GetWorldTimerManager().ClearTimer(Spawn_TimerHandle);

	AliveBotCount = 0;
	TotalBotCount = 0;
}