// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "MoveToLocation.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SNAKE_PROJECT_API UMoveToLocation : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMoveToLocation();

	/** How long it takes for the Actor to reach the curret waypoint */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Navigation")
	float TravelTime;

	/** Is there a delay before the next waypoint is chosen? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Navigation")
	float WaitTime;

	/** If True, the Actor will wait for a durtion before moving to the next waypoint */
	UPROPERTY(EditAnywhere, Category = "Navigation")
	bool bWait;

	/** Does the Movement Loop? If true, bCompleted is NEVER true */
	UPROPERTY(EditAnywhere, Category = "Control")
	bool bLoop;

	/** Start moving through the loop when initialized */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control")
	bool bPlayAtStart;

	/** Delay before Starting Movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control")
	float StartDelay;

	/** Add World Location to the Waypoint Location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control")
	bool bUseWorldLocation;

	/** Should the Movement be swept? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control")
	bool bSweep;

	/** Start a new Cycle through the waypoints. */
	UFUNCTION(BlueprintCallable, Category = "Start Movement")
	void StartMovementCycle();

	/** Move towards the next waypoint */
	void MoveTo(float DeltaTime);

	/** Set Location to  */
	void MoveToImmediately(int32 Index);

	/** Wait before moving to the next location */
	void Wait();

	/** Get the Next Waypoint */
	void GetNextWaypoint();

	/** Use the Search Radius to find an enemy */
	void FindTarget();

	// Called when the game starts
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	/** Clear and set the waypoints array. Waypoints are the locations where this actor will move to. Set Via Blueprint. */
	UFUNCTION(BlueprintCallable, Category = "Move To Properties")
	void SetWaypoints(TArray<FVector> InWaypoints);

	/** Go Directly too this node. Loop must be false! */
	UFUNCTION(BlueprintCallable, Category = "Control")
	void SetNextWaypoint(int32 Index);

	/** Offset all the waypoints by the delta. The Initial Location becomes Actor Location + Delta */
	UFUNCTION(BlueprintCallable, Category = "Control")
	void MoveActorAndWaypointsDelta(FVector Delta, bool StartCycle = false);

	/** Offset all the waypoints by the delta. Initial Location = Actors Location at the time method was called */
	UFUNCTION(BlueprintCallable, Category = "Control")
	void MoveWaypointsDelta(FVector Delta);

	/** Set the Actor and waypoints potion to the new location */
	UFUNCTION(BlueprintCallable, Category = "Control")
	void MoveActorAndWaypoints(FVector NewLocation, bool StartCycle = false);

	/** Move the Waypoints using the NewLocation as an initial Location*/
	UFUNCTION(BlueprintCallable, Category = "Control")
	void MoveWaypoints(FVector NewLocation);

	UFUNCTION(BlueprintPure, Category = "Control")
	bool IsCompleted() const;

	UFUNCTION(BlueprintPure, Category = "Control")
	bool HasStarted() const;

	/** On Start Delegate */
	DECLARE_DELEGATE_OneParam(FOnStart, UMoveToLocation*);
	
	FOnStart OnStart;

	/** Multi Delegate for when movement has finished. Only called if loop false */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFinished, const int32&, LastIndex);

	UPROPERTY(BlueprintAssignable, Category = "Custom Event")
	FOnFinished OnFinished;

	/** Start the Component. Called from beign play if Manual Start is true */
	UFUNCTION(BlueprintCallable, Category = "Control")
	void Start();

	UFUNCTION(BlueprintPure, Category = "Navigation")
	float GetTravelProgress() const;

protected:
	/** Locations where this Actor can move to. If this is empty, the Actor will remain in place */
	UPROPERTY()
	TArray<FVector> Waypoints;

	/** Each waypoint in the Array has been traveled too */
	UPROPERTY()
	bool bCompleted;

	FVector GetWaypointLocation() const;

private:
	FTimerHandle MoveTo_TimerHandle;
	FTimerHandle Wait_TimerHandle;
	FTimerHandle Start_TimerHandle;

	TArray<FVector> InitialWaypoints;

	/** Travel Time Progress */
	float Progress;

	/** Can this Actor move? This will be set if there are any waypoints */
	bool bCanMove;

	/** Move to has been called */
	bool bMoveToOverride;
	
	/** Current Waypoint Index*/
	int32 WaypointIndex;

	/** Location at the start of the Lerp */
	FVector InitialLocation;

	/** By default, the waypoints are in local space. Add the players position to bring them to world space */
	UFUNCTION()
	void InitializeWaypoints();

	/** Waiting has completed, move to the next waypoint */
	void EndWait();

	/** Start. Called after thfe Start Delay. Call StartMovementCycle to move between waypoints */
	void StartMovement();

	UFUNCTION(Reliable, NetMultiCast, WithValidation)
	void MultiCastMoveTo(FVector Location);
	virtual void MultiCastMoveTo_Implementation(FVector Location);
	virtual bool MultiCastMoveTo_Validate(FVector Location) { return true; }

};
