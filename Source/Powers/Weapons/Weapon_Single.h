// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Powers/Weapons/Weapon.h"
#include "Weapon_Single.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AWeapon_Single : public AWeapon
{
	GENERATED_BODY()

public:
	AWeapon_Single();

	/** Gather the Data to spawn a Projectile */
	virtual void Fire(FVector FireOffset) override;

	/** Fire a Single Projectile */
	void FireSingle(FVector FireOffset);

	/** Use the Components World location and rotation as spawn transform for this weapon */
	virtual void Fire(USceneComponent* InComponent, FRotator ActorRotation = FRotator::ZeroRotator);
		
	/** Fire Projectile */
	virtual void ServerFire_Implementation(const FVector& Locations, const TArray<FRotator>& Rotations, int32 Index) override;

};
