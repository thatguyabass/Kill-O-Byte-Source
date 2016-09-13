// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SingleBotSpawner.generated.h"

UENUM(BlueprintType)
enum class ESpawnParameters : uint8
{
	Farthest,
	Inbetween,
	Closest,
	Random
};

UCLASS()
class SNAKE_PROJECT_API ASingleBotSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASingleBotSpawner();

	UPROPERTY(EditAnywhere, Category = "BotSpawner|Properties")
	TArray<TSubclassOf<class ABot>> BotClasses;

	UPROPERTY(EditAnywhere, Category = "BotSpawner|Properties")
	class APOIGroup* POIGroup;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	/** Does spawning start when this actor is spawned? */
	UPROPERTY(EditAnywhere, Category = "BotSpawner|Properties")
	bool bSpawnAtStart;

	/** If true, the spawner will destroy it self after spawning a bot */
	UPROPERTY(EditAnywhere, Category = "BotSpawner|Properties")
	bool bDestroyAfterSpawn;

	UFUNCTION(BlueprintCallable, Category = "BotSpawner|Event")
	virtual void StartSpawning();

	/** Spawner has finished spawning bots. if true, no more bots will be spawned */
	UFUNCTION(BlueprintCallable, Category = "BotSpawner|Event")
	virtual bool IsFinishedSpawning();

	/** If true, Spawner has finished spawning bots and all the spawned bots have been killed */
	UFUNCTION(BlueprintCallable, Category = "BotSpawner|Event")
	virtual bool IsFinishedWithNoAliveBots();

	/** Reset the Spawner without destroying any alive bots */
	UFUNCTION(BlueprintCallable, Category = "BotSpawner|Event")
	virtual void SoftReset();

	/** Reset the Spawner to the Pre Spawn State */
	UFUNCTION(BlueprintCallable, Category = "BotSpawner|Event")
	virtual void Reset();

	UFUNCTION(BlueprintCallable, Category = "BotSpawner|Utility")
	TArray<class ABot*>& GetSpawnedBots();

	UFUNCTION(BlueprintCallable, Category = "BotSpawner|Control")
	void SetAdditionalSpawnPoints(TArray<FVector> Locations);

	UPROPERTY(EditAnywhere, Category = "BotSpawner|Properties")
	ESpawnParameters SpawnParams;

	/** Get the number of currently alive bots */
	UFUNCTION(BlueprintPure, Category = "BotSpawner|Utility")
	int32 GetAliveBotCount();

	UFUNCTION(BlueprintPure, Category = "BotSpawner|Utility")
	int32 GetSpecialBotCount();

	UFUNCTION(BlueprintPure, Category = "BotSpawner|Utility")
	int32 GetTargetTypeBotCount(uint8 BotTypeTarget);

protected:
	UPROPERTY()
	TArray<class ABot*> SpawnedBots;

	UPROPERTY()
	TArray<FVector> AdditionalSpawnLocations;

	UPROPERTY()
	TArray<class ASnakePlayer*> PlayerControllers;

	/** Flag to track if the spawner has finished spawning */
	bool bFinished;

	/** Set Finished to the True state */
	virtual void MarkFinished();

	/** Spawn a Bot */
	virtual void SpawnBot();

	/** Return the index of the next bot. Base method picks a random index of BotClasses Array */
	virtual int32 GetSpawnIndex();

	FVector GetSpawnLocation();

	/** if true, Conditions are right for a New Bot! */
	virtual bool CanSpawn();
	
	/** The latest bot that has spawned. Override this to access new bot */
	virtual void LatestBot(class ABot* Bot);

	UPROPERTY()
	class ABot* LastSpawnedBot;

public:
	UFUNCTION(BlueprintPure, Category = "Bot Utility")
	class ABot* GetLatestBot();

};
