// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Powers/Weapons/Weapon.h"
#include "Weapon_Special.generated.h"

/**
 *  Special Weapon to fire the Lazor projectile
 */
UCLASS()
class SNAKE_PROJECT_API AWeapon_Special : public AWeapon
{
	GENERATED_BODY()
	
public:
	AWeapon_Special();
	
	UPROPERTY(EditAnywhere, Category = "Special Projectile")
	TSubclassOf<class ALazor> LazerClass;

	/** How long the lazer will be fired for */
	UPROPERTY()
	float Duration;

	/** If true, this weapon will bypass god mode and damage the actor */
	UPROPERTY(EditAnywhere, Category = "Damage Override")
	bool bForceDamageOverride;

	virtual void Tick(float DeltaTime) override;

	/** Default override to fire the projectile */
	virtual void Fire(FVector FireOffset) override;
	
	virtual bool ProjectileBeingFired() override;

	UFUNCTION(BlueprintPure, Category = "Lazer Getter")
	float GetLazerDuration() const;

	/** Stop the lazer from being fired */
	virtual void TerminateProjectile() override;

	UFUNCTION()
	class ALazor* GetLazer();

protected:
	/** Spawn and attach the Lazor */
	void FireLazer(FVector FireOffset);

	/** The current Lazer Projectile */
	class ALazor* Lazer;

	virtual void GetTableData() override;

};
