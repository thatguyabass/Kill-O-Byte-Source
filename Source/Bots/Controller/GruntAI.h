// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Controller/BaseAIController.h"
#include "GruntAI.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AGruntAI : public ABaseAIController
{
	GENERATED_BODY()
	
public:
	AGruntAI(const class FObjectInitializer& PCIP);

	/** Min Distance to Target */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behavior")
	float MinDistance;

	/** Max Distance away from target */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behavior")
	float MaxDistance;

	virtual void Possess(APawn* InPawn) override;

	/** Find a location that is near the player and maintain a set distance from the player. */
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	virtual void MaintainDistanceCustom(const float MinimumDistance = 400.0f, const float MaximumDistance = 600.0f);

	/** Find a location that is near the player and maintain a set distance from the player. Use the Interal Values for Min and Max Distance */
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	void MaintainDistance();

	UFUNCTION(BlueprintCallable, Category = "Behavior")
	void FindPointOfInterest();

	/** Override the Base Find Target */
	virtual void FindTarget() override;

	virtual void InitializeBlackboardKeys() override;

	/** Set a point of interest */
	void SetPointOfInterest(class APointOfInterest* Interest);

	APointOfInterest* GetPointOfInterest();

	virtual FCollisionObjectQueryParams GetObjectQuery() override;

	/** If true, the Target is within the MaxDistance */
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	bool InRangeOfTarget();

	/** Set In Range Key */
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	void SetInRange(bool InRange);

	/** Set Stored Direction Key */
	void SetStoredDirection(FVector StoredDirection);

protected:
	int32 InterestKey;
	int32 InRangeKey;
	int32 StoredDirectionKey;

public:
	/** Prepare the Actors to start orbiting.
	*	Get the Initial Direction
	*	Set the Exepcted Delta Rotation
	*	Set In Orbit Range
	*/
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	void InitializeOrbit();

	/** Orbit around the Target. Direction is random */
	UFUNCTION(BlueprintCallable, Category = "Behavior|Drone")
	void OrbitTarget();

	/** Distance From the Target During Orbiting */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behavior|Drone")
	float OrbitRadius;

	/** How long the Full orbit takes. */
	UPROPERTY(EditAnywhere, Category = "Behavior|Drone")
	float OrbitDuration;

	/** How deep the Line Trace runs to detect the terrain */
	UPROPERTY(EditAnywhere, Category = "Behavior|Drone")
	float OrbitLineTraceDepth;

protected:
	/** Total Rotation around any target. */
	UPROPERTY()
	FRotator OrbitAmount;

	/** Direction the Bot approached the target at */
	UPROPERTY()
	FVector InitialDirection;

	/** Movement speed at initialization */
	UPROPERTY()
	float BaseMovementSpeed;

	/** Current Progress around the Target */
	float OrbitProgress;
	/** Distance of the Orbit */
	float OrbitDistance;

	/** Direction of the orbit. This is randomly assigned during orbit initialization */
	float OrbitDirection;

};
