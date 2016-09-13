// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Bot/BaseBoss.h"
#include "RocketTank.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ARocketTank : public ABaseBoss
{
	GENERATED_BODY()
	
public:
	ARocketTank();

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UBoxComponent* HitBox;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UBoxComponent* HitBox02;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USceneComponent* WeaponPlacement01;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USceneComponent* WeaponPlacement02;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USceneComponent* WeaponPlacement03;

	UPROPERTY(EditAnywhere, Category = "RocketTank|Targets")
	TSubclassOf<class ARocketTankTargetDecal> TargetClass;

	/** Range Around player that rockets can land */
	UPROPERTY(EditAnywhere, Category = "RocketTank|Targets")
	float BarrageSpread;

	UPROPERTY(EditAnywhere, Category = "RocketTank|Targets")
	float IdealDistance;

	/** Threshold of the phase transition */
	UPROPERTY(EditAnywhere, Category = "RocketTank|Phase")
	float PhaseTransitionThreshold;

	/** Spawn teh Weapons for this boss */
	virtual void SpawnWeapon() override;

	/** Before the projectile has spawned. */
	virtual void PreProjectileSpawn(int32 WeaponIndex) override;

	/** Fire all guns during the same tick */
	virtual void SimultaneousFire() override;

protected:
	/** List of Scene components used for weapon placement */
	TArray<USceneComponent*> WeaponPlacementComponents;

	/** Actor References to the Weapon targets */
	TArray<class ARocketTankTargetDecal*> WeaponTargets;

	/** Total number of weapons that will spawn */
	static const int32 WeaponCount;
	/** Total number of targets that will spawn */
	static const int32 TargetCount;
	
	/** Loop through the Weapon Targets looking for an inactive target. When found it is then moved to the correct location before being returned */
	AActor* GetAvailableTarget();

	virtual void HideAndDestroy() override;

public:
	virtual void PostInitializeComponents() override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, class AActor* DamageCauser) override;

	UFUNCTION()
	void BeginOverlapCheck(class AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Stop the timers for barrage and chagre. State is set to Idle */
	UFUNCTION(BlueprintCallable, Category = "Control")
	void StopActionAbilities();

	/** Reset the timers for barrage and charge. State remains unchanged */
	UFUNCTION(BlueprintCallable, Category = "Control")
	void StartActionAbilities();

protected:
	/** Phase Index */
	static const int32 ThirdPhaseIndex;

public:
	virtual void StartEncounter() override;

	/** Initialize Bot for Charge */
	UFUNCTION()
	void StartCharge();

	/** Reset Vaiables for Post Charge */
	UFUNCTION()
	void FinishCharge();

	/** Blueprint Event called when Charge Started */
	UFUNCTION(BlueprintImplementableEvent, Category = "Charge")
	void OnChargeStart(FVector Direction, FVector Start, FVector Destination);

	/** Blueprint Event called when Charge Finished */
	UFUNCTION(BlueprintImplementableEvent, Category = "Charge")
	void OnChargeFinished();

	/** Blueprint Exposed Hit Impact Normal */
	UPROPERTY(BlueprintReadOnly, Category = "Charge")
	FVector ChargeHitNormal;

};
