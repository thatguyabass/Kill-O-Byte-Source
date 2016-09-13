// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Powers/Weapons/Weapon_Multi.h"
#include "Weapon_UniqueMulti.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AWeapon_UniqueMulti : public AWeapon_Multi
{
	GENERATED_BODY()
	
public:
	AWeapon_UniqueMulti();

	UPROPERTY(EditAnywhere, Category = "")
	TArray<TSubclassOf<ABaseProjectile>> ProjectileClasses;

	virtual void Fire(FVector Offset) override;
	
	virtual void ServerFireCustomMulti_Implementation(const FVector& InLocation, const TArray<FRotator>& InRotations, const TArray<TSubclassOf<ABaseProjectile>>& InProjectileClassess);

};
