// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/BotSpawnerWave.h"
#include "EndlessBotSpawner.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AEndlessBotSpawner : public ABotSpawnerWave
{
	GENERATED_BODY()

public:
	AEndlessBotSpawner();

	/** Stop spawning bots */
	UFUNCTION(BlueprintCallable, Category = "BotSpawner|Control")
	void FinishSpawning();

protected:
	virtual bool CanSpawn() override;

	virtual void MarkFinished() override;
};
