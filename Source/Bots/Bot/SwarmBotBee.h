// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SwarmBotBee.generated.h"

UCLASS()
class SNAKE_PROJECT_API ASwarmBotBee : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASwarmBotBee();

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UDestructibleComponent* DestructibleComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UAudioComponent* DeathSFXComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UAudioComponent* FireWeaponSFXComponent;

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Properties")
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Properties")
	FVector FireOffset;

	UPROPERTY(EditAnywhere, Category = "Properties")
	FVector2D FireRateRange;

	UPROPERTY(EditAnywhere, Category = "Properties")
	float DestroyTime;

	virtual void Tick(float DeltaTime) override;

	/** Fire the Weapon */
	virtual void FireWeapon();

	/** Spawn the waepon */
	virtual void SpawnWeapon();	

	void HideAndDestroy();
	
	UFUNCTION(BlueprintCallable, Category = "Method")
	void SetBotOwner(class AActor* InOwner, int32 Index);

	class AWeapon* GetWeapon();

	/** Set the AttackType for the DMI */
	UFUNCTION(BlueprintCallable, Category = "Method")
	void SetAttackType(struct FAttackType NewAttackType);

	struct FAttackType GetAttackType() const;

	/** Create and Set a DMI for this actor */
	UFUNCTION(BlueprintImplementableEvent, Category = "Event")
	void CreateAndSetDMI();

	/** Apply the Attack Type colors to the DMI. */
	UFUNCTION(BlueprintCallable, Category = "Material")
	virtual void SetDMIColor();

	/** Primary Color Material Parameter Name */
	UPROPERTY(EditAnywhere, Category = "Material")
	FName PrimaryColorName;

	/** Secondary Color Material Parameter Name */
	UPROPERTY(EditAnywhere, Category = "Material")
	FName SecondaryColorName;

	/** Fracture Color Material Parameter Name */
	UPROPERTY(EditAnywhere, Category = "Material")
	FName FractureColorName;

	/** Dissolve Blend Param Name */
	UPROPERTY(EditAnywhere, Category = "Bot|Material|Dissolve")
	FName DissolveBlendName;

	/** Dissolve Outline Param Name */
	UPROPERTY(EditAnywhere, Category = "Bot|Material|Dissolve")
	FName DissolveOutlineWidthName;

	UPROPERTY(EditAnywhere, Category = "Bot|Material|Dissolve")
	float DissolveOutlineWidth;

	/** Total time for the duration to complete from start to end */
	UPROPERTY(EditAnywhere, Category = "Bot|Material|Dissolve")
	float DissolveDuration;

	/** Delay before the dissolve takes place */
	UPROPERTY(EditAnywhere, Category = "Bot|Material|Dissolve")
	float DissolveStartDelay;

protected:
	/** if true, Dissolve is in progress */
	bool bDissolveInprogress;

	/** Total time expired of the dissolve process */
	float DissolveProgress;

	FTimerHandle DissolveDelay_TimerHandle;

	/** Start Dissolving the bot */
	UFUNCTION()
	void StartDissolve();

	UFUNCTION()
	void SetDissolveBlend();

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Material")
	TArray<UMaterialInstanceDynamic*> DMI;

	/** Weapon this Actor will fire when directed by the BotOwner */
	UPROPERTY()
	class AWeapon* Weapon;

	/** Index of this bot in the Hives array */
	int32 BotIndex;

	/** The Bot class actor that owns this actor. */
	UPROPERTY()
	class AActor* BotOwner;

	/** Attack type of this actor */
	UPROPERTY()
	struct FAttackType AttackType;

	FTimerHandle SpawnDelay_TimerHandle;

public:
	/** Initialize the Spawn VFX variables */
	UFUNCTION()
	void StartSpawnEffect(float ActorHalfHeight, float Offset, float OutlineWidth);

	/** Set the Blend Using the Swarm Bot's values */
	void SetSpawnEffectBlend(float InBlend = 0.0f);

	UPROPERTY(EditAnywhere, Category = "Material")
	FName ActorHalfHeightName;

	UPROPERTY(EditAnywhere, Category = "Material")
	FName SpawnBlendName;

	UPROPERTY(EditAnywhere, Category = "Material")
	FName SpawnOriginOffsetName;

	UPROPERTY(EditAnywhere, Category = "Material")
	FName SpawnEffectOutlineWidthName;

public:
	UFUNCTION()
	void SetFireSoundCue();

	UFUNCTION()
	void PlayFireWeaponSFX();

protected:
	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<class USoundCue*> WeaponSFX;

};
