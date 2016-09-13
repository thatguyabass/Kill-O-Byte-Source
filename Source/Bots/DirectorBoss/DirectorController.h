// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/BotStructs.h"
#include "Bots/Controller/BaseAIController.h"
#include "DirectorController.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ADirectorController : public ABaseAIController
{
	GENERATED_BODY()
	
public:
	ADirectorController(const class FObjectInitializer& PCIP);
	
	virtual void PostInitializeComponents() override;
	virtual void Possess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

	/** Decision State Ticks */
	/** If override true, Skip the timer and process the method body */
	void VulnerableTick(float DeltaTime, bool Override);
	void PostVulnerableTick(float DeltaTime);
	void TransitionTick(float DeltaTime);

	/** The Charge has finished and it is time to execute the ability based on the current battle state. */
	void ExecuteAbility();

	/** Clean up the ability allowing the encounter to continue */
	void CleanupAbility();

public:
	UFUNCTION(BlueprintPure, Category = "DirectorAI|State")
	EDirectorBattleState GetBattleState() const;

	UFUNCTION(BlueprintCallable, Category = "DirectorAI|State")
	void SetBattleState(EDirectorBattleState InBattleState);

	/** Return true, if all the battle states have been compelted */
	UFUNCTION(BlueprintPure, Category = "DirectorAI|State")
	bool IsFinalBattleState() const;

	/** Cycle the battle states to the next state using the order they are initialized in */
	UFUNCTION(BlueprintCallable, Category = "DirectorAI|State")
	void IncreaseBattleState(); 

	UFUNCTION(BlueprintPure, Category = "DirectorAI|State")
	EDirectorDecisionState GetDecisionState() const;

	UFUNCTION(BlueprintCallable, Category = "DirectorAI|State")
	void SetDecisionState(EDirectorDecisionState InDecisionState);

	/** Return true if the player is in a state where the Director can be damaged */
	UFUNCTION(BlueprintPure, Category = "DirectorAI|State")
	bool CanDamageDirector() const;

	/** Return true if in the Vulnerable Decision State */
	UFUNCTION(BlueprintPure, Category = "DirectorAI|State")
	bool IsVulnerable() const;

	UFUNCTION(BlueprintPure, Category = "DirectorAI|State")
	bool IsPostVulnerable() const;

	/** Return true if in the Transition Decision State */
	UFUNCTION(BlueprintPure, Category = "DirectorAI|State")
	bool IsTransitioning() const;

	UFUNCTION(BlueprintPure, Category = "DirectorAI|State")
	bool IsFinalDecisionState() const;

protected:
	/** Current Battle State */
	EDirectorBattleState BattleState;

	/** Current Decision State */
	EDirectorDecisionState DecisionState;

	/** Handle the new battle state */
	void HandleBattleState();

	/** Handle the new decision State */
	void HandleDecisionState();

public:
	/** Duration of the vulnerable state */
	UPROPERTY(EditAnywhere, Category = "DirectorAI|Vulnerable")
	float VulnerableDuration;

	/** Duration of the post vulnerable state. */
	UPROPERTY(EditAnywhere, Category = "DirectorAI|Vulnerable")
	float PostVulnerableDuration;

	/** Initialize the variables for the vulnerable state */
	void InitializeVulnerableState();

	/** Initialize the variables for the post vulnerable state */
	void InitializePostVulnerableState();

protected:
	/** Current time spent in the vulnerable state */
	float VulnerableProgress;

	/** Currnet Time spent in the post vulnerable state */
	float PostVulnerableProgress;

public:
	UPROPERTY(EditAnywhere, Category = "DirectorAI|Transition")
	float TransitionDuration;

	/** Percent of the max health the Bot will be initialized with during the final state */
	UPROPERTY(EditAnywhere, Category = "DirectorAI|Transition")
	float FinalHealthScale;

	/** Reinitialize the bosses health after a transition to a new phase. */
	void ReinitializeBossHealth();

protected:
	float TransitionProgress;

public:
	UFUNCTION(BlueprintPure, Category = "DirectorAI|Util")
	class ADirectorBoss* GetBoss();

	/** Reset the platform to the normal combat state */
	void ResetPlatform(bool NextWave);

};
