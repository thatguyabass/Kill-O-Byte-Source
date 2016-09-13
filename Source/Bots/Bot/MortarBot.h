// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Bot/Bot.h"
#include "MortarBot.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AMortarBot : public ABot
{
	GENERATED_BODY()
	
public:
	AMortarBot();
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USceneComponent* WeaponPlacement;

	UPROPERTY(EditAnywhere, Category = "MortarBot|Properties")
	TSubclassOf<class ARocketTankTargetDecal> TargetClass;

	/** Ideal distance of the target to this actor */
	UPROPERTY(EditAnywhere, Category = "MortarBot|Properties")
	float IdealDistance;

	/** Radius around the target where the final target may be offset by */
	UPROPERTY(EditAnywhere, Category = "MortarBot|Properties")
	float MissFireRadius;

	virtual void SpawnWeapon() override;

	virtual void FireWeapon() override;

protected:
	class ARocketTankTargetDecal* ProjectileTarget;

};
