// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Spawner/BaseSpawner.h"
#include "TimeSpawner.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ATimeSpawner : public ABaseSpawner
{
	GENERATED_BODY()
	
public:
	ATimeSpawner(const class FObjectInitializer& PCIP);
	
	UPROPERTY(EditAnywhere, Category = Timer)
	float MinSpawnTime;

	UPROPERTY(EditAnywhere, Category = Timer)
	float MaxSpawnTime;

	//Is the Object Set to Spawn anything
	UPROPERTY(EditAnywhere, Category = Timer)
	bool bSpawnEnabled;

	//Reset the Spawner - Enable Spawning and Set Progress to 0
	UFUNCTION()
	void Reset();

	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);

private:
	UPROPERTY()
	float SpawnProgress;

	UPROPERTY()
	float SpawnTime;

	void SetSpawnTime();

};
