// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup/Pickup.h"
#include "GameFramework/Actor.h"
#include "BaseSpawner.generated.h"

/**
 * 
 */
USTRUCT()
struct FSpawnContent
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = Content)
	TSubclassOf<AActor> Actor;

	UPROPERTY(EditAnywhere, Category = Content)
	int32 SpawnChance;

	/** Rotation of the Spaned Actor at spawn */
	UPROPERTY(EditAnywhere, Category = Content)
	FRotator Rotation;

	FSpawnContent()
	{
		SpawnChance = 100;
		Rotation = FRotator::ZeroRotator;
	}

};

UCLASS()
class SNAKE_PROJECT_API ABaseSpawner : public AActor
{
	GENERATED_BODY()

public:
	ABaseSpawner(const class FObjectInitializer& PCIP);

	UPROPERTY(VisibleDefaultsOnly, Category = Spawner)
	class UBoxComponent* RootBoxComp;

	UPROPERTY(EditAnywhere, Category = "Content")
	TArray<FSpawnContent> Content;

	/** The Total Depth with be double this value. Start = Depth Above | End = Depth Below */
	UPROPERTY(EditAnywhere, Category = "Content")
	float TraceDepth;

	/** [Server] Spawn a pickup */
	UFUNCTION(BlueprintCallable, Category = "Spawner|Spawn Functions")
	void Spawn();

	/** execute spawning a Pickup on the server*/
	//UFUNCTION(Reliable, Server, WithValidation)
	//void ServerSpawn(int32 ContentIndex, FVector Location, FRotator Rotation);
	//virtual void ServerSpawn_Implementation(int32 ContentIndex, FVector Location, FRotator Rotation);
	//virtual bool ServerSpawn_Validate(int32 ConentIndex, FVector Location, FRotator Rotation) { return true; }

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Spawner")
	bool bPerformLineTrace;

protected:
	UPROPERTY()
	AActor* SpawnedActor;

	FVector GetSpawnLocation();

	int32 GetSpawnIndex();
	void VarifySpawnChances();

};
