// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/BotSpawnerWave.h"
#include "BotSpawnerPresetWave.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ABotSpawnerPresetWave : public ABotSpawnerWave
{
	GENERATED_BODY()
	
public:
	ABotSpawnerPresetWave();

	virtual void SoftReset() override;
	virtual void Reset() override;

protected:
	virtual void LatestBot(class ABot* Bot) override;

	/** Current Bot that is being spawned */
	int32 SpawnIndex;

	virtual void MarkFinished() override;

	virtual int32 GetSpawnIndex() override;

	virtual bool CanSpawn() override;

};
