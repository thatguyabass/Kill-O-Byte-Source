// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Controller/GruntAI.h"
#include "WallerAI.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AWallerAI : public AGruntAI
{
	GENERATED_BODY()
	
public:
	AWallerAI(const class FObjectInitializer& PCIP);

	/** Target the AI is moving towards */
	UPROPERTY(EditAnywhere, Category = "Navigation")
	TSubclassOf<class ATargetActor> TargetClass;

	/** Distance the Target is moved */
	UPROPERTY(EditAnywhere, Category = "Navigation")
	float Distance;

	/** Wall object that follows the bot */
	UPROPERTY(EditAnywhere, Category = "Wall Properties")
	TSubclassOf<class ABoundary> PillarClass;

	/** Max number of walls that are spawned */
	UPROPERTY(EditAnywhere, Category = "Wall Properties")
	int32 MaxPillars;

	/** Min number of walls before the bot dies */
	UPROPERTY(EditAnywhere, Category = "Wall Properties")
	int32 MinPillars;

	/** Delay applied to the Walls which delay their movement */
	UPROPERTY(EditAnywhere, Category = "Wall Properties")
	float StartDelay;

	/** Offset from the specified spawn location */
	UPROPERTY(EditAnywhere, Category = "Wall Properties")
	FVector SpawnOffset;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** Move the Target. Method auto checks if the target is moved into a blocking object */
	UFUNCTION(BlueprintCallable, Category = "Control")
	void MoveTarget();

	/** Destroy any pillars that may exist when this bot is being destroyed */
	virtual void BotDead(class ASnakePlayerState* Killer) override;

private:
	/** Move to components that are attached to the Walls */
	TArray<class UMoveToLocation*> MoveToComponents;

	/** Actor the Bot will move towards */
	UPROPERTY()
	class ATargetActor* Target;

	/** Flag to varify the Wall has been Initialized */
	bool bHasBeenInitialized;

	/** has this Wall been killed? */
	bool bDead;

	/** Spawn and Initialize the Pillars */
	void InitializePillars();

	/** Move the wall to the Current Pawn Position */
	void MoveWall(class UMoveToLocation* MoveTo);

	/** Start movement */
	void StartWallMovement(UMoveToLocation* Caller);

	/** Check the health of all the walls in the Walls Array and remove any that are being destroyed */
	void HealthCheck();

	/** Set a timer if to recall Initialize Pillars if the Pawn is null. */
	FTimerHandle Initialize_TimerHandle;
};
