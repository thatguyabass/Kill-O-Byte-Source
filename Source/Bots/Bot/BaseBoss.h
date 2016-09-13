// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"
#include "Bots/Bot/Bot.h"
#include "BaseBoss.generated.h"

UCLASS()
class SNAKE_PROJECT_API ABaseBoss : public ABot
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseBoss();

	/** Fire Mode State */
	UPROPERTY(EditAnywhere, Category = "Weapons")
	EFireMode FireMode;

	UPROPERTY(EditAnywhere, Category = "Weapons")
	TArray<TSubclassOf<class AWeapon>> WeaponClasses;

	/** Start the Boss Fight! */
	UFUNCTION(BlueprintCallable, Category = "Start Boss Fight")
	virtual void StartEncounter();

	virtual void PostInitializeComponents() override;
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void SpawnWeapon() override;

	/** Fire Weapons */
	virtual void FireWeapon() override;

	/** Called before a weapon is about to spawn its projectile. Override for custom variable passing to the weapon before spawning occurs. */
	virtual void PreProjectileSpawn(int32 WeaponIndex);

	virtual void TargetInstigator(AController* Instigator) override;
	
protected:
	virtual void SimultaneousFire();
	virtual void SequentialFire();

	/** Weapons owned by this boss */
	TArray<class AWeapon*> Weapons;

	FTimerHandle Fire_TimerHandle;

	/** Current Fire index */
	int32 FireIndex;

	/** flag to increment the Fire Index */
	bool bNextIndex;

	void SetNextFireIndex();

	/** Initialize the Weak Points and Set the owner as this */
	UFUNCTION(BlueprintCallable, Category = "Weap Points")
	virtual void SetWeakPoints(TArray<class AWeakPoint*> InArray);

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Weak Points")
	TArray<class AWeakPoint*> WeakPoints;

};
