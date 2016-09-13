// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Bot/BaseBoss.h"
#include "SpeedyBoss.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ASpeedyBoss : public ABaseBoss
{
	GENERATED_BODY()
	
public:
	ASpeedyBoss();
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USceneComponent* WeaponPlacement;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USceneComponent* SecondaryWeaponPlacement;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USceneComponent* WeakPointPlacement;

	UPROPERTY(EditAnywhere, Category = "Bot|POI")
	struct FPointOfInterestGroup BossPointOfInterestGroup;

	virtual void PostInitializeComponents() override;

	virtual void SpawnWeapon() override;

	UPROPERTY(EditAnywhere, Category = "Weapons")
	TSubclassOf<class AWeapon> SecondaryWeaponClass;

	UPROPERTY()
	class AWeapon* SecondaryWeapon;

	/** Override the base weapon using the Weapon_Multi SceneComponent Fire Method */
	virtual void PreProjectileSpawn(int32 WeaponIndex) override;

	/** Override the Weapon FireRate. */
	void OverrideWeaponFireRate(float InFireRate);

	/** Fire the Secondary Weapon */
	UFUNCTION(BlueprintCallable, Category = "SpeedBoss|SecondaryWeapon")
	void FireSecondaryWeapon();

	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void StartEncounter() override;

	virtual void HideAndDestroy() override;

public:
	/** Slow down the Bots movement speed for a duration */
	virtual void ApplySlow(FSpeedPower Slow) override;

	UFUNCTION(BlueprintPure, Category = "SpeedBoss|Slow")
	bool GetSlowImmunity() const;

	UFUNCTION(BlueprintCallable, Category = "SpeedBoss|Slow")
	void SetSlowImmunity(bool InImmunity);

	UFUNCTION(BlueprintCallable, Category = "SpeedBoss|Slow")
	void RemoveSlow();

	/** How long the boss will remain at max slow before finishing the movement pass */
	UPROPERTY(EditAnywhere, Category = "SpeedBoss|Slow")
	float MaxSlowDuration;

protected:
	/** If true, the bot is immune to any new slows. */
	UPROPERTY()
	bool bSlowImmunity;

public:
	UPROPERTY(EditAnywhere, Category = "SpeedBoss|WeakPoint")
	float OrbitDuration;

	UPROPERTY(EditAnywhere, Category = "SpeedBoss|WeakPoint")
	float OrbitRadius;

protected:
	UPROPERTY()
	float OrbitProgress;

	UPROPERTY()
	FRotator OrbitAmount;

public:
	/** This is a blueprint only actor. Will need to be referenced in BP! */
	UPROPERTY(EditAnywhere, Category = "SpeedBoss|Spawn Area")
	AActor* SpawnArea;

	UPROPERTY(EditAnywhere, Category = "SpeedBoss|Rotation")
	float RotationRate;

protected:
	float RotationRateScale;

	virtual void ApplyMovementChange() override;

};
