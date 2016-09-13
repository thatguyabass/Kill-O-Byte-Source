// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Bot/Bot.h"
#include "SeekerBot.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ASeekerBot : public ABot
{
	GENERATED_BODY()
	
public:
	ASeekerBot();
	
	UPROPERTY(EditAnywhere, Category = "Seeker")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY()
	float ExplosionRadius;

	UPROPERTY()
	int32 Damage;

	virtual void PostInitializeComponents() override;

	virtual void SpawnDeathVFX() override;

	UPROPERTY(EditAnywhere, Category = "Bot|DeathVFX")
	FName SizeParameterName;

protected:
	UPROPERTY()
	UDataTable* WeaponDataTable;

};
