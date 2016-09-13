// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Controller/GruntAI.h"
#include "SpeedyBossAI.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FSpeedBossPhase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireRate")
	float FireRate;

};

UCLASS()
class SNAKE_PROJECT_API ASpeedyBossAI : public AGruntAI
{
	GENERATED_BODY()
	
public:
	ASpeedyBossAI(const class FObjectInitializer& PCIP);

	virtual void InitializeBlackboardKeys() override;

	virtual void Possess(APawn* InPawn) override;

	/** Wait at the Location and fire at the player. 
	*	@Return - Return true when the wait has been completed 
	*/
	UFUNCTION(BlueprintCallable, Category = "Bullet Hell")
	bool WaitAndFire(float DeltaTime);

	/** Fire the Bots secondary weapon 
	*	@Return - Return true when the secondary fire has completed */
	UFUNCTION(BlueprintCallable, Category = "Bullet Hell")
	bool FireSecondaryWeapon(float DeltaTime);

	virtual void FindTarget() override;

public:
	/** Trigger the Encounter to start. This will activate the Boss */
	void StartEncounter();

	UFUNCTION(BlueprintCallable, Category = "SpeedBoss Phase")
	void IncreaseBossPhase();

protected:
	UPROPERTY(EditAnywhere, Category = "Bullet Hell Properties")
	float WaitDuration;

	UPROPERTY(EditAnywhere, Category = "BulletHell Properties")
	float SecondaryFiringDuration;

	UPROPERTY(EditAnywhere, Category = "Bullet Hell Properties")
	TArray<FSpeedBossPhase> BossPhase;

	UPROPERTY(BlueprintReadWrite, Category = "Bullet Hell Properties")
	int32 PhaseIndex;

	/** Currnet Progress of the Wait and Fire. */
	UPROPERTY()
	float WaitProgress;

	/** Currnet Progrss of Secondary Fire */
	UPROPERTY()
	float SecondaryFiringProgress;

	/** Current Progress of the Wait and Fire, weapon fire rate */
	UPROPERTY()
	float FireProgress;

public:
	/** Percent of max health before moving into the next state */
	UPROPERTY(EditAnywhere, Category = "SpeedBoss|Damage")
	float DamageThreshold;

	/** Percent of max health the bot can take when slowed before the slow is removed the weapons are fired */
	UPROPERTY(EditAnywhere, Category = "SpeedBoss|Damage")
	float SlowDamageThreshold;

	/** Damage the Bot has taken */
	UFUNCTION()
	void DamageTaken(float Damage);

protected:
	/** Damage Taken during the current phase */
	UPROPERTY()
	int32 StoredDamage;

	/** Damage Taken While Slowed */
	UPROPERTY()
	int32 StoredSlowDamage;

public:
	UFUNCTION(BlueprintCallable, Category = "SpeedyBossAI")
	AActor* GetBossSpawnArea();

public:
	UFUNCTION(BlueprintPure, Category = "SpeedyBossAI")
	bool GetEncounterStarted() const;

	UFUNCTION(BlueprintCallable, Category = "SpeedyBossAI")
	void SetEncounterStarted(bool Value);

	UFUNCTION(BlueprintPure, Category = "SpeedyBossAI")
	bool GetSecondaryFiring() const;

	UFUNCTION(BlueprintCallable, Category = "SpeedyBossAI")
	void SetSecondaryFiring(bool Value);

protected:
	/** Blackboard Key EncounterStarted */
	int32 EncounterKey;
	
	/** Blackboard Key SecondaryWeaponFiring */
	int32 SecondaryFiringKey;

};
