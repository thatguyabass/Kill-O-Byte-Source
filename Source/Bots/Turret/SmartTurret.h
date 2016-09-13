// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Turret/Turret.h"
#include "SmartTurret.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ASmartTurret : public ATurret
{
	GENERATED_BODY()
	
public:
	ASmartTurret();
	
	/** Locations where this turret can move to. If this is empty, the turret will remain in place */
	UPROPERTY(BlueprintReadWrite, Category = "Other")
	TArray<FVector> Waypoints;

	/** Is there a delay before the next waypoint is chosen? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Other")
	float WaitTime;

	/** If True, the turret will wait for a durtion before moving to the next waypoint */
	UPROPERTY()
	bool bWait;

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** Get the Next Waypoint */
	UFUNCTION(BlueprintCallable, Category = "Turret|Navigation")
	FVector GetNextWaypoint();

private:


	/** Can this turret move? This will be set if there are any waypoints */
	bool bCanMove;

	UPROPERTY()
	int32 WaypointIndex;

	UPROPERTY()
	FVector InitialLocation;

	/** By default, the waypoints are in local space. Add the players position to bring them to world space */
	UFUNCTION()
	void InitializeWaypoints();

};
