// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Powers/Projectiles/DefaultProjectile.h"
#include "BlastProjectile.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ABlastProjectile : public ADefaultProjectile
{
	GENERATED_BODY()
	
public:
	ABlastProjectile(const class FObjectInitializer& PCIP);

	UPROPERTY(EditAnywhere, Category = "Blast|Component")
	TSubclassOf<class AWeapon> WeaponClass;

	virtual void PostInitializeComponents() override;

	void Detonate();

private:

	void SpawnWeapon();

	class AWeapon* Weapon;

	virtual void OnImpact(const FHitResult& Hit) override;

};
