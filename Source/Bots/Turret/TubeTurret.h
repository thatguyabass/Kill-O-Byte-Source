// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Bot/SwarmBotBee.h"
#include "TubeTurret.generated.h"

UENUM(BlueprintType)
enum class ETubeTurretState : uint8
{
	Initialize,
	Phase01,
	Phase02,
	Dead
};

UCLASS()
class SNAKE_PROJECT_API ATubeTurret : public ASwarmBotBee
{
	GENERATED_BODY()
	
public:
	ATubeTurret();

	/** Collision that will act as the primary hit box */
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UBoxComponent* HitBox;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* WeakPointPlacement;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UAudioComponent* MeshChangeSFXComponent;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	TArray<UStaticMesh*> MeshPhases;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* WeaponFireSFXPhase02;

	virtual void SpawnWeapon() override;

	virtual void FireWeapon() override;

	virtual void PostInitializeComponents() override;

public:
	UPROPERTY(EditAnywhere, Category = "Health")
	TSubclassOf<class AWeakPoint> WeakPointClass;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetHealth(int32 InHealth);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void ReduceHealth(int32 InDamage, AActor* DamageCauser);

	/** Override the base TakeDamage Method */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, class AActor* DamageCauser) override;

protected:
	/** Total And Starting health value */
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	int32 MaxHealth;

	/** Current Health Value */
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	int32 Health;

	/** Death Flag */
	bool bDead;

	/** Weak Point Reference */
	class AWeakPoint* WeakPoint;

public:
	UFUNCTION(BlueprintPure, Category = "Phase")
	ETubeTurretState GetState() const;

	UFUNCTION(BlueprintCallable, Category = "Phase")
	void SetState(ETubeTurretState State);

	/** Blueprint Event to change the Static Mesh for the new phase */
	UFUNCTION(BlueprintImplementableEvent, Category = "Phase")
	void StateChanged(ETubeTurretState State);

protected:
	/** Handle the new state */
	void HandleNewState();

	/** Current State of the Tube */
	ETubeTurretState TurretState;

	/** Mark for death, Keep the collision enabled. */
	void Dead();

public:
	/** Initialize the Actor for Phase 2 */
	void BeginPhase2();

	UPROPERTY(EditAnywhere, Category = "Phase")
	TArray<FVector> FireOffsetPhase02;

protected:
	/** Spawn the Phase 2 Weapons */
	void SpawnWeaponsPhase2();

	/** References to the Phase 2 Weapons */
	UPROPERTY()
	TArray<class AWeapon*> WeaponsPhase2;
	
	/** Spawn the Weak Point Actor */
	void SpawnWeakPoint();

	FTimerHandle WeakPoint_TimerHandle;

public:
	/** Initialize the Dead State */
	void BeginDeadState();

	/** Clean up and Destroy */
	void CleanDestroy();

private:
	/** Swap the current mesh with the mesh at index */
	void SwapStaticMesh(int32 Index);

	static const int32 MeshPhase01;
	static const int32 MeshPhase02;
	static const int32 MeshPhase03;

public:
	UFUNCTION(BlueprintCallable, Category = "Properties")
	void SetAttackTypeDataAsset(class UAttackTypeDataAsset* InType);

protected:
	virtual void SetDMIColor() override;

	float DestroyComponentDelay;
	FTimerHandle DestroyComponent_TimerHandle;

	UFUNCTION()
	void DestroyDestructionComponent();
};
