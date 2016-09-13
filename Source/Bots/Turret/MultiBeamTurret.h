// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Turret/BeamTurret.h"
#include "MultiBeamTurret.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AMultiBeamTurret : public ABeamTurret
{
	GENERATED_BODY()
	
public:
	AMultiBeamTurret();
	
	/** Due to limitations in the lazer weapon each lazer needs to be spawned directly. Spawn weapon will get the weapon data from the DataTable */
	virtual void SpawnWeapon() override;

	/** Trigger the Fire Weapon Command and start the pre fire particle */
	virtual void FireWeapon() override;

	/** Spawn the lazers for this turret */
	void FireWeaponInternal();

	virtual void Dead(class ASnakePlayerState* Killer) override;

	UPROPERTY(EditAnywhere, Category = "Lazer")
	TSubclassOf<class ALazor> LazerClass;

	UPROPERTY(EditAnywhere, Category = "Lazer")
	float WeaponSpread;

	UPROPERTY(EditAnywhere, Category = "Lazer")
	int32 WeaponCount;

	/** How long the Turret and lazers will be fired before being turned off */
	UPROPERTY(EditAnywhere, Category = "Lazer")
	float LifeTime;

	/** If true, the actor will self-destruct after its first shot */
	UPROPERTY(EditAnywhere, Category = "Lazer")
	bool bPlayOnce;

	virtual void HideAndDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "Lazer|Control")
	void FadeOutLazers();

	UFUNCTION(BlueprintPure, Category = "Lazer")
	float GetLazerDuration() const;

public:
	/** If true, a pre fire particle effect will spawn. Uses Spawn Effect */
	UPROPERTY(EditAnywhere, Category = "Lazer|PreFire")
	bool bUsePreFire;

	/** Duration of the pre fire particle effect before weapon fire occurs. */
	UPROPERTY(EditAnywhere, Category = "Lazer|PreFire")
	float PreFireDuration;

private:
	UPROPERTY()
	TArray<class ALazor*> Lazers;

	FTimerHandle Dead_TimerHandle;
	FTimerHandle Purge_TimerHandle;
	FTimerHandle Fire_TimerHandle;

	/** if the lazers duration has expired, empty the lazer array */
	void PurgeLazerArray();

	UPROPERTY(EditAnywhere, Category = "Lazer")
	UDataTable* WeaponDataTable;

	UPROPERTY(EditAnywhere, Category = "Lazer")
	FName WeaponRowName;

	static const FString WeaponDataContext;

	UPROPERTY()
	int32 LazerDamage;

	UPROPERTY()
	float LazerDuration;

};
