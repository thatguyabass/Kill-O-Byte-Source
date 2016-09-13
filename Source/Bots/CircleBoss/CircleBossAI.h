// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Controller/BaseAIBoss.h"
#include "CircleBossAI.generated.h"


UENUM(BlueprintType)
enum class ECircleBossState : uint8
{
	Idle			= 0,
	WallSetup		= 1,
	WallInProgress	= 2,
	WallTakeDown	= 3,
	ArmSetup		= 4,
	ArmInProgress	= 5,
	ArmTakeDown		= 6
};

UENUM(BlueprintType)
enum class EWallMovementState : uint8
{
	Movement = 0,
	Crush	= 1
};

UCLASS()
class SNAKE_PROJECT_API ACircleBossAI : public ABaseAIBoss
{
	GENERATED_BODY()
	
public:
	ACircleBossAI(const class FObjectInitializer& PCIP);
	
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void IdleTick(float DeltaTime);
	void WallInProgressTick(float DeltaTime);
	void ArmsSetupTick(float DeltaTime);

	/** Start the Fight */
	virtual void StartEncounter() override;

public:
	UPROPERTY(EditAnywhere, Category = "Timer")
	float WallStartDelay;

	/** Delay after a successful crush before the wall resets and moves again */
	UPROPERTY(EditAnywhere, Category = "Timer")
	float CrushRestartDelay;

	/** Setup the Walls to prepare for the wall phase */
	UFUNCTION(BlueprintCallable, Category = "Wall Phase")
	void StartWallSetup();

	/** Tick the wall movement. Wall actors and movement takes place in the Bot Class */
	UFUNCTION(BlueprintCallable, Category = "Wall Phase")
	void MoveWalls(float DeltaTime);

	/** Function to collect the float from the WallMovementTimeline */
	UFUNCTION()
	void SetWallTimelineProgress(float Value);

	/** Move the walls out of sight */
	UFUNCTION(BlueprintCallable, Category = "Wall Phase")
	void FinishWallMovement(bool bReset = true);

	UFUNCTION()
	void InternalFinishWallMovement();

	UFUNCTION(BlueprintCallable, Category = "Wall Phase")
	void SetMoveWallReady();

	UPROPERTY(EditAnywhere, Category = "Wall Phase")
	UCurveFloat* WallMovementCurve;

	/** Time when the Walls will switch to the crushing phase */
	UPROPERTY(EditAnywhere, Category = "Wall Phase")
	float CrushTime;

protected:
	/** Wall Movement Timeline */
	FTimeline WallMovementTimeline;

	/** Resets and starts the abilities from the start. 
	*	Transitions to the Idle state and calls the Wallmovement start
	*/
	UFUNCTION()
	void ResetAbilities(float Duration);

	/** Elapsed time since the wall movement has started. Used against the spline duration to find location and rotation */
	UPROPERTY()
	float TotalProgress;

	/** Elapsed time since the last move state was passed */
	UPROPERTY()
	float MoveProgress;

	/** Target Time for this wall movement state */
	float MoveDuration;

	/** Values used to determine how far along the spline has been traveled */
	/** Initial Progress during each phase of the movement states. */
	float InitialMoveProgress;
	/** Target Progress during each phase of the movements states. */
	float TargetMoveProgress;

	/** Travel time for the walls to rise and fall */
	float WallRiseDuration;

	/** is the wall move ability ready? */
	bool bMoveWallReady;

	/** Track which stage of the wall duration we are in. */
	EWallMovementState WallMovementState;

	static const int32 MaxWallMovementStateCount;

	UFUNCTION()
	void SetWallCrushState();

	/** Timer handle for each movement state */
	FTimerHandle MoveDuration_TimerHandle;

	FTimerHandle WallSetup_TimerHandle;
	FTimerHandle WallInProgress_TimerHandle;

public:
	/** Delay between wall movement stage finishing and the arm phase starting */
	UPROPERTY(EditAnywhere, Category = "Timer")
	float ArmSetupDelay;

	/** Delay between the Arms comming up and the walls moving again. */
	UPROPERTY(EditAnywhere, Category = "Timer")
	float RestartDelay;

	UFUNCTION(BlueprintCallable, Category = "DEBUG")
	void TriggerArmPhase();

	/** Start moving the arms in a Forward Sequence */
	UFUNCTION(BlueprintCallable, Category = "Arm Phase")
	void StartArmSetup();

	/** Start a Reverse Movement Sequence */
	UFUNCTION(BlueprintCallable, Category = "Arm Phase")
	void StartArmTakedown();

	/** State that the arms are in position to block the player */
	void SetArmsInPosition();

public:
	void ClearTimers();

protected:
	FTimerHandle ArmSetup_TimerHandle;
	FTimerHandle ArmInProgress_TimerHandle;
	FTimerHandle LazerCharge_TimerHandle;

public:
	/** How long the lazer takes to charge before it can be shot */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timer")
	float LazerChargeDuration;

	/** How long the lazer will be firing for */
	UPROPERTY(EditAnywhere, Category = "Timer")
	float LazerFireDuration;

	UPROPERTY(EditAnywhere, Category = "Timer")
	float ArmsTakedownDelay;

	UFUNCTION(BlueprintPure, Category = "Timer")
	float GetLazerChargeProgress();

	/** Start charging the lazer */
	UFUNCTION()
	void ChargeLazer();

	/** Fire the Lazer*/
	UFUNCTION()
	void FireLazer();

	/** Stop all timers and rest the variables for next use */
	UFUNCTION()
	void ResetLazer();

	/** Call from a Timer Delegate. Auto Uses RestartDelay as the timer duration */
	void ResetAbilitiesTimer();

protected:
	/** Can the boss face the target? */
	UPROPERTY()
	bool bCanFollowPlayer;

	/** Delay between damage being applied to the player. */
	UPROPERTY(EditAnywhere, Category = "Timer")
	float LazerDamageDelay;

	FTimerHandle LazerDamage_TimerHandle;

	/** Damage the Actors inside the Lazer Box Overlap */
	UFUNCTION()
	void ApplyLazerDamage();

public:
	UFUNCTION(BlueprintCallable, Category = "State")
	void SetState(ECircleBossState InState);

	UFUNCTION(BlueprintPure, Category = "State")
	ECircleBossState GetState() const;

	void SetWallSetup();
	void SetWallInProgress();
	void SetArmInProgress();

	/** Can the Boss Spawn Bots? */
	UFUNCTION()
	bool CanSpawnBots();

protected:
	UPROPERTY()
	ECircleBossState BossState;

public:
	/** Declare delegate event for state change. */
	DECLARE_DELEGATE_OneParam(FOnWallMovementStateChange, EWallMovementState);

	/** Delegate event when the state changes */
	FOnWallMovementStateChange OnWallMovementStateChange;

	virtual void SetTarget(AActor* InActor) override;

};
