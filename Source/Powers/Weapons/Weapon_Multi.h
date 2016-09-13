// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Powers/Weapons/Weapon_Single.h"
#include "Weapon_Multi.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AWeapon_Multi : public AWeapon_Single
{
	GENERATED_BODY()

public:
	AWeapon_Multi();

	UPROPERTY(EditAnywhere, Category = "Weapon|Property")
	int32 ProjectileCount;

	UPROPERTY(EditAnywhere, Category = "Weapon|Property")
	float RotationSpread;

	UPROPERTY(EditAnywhere, Category = "Weapon|Property")
	EFireDirection FireDirectionOverride;

	/** Gather the Data to spawn a Projectile */
	virtual void Fire(FVector FireOffset) override;

	virtual void Fire(USceneComponent* InComponent, FRotator Rotation = FRotator::ZeroRotator) override;

	/** Fire Multiple Projectiles. If component is valid, use it as the spawn transform */
	void FireMulti(FVector FireOffset, USceneComponent* InComponent = nullptr);

};
