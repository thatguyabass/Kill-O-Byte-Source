// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/BotStructs.h"
#include "Bots/Bot/BaseBoss.h"
#include "DirectorBoss.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ADirectorBoss : public ABaseBoss
{
	GENERATED_BODY()
	
public:
	ADirectorBoss();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* Controller, AActor* DamageCauser) override;

public:
	/** Start the director moveing along the spline path */
	void StartMovementState();

	/** Blueprint Event to trigger the bot to start moving along the spline path */
	UFUNCTION(BlueprintImplementableEvent, Category = "DirectorBoss|Movement Event")
	void StartDirectorMovement();

public:
	UFUNCTION(BlueprintCallable, Category = "DirectorBoss|State")
	void SetDecisionState(EDirectorDecisionState InState);

	/** Execute a blueprint event for the new state */
	UFUNCTION(BlueprintImplementableEvent, Category = "DirectorBoss|State")
	void OnDecisionStateChange(EDirectorDecisionState NewState);

	/** Spawn the correct actors for the current battle state */
	void InitializeVulnerableState(EDirectorBattleState InBattleState);

	/** Initialize Post Vulnerable Variables */
	void InitializePostVulnerableState();

	/** Blueprint Event to reset the platform to the full combat state */
	UFUNCTION(BlueprintImplementableEvent, Category = "DirectorAI|Transition")
	void ResetPlatform(bool NextWave);

	UFUNCTION(BlueprintImplementableEvent, Category = "Director|Final")
	void InitializeFinalDecisionState();

public:
	/** Outer Spawn Locations */
	UPROPERTY(EditAnywhere, Category = "DirectorBoss|SpawnLocations")
	TArray<AActor*> OuterSpawnLocations;

	/** Inner Spawn Locations */
	UPROPERTY(EditAnywhere, Category = "DirectorBoss|SpawnLocations")
	TArray<AActor*> InnerSpawnLocations;

	UPROPERTY(EditAnywhere, Category = "DirectorBoss|Speed")
	TSubclassOf<class ADeathSpeedBot> DeathBotClass;

	/** Spawn the Death Bots at the inner locations */
	UFUNCTION()
	void SpawnDeathBots();

	/** Activate the Death Scenario with the death bots */
	UFUNCTION()
	void ActivateDeathBots();

	UFUNCTION(BlueprintCallable, Category = "DirectorBoss|Battle State Speed")
	void CleanupDeathBots();

	UFUNCTION(BlueprintPure, Category = "DirectorBoss|State")
	bool AllBotsCleared();

protected:
	/** Bots that need to be cleaned */
	UPROPERTY()
	TArray<class ABot*> DeathBots;

public:
	/** Mortar Bots Base class */
	UPROPERTY(EditAnywhere, Category = "DirectorBoss|Mortar")
	TSubclassOf<class ABot> MortarBotClass;

	UPROPERTY(EditAnywhere, Category = "DirectorBoss|Mortar")
	TSubclassOf<class ABot> InnerMortarBotClass;

	/** Spawn the Mortars at the outer locations */
	UFUNCTION(BlueprintCallable, Category = "DirectorBoss|Mortar")
	void SpawnOuterMortars();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "DirectorBoss|Mortar")
	void OnSpawnOuterMortars();

	/** Spawn the mortars at the inner locations */
	UFUNCTION(BlueprintCallable, Category = "DirectorBoss|Mortar")
	void SpawnInnerMortars();

	/** Fire Rate for each weapon fired. */
	UPROPERTY(EditAnywhere, Category = "DirectorBoss|Mortar")
	float BarrageFireRate;

	/** Start The mortar Barrage */
	UFUNCTION()
	void StartBarrage();

	/** Tick the Inner Mortar Bots to control the mortar barrage */
	UFUNCTION()
	void BarrageTick(float DeltaTime);

	/** Stop and cleanup the barrage */
	UFUNCTION()
	void FinishBarrage();

protected:
	/** Mortar Bot References */
	UPROPERTY(BlueprintReadOnly, Category = "DirectorBoss|Mortar")
	TArray<class ABot*> MortarBots;

	/** Next Bot to fire its weapon */
	UPROPERTY()
	int32 MortarBotIndex;

	/** Flag to track when to increase the MortarBotIndex */
	bool bFireNext;

	/** Flag, Track when a barrage is inprogress */
	bool bBarrageInprogress;

	/** Current time before the next mortar is fired */
	float BarrageProgress;

	/** Fire a mortar bot */
	void FireMortar();

	/** Verify the mortar bots health. If their health drops below Zero, remove them from the array */
	void MortarHealthCheck();

	/** Spawn the Bots */
	UFUNCTION()
	void SpawnBots(TSubclassOf<class ABot> InBotClass, TArray<AActor*>& SpawnLocations, TArray<class ABot*>& OutBots);

public:
	/** Lazer Head for the Lazer State */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DirectorBoss|Lazer")
	class ALazerBossHead* LazerBossHead;

	UPROPERTY(EditAnywhere, Category = "DirectorBoss|Lazer")
	AActor* HeadSpawnLocation;

	UPROPERTY(EditAnywhere, Category = "DirectorBoss|Lazer")
	float ReturnToSpawnInterpRate;

	/** Trigger LazerBossHead Cleanup state */
	UFUNCTION()
	void StartHeadCleanup();

protected:
	UPROPERTY()
	bool bHeadMovement;

	/** Spawn the lazer Head */
	UFUNCTION()
	void StartLazerBossSequence();

	/** Return the Head to the Spawn Location */
	UFUNCTION()
	void ReturnHeadToSpawn();

	/** Clean up the Lazer Boss Head */
	UFUNCTION()
	void CleanupLazerBossHead();

public:
	/** Transition Start Event */
	UFUNCTION(BlueprintImplementableEvent, Category = "DirectorBoss|Animation")
	void OnTransitionStart();

	/** Transition End Event */
	UFUNCTION(BlueprintImplementableEvent, Category = "DirectorBoss|Animation")
	void OnTransitionEnd();

public:
	/** Damage Threshold Percent is calculated by multiplaying the Percent by Max Health */
	UPROPERTY(EditAnywhere, Category = "Damage")
	float DamageThresholdPercent;

	UPROPERTY()
	int32 DamageTaken;

	/** If true, damage taken will be tracked */
	UPROPERTY()
	bool bTrackDamageTaken;

	UFUNCTION()
	void ResetDamageTaken(bool TrackDamageFlag);

};
