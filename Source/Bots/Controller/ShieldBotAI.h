// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Controller/GruntAI.h"
#include "ShieldBotAI.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AShieldBotAI : public AGruntAI
{
	GENERATED_BODY()
	
public:
	AShieldBotAI(const class FObjectInitializer& PCIP);

	virtual void Tick(float DeltaTime) override;

	virtual void InitializeBlackboardKeys() override;
	
	virtual void SetTarget(AActor* InTarget) override;

	/** Speed which this actor rotates when facing the target */
	UPROPERTY(EditAnywhere, Category = "ShieldBotAI|Properties")
	float TurnRotationRate;

	/** Check for line of sight, and deply the shield. If line of sight is lost, remove the shield. */
	UFUNCTION(BlueprintCallable, Category = "ShieldBot")
	void DeployShieldOnSight();

	/** Spawn the Nullification EMP under the target */
	UFUNCTION(BlueprintCallable, Category = "ShieldBot")
	bool ActivateNullifyingEMP();

	void FindFocus();

	/** Remove the actors focus on the target */
	void RemoveFocus();

	UFUNCTION(BlueprintCallable, Category = "ShieldBot")
	bool IsShieldShutdown();

	/** Delay between null emp's */
	UPROPERTY(EditAnywhere, Category = "ShieldBotAI|Proeprties")
	float NullDelayDuration;

	/** bCanUseNullEMP getter */
	UFUNCTION(BlueprintPure, Category = "ShieldBotAI")
	bool GetNullEMPState() const;

protected:
	/** If true, look towards the target at all times */
	UPROPERTY()
	bool bFaceTarget;

	/** Current progress between null deployments */
	UPROPERTY()
	float NullDelayProgress;

	bool bCanUseNullEMP;

public:
	UFUNCTION(BlueprintCallable, Category = "ShieldBot|Key")
	void SetRetreat(bool InValue);

	UFUNCTION(BlueprintPure, Category = "ShieldBot|Key")
	bool GetRetreat() const;

protected:
	int32 RetreatKey;

};
