// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Turret/Turret.h"
#include "BeamTurret.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ABeamTurret : public ATurret
{
	GENERATED_BODY()
	
public:
	ABeamTurret();

	virtual void SpawnWeapon() override;
	virtual void PostSpawnWeapon() override;

	/** Use the Base Turret Fire Structure. Repeatedly fire the weapons */
	UPROPERTY(EditAnywhere, Category = "BeamTurret|Properties")
	bool bMultiFire;

	UPROPERTY(EditAnywhere, Category = "BeamTurret|Properties")
	bool bForceDamageOverride;

	virtual void FireWeapon() override;

	/** Stop the lazer from firing */
	UFUNCTION(BlueprintCallable, Category = "BeamTurret|Method")
	void TerminateProjectile();

	UFUNCTION(BlueprintCallable, Category = "BeamTarget|Method")
	class ABeamTarget* GetBeamTarget();

protected:
	/** Weapon References */
	UPROPERTY()
	TArray<class AWeapon*> BeamTurretWeapons;

};
