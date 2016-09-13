// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BotSpawnerPresetWave.h"

ABotSpawnerPresetWave::ABotSpawnerPresetWave()
	: Super()
{
	SpawnIndex = 0;
	MaxAliveBotCount = 0;
}

void ABotSpawnerPresetWave::LatestBot(ABot* Bot)
{
	if (Bot)
	{
		SpawnIndex++;
		Super::LatestBot(Bot);
	}
}

void ABotSpawnerPresetWave::MarkFinished()
{
	if (SpawnIndex >= BotClasses.Num())
	{
		GetWorldTimerManager().ClearTimer(Spawn_TimerHandle);
		ASingleBotSpawner::MarkFinished();
	}
}

int32 ABotSpawnerPresetWave::GetSpawnIndex()
{
	return SpawnIndex;
}

bool ABotSpawnerPresetWave::CanSpawn()
{
	const bool b = (MaxAliveBotCount == 0) ? false : AliveBotCount >= MaxAliveBotCount;
	if (IsFinishedSpawning() || b)
	{
		return false;
	}

	return true;
}

void ABotSpawnerPresetWave::SoftReset()
{
	ASingleBotSpawner::SoftReset();

	SpawnIndex = 0;
}

void ABotSpawnerPresetWave::Reset()
{
	ASingleBotSpawner::Reset();

	SpawnIndex = 0;
}