// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/SpeedyBoss/SpeedyBoss.h"
#include "SpeedyBot.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ASpeedyBot : public ASpeedyBoss
{
	GENERATED_BODY()
	
public:
	ASpeedyBot();
	
	virtual void PostInitializeComponents() override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void HideAndDestroy() override;

};
