// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Controller/BaseAIController.h"
#include "BaseAIBoss.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ABaseAIBoss : public ABaseAIController
{
	GENERATED_BODY()
	
public:
	ABaseAIBoss(const class FObjectInitializer& PCIP);

	virtual void FindTarget() override;
	
	/** Start the Boss Fight! */
	virtual void StartEncounter();

	UFUNCTION(BlueprintPure, Category = "Encounter")
	bool GetEncounterStarted();

protected:
	/** Track if the boss has encounted the player */
	UPROPERTY()
	bool bEncounterStarted;

};
