// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Controller/MortarAI.h"
#include "ShieldMortarAI.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AShieldMortarAI : public AMortarAI
{
	GENERATED_BODY()
	
public:
	AShieldMortarAI(const class FObjectInitializer& PCIP);
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetTarget(AActor* Target) override;

	UFUNCTION(BlueprintCallable, Category = "MortarAI|Shield")
	void SetShieldActive(bool Active);

	UFUNCTION(BlueprintPure, Category = "MortarAI|Shield")
	bool IsShieldActive() const;

};
