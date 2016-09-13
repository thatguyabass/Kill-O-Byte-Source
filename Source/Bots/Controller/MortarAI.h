// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Controller/GruntAI.h"
#include "MortarAI.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AMortarAI : public AGruntAI
{
	GENERATED_BODY()
	
public:
	AMortarAI(const class FObjectInitializer& PCIP);
	
	/** Place the Weapon Target Around the target actor */
	void SetWeaponTarget(AActor* WeaponTarget, float SearchRadius = 500.0f);

	virtual void SetTarget(AActor* InTarget) override;
	
};
