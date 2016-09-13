// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Bot/Bot.h"
#include "Turret.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ATurret : public ABot
{
	GENERATED_BODY()
	
public:
	ATurret();
	
	/** Weapon's Fire Rate */
	UPROPERTY(EditAnywhere, Category = "Turret|Combat")
	float FireRate;

	/** Does this turret start firing as soon as the level loads? if false, Call StartFire Manually */
	UPROPERTY(EditAnywhere, Category = "Turret|Settings")
	bool bFireOnSpawn;
	
	/** Start Firing projectiles */
	UFUNCTION(BlueprintCallable, Category = "Turret|Fire")
	virtual void StartFire();

	/** Spawn the weapon and start the fire sequence */
	virtual void SpawnWeapon() override;

protected:
	FTimerHandle TimerHandle;

	virtual void Dead(class ASnakePlayerState* Killer) override;

};
