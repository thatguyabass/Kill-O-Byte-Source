// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/SingleBotSpawner.h"
#include "BotSpawnerWave.generated.h"

UCLASS()
class SNAKE_PROJECT_API ABotSpawnerWave : public ASingleBotSpawner
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABotSpawnerWave();

	/** Maximum number of bots that can spawn from this spawner */
	UPROPERTY(EditAnywhere, Category = "BotSpawner|Properties")
	int32 MaxBotCount;

	/** Max number of bots that can be alive at a Time */
	UPROPERTY(EditAnywhere, Category = "BotSpawner|Properties")
	int32 MaxAliveBotCount;

	/** Delay better bots spawned */
	UPROPERTY(EditAnywhere, Category = "BotSpawner|Properties")
	float SpawnDelay;

	virtual void StartSpawning() override;

	/** Stop spawning additional bots */
	virtual void SoftReset() override;

	/** Reset spawner to pre start state. Kills allready spawned bots */
	virtual void Reset() override;

protected:
	/** Total number of bots that have spawned */
	int32 TotalBotCount;

	/** Current number of alive bots */
	int32 AliveBotCount;

	/** Alive bots that this spawner has spawned. Use to track if any have died */
	UPROPERTY()
	TArray<class ABot*> AliveBots;

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** if true, Conditions are right for a New Bot! */
	virtual bool CanSpawn() override;

	virtual void LatestBot(class ABot* Bot) override;

	/** Set the Finished flag when all the bots have been spawned */
	virtual void MarkFinished() override;

	FTimerHandle Spawn_TimerHandle;

};
