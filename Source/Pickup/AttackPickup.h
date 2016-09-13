// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup/Pickup.h"
#include "AttackPickup.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AAttackPickup : public APickup
{
	GENERATED_BODY()
	
public:
	AAttackPickup(const class FObjectInitializer& PCIP);
	
	UPROPERTY(EditAnywhere, Category = "Pickup|Weapon")
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Pickup|Weapon")
	float Duration;

	virtual void PostInitializeComponents() override;

	virtual void GiveTo(class ASnakeLink* Link) override;
	
};
