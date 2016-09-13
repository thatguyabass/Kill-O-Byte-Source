// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/POIGroup.h"
#include "Bots/BotStructs.h"
#include "GameMode/BaseGameMode.h"
#include "GameFramework/Character.h"
#include "Utility/DataTables/StatData.h"
#include "Bot.generated.h"

UCLASS()
class SNAKE_PROJECT_API ABot : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABot();

	UPROPERTY(EditAnywhere, Category = "Bot|Behavior")
	class UBehaviorTree* BotBehaviorTree;

	UPROPERTY(BlueprintReadOnly, Category = "Bot|Arcade Points")
	class AFloatyTextManager* FloatyText;

	UPROPERTY(VisibleDefaultsOnly, Category = "Bot|Component")
	UDestructibleComponent* DestructibleComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Bot|Component")
	UParticleSystemComponent* SpawnEffectVFX;

	/** Audio component to play death SFX. */
	UPROPERTY(VisibleDefaultsOnly, Category = "Bot|Component")
	UAudioComponent* DeathSFXComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Bot|Component")
	UAudioComponent* FireWeaponSFXComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Bot|Component")
	UAudioComponent* SpawnSFXComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Bot|Component")
	UAudioComponent* AmbientSFXComponent;

	UPROPERTY(EditAnywhere, Category = "Bot|Behavior")
	int32 TeamNumber;

	UPROPERTY(EditAnywhere, Category = "Bot|Behavior")
	float SearchRadius;

	UPROPERTY(EditAnywhere, Category = "Behavior|Weapon")
	FVector FireOffset;

	UPROPERTY(EditAnywhere, Category = "Behavior|Weapon")
	TSubclassOf<class AWeapon> WeaponClass;

	/** If true, this bot will not take any damage from any source */
	UPROPERTY(EditAnywhere, Category = "Bot|Attributes")
	bool bInvulnerable;

	/** If true, Invulnerable will be ignored. This will only last for the called frame and will be reset once ReduceHealth has been called */
	UPROPERTY()
	bool bOverrideInvulnerable;

	/** The Max Health this Bot will have */
	UPROPERTY()
	int32 BaseHealth;

	void SetHealth(int32 InValue);

	UFUNCTION(BlueprintPure, Category = "Bot|Attributes")
	virtual int32 GetMaxHealth() const;

	UFUNCTION(BlueprintPure, Category = "Bot|Attributes")
	virtual int32 GetCurrentHealth() const;

	UPROPERTY(EditAnywhere, Category = "Bot|Arcade Points")
	float Points;

	UPROPERTY(EditAnywhere, Category = "Bot|Attributes")
	EBotType BotType;

	UPROPERTY(EditAnywhere, Category = "Bot|Attributes")
	EBotClass BotClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bot|Attributes")
	class UAttackTypeDataAsset* AttackTypeDataAsset;

	UFUNCTION(BlueprintPure, Category = "Bot|Attributes")
	virtual FAttackType GetAttackType();

	/** Spawn the Weapons */
	virtual void PostInitializeComponents() override;
	
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/** Fire the Primary Weapon. Called form the AI controller */
	UFUNCTION(BlueprintCallable, Category = "Bot|Control")
	virtual void FireWeapon();

	/** Override the base TakeDamage Method */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, class AActor* DamageCauser) override;

	/** Target the Instigator */
	virtual void TargetInstigator(AController* Instigator);

	/** Spawn the Weapon Object. */
	virtual void SpawnWeapon();

	UFUNCTION(BlueprintPure, Category = "Weapon")
	class AWeapon* GetWeapon();

	/** Called right after the weapon has been spawned. */
	virtual void PostSpawnWeapon();

	/** Trigger Dead and Call Hide And Destroy */
	UFUNCTION(BlueprintCallable, Category = "Bot|Destroy")
	void DestroyBot();

	/** Hide the actor before destroying it */
	virtual void HideAndDestroy();

	/** Time this actor will exist in the level after Hide And Destroy is called */
	UPROPERTY(EditAnywhere, Category = "Bot|Attributes")
	float DestroyTime;

	/** Blueprint Event triggered by Hide And Destroy */
	UFUNCTION(BlueprintNativeEvent, Category = "Bot|Event")
	void OnHideAndDestroy(float InLifeSpan);
	virtual void OnHideAndDestroy_Implementation(float InLifeSpan);

protected:
	/** Primary Weapon */
	UPROPERTY()
	class AWeapon* Weapon;

	/** Current Health */
	int32 Health;

public:
	/** Maximum the Movement speed can be reduced by */
	UPROPERTY(EditAnywhere, Category = "Bot|Slow Properties")
	float MaxSlowModifier;

	/** Slow down the Bots movement speed for a duration */
	virtual void ApplySlow(FSpeedPower Slow);

	/** Check to see if the bot is currently being slowed. If true, the bot is slowed */
	bool IsSlowed() const;

protected:
	/** Currently applied slow effects */
	UPROPERTY()
	TArray<FSpeedPower> SlowModifiers;

	static const float BaseSlowModifier;

	/** The initial Unmodified Movement speed */
	float BaseMovementSpeed;

	/** Get the Current Slow Modifier Total 
	*	@Param Raw - Value unmodified
	*/
	float GetSlowModifier(bool Raw = false);

	/** Has the Max Slow Modifier been reached? */
	bool CanApplySlow();

	/** Modifiy the Character Movement component after modifying a movement value */
	virtual void ApplyMovementChange();

public:
	/** if true, This bot is Dead! */
	UFUNCTION(BlueprintPure, Category = "Bot|Status")
	bool IsDead();

	UFUNCTION(BlueprintImplementableEvent, Category = "Bot|Status")
	void OnBotDead();

protected:
	/** Track if this actor is dead */
	bool bDead;

	/** If true, this will bypass the Calculate Health Method and deal the current damage to the target */
	bool bForceDamageOverride;

	/** Reduce the health of the Bot */
	virtual void ReduceHealth(int32 Damage, class ASnakePlayerState* DamageCauser);

	/** Add the Poitns to the Killer and destroy the bot */
	virtual void Dead(class ASnakePlayerState* Killer);

	/** base Game Mode Getter */
	ABaseGameMode* GetGameMode();

	/** Find the Floaty Text Manager in the world */
	class AFloatyTextManager* GetFloatyText();

public:
	/** [Server] Reduce the health of this bot */
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerReduceHealth(int32 Damage, class ASnakePlayerState* DamageCauser);
	virtual void ServerReduceHealth_Implementation(int32 Damage, class ASnakePlayerState* DamageCauser);
	virtual bool ServerReduceHealth_Validate(int32 Damage, class ASnakePlayerState* DamageCauser) { return true; }

	/** [Server + Client] Remove collision from this actor */
	UFUNCTION(Reliable, NetMultiCast, WithValidation)
	void MultiCastRemoveCollision();
	virtual void MultiCastRemoveCollision_Implementation();
	virtual bool MultiCastRemoveCollision_Validate() { return true; }

public:
	UPROPERTY(EditAnywhere, Category = "Bot|Material")
	FName PrimaryColorName = "Primary Color";

	UPROPERTY(EditAnywhere, Category = "Bot|Material")
	FName SecondaryColorName = "Secondary Color";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bot|Material")
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

	/** If true, the Bot will enter Spawn VFX debug mode */
	UPROPERTY(EditAnywhere, Category = "Bot|Material|Spawn")
	bool bSpawnEffectDebug;

	/** If true, the Spawn Effext VFX will play on begin play. Else, it will not play at all. */
	UPROPERTY(EditAnywhere, Category = "Bot|Material|Spawn")
	bool bPlaySpawnVFX;

	/** Actor Half Height Param Name. */
	UPROPERTY(EditAnywhere, Category = "Bot|Material|Spawn")
	FName ActorHalfHeightName;

	/** Spawn Blend Param Name */
	UPROPERTY(EditAnywhere, Category = "Bot|Material|Spawn")
	FName SpawnBlendName;

	/** Spawn Origin Offset Param Name */
	UPROPERTY(EditAnywhere, Category = "Bot|Material|Spawn")
	FName SpawnOriginOffsetName;

	UPROPERTY(EditAnywhere, Category = "Bot|Material|Spawn")
	FName SpawnEffectOutlineWidthName;

	/** Duration of the Spawn Effect */
	UPROPERTY(EditAnywhere, Category = "Bot|Material|Spawn")
	float SpawnEffectDuration;

	/** Spawn Offset from the parent actor of the static mesh */
	UPROPERTY(EditAnywhere, Category = "Bot|Material|Spawn")
	float ActorHalfHeight;

	/** Spawn offset for the Spawn effect. This will normally equal the Opposite Z Value of the static meshes relative location
	*	Example: 1Z Location = -1 Offset */
	UPROPERTY(EditAnywhere, Category = "Bot|Material|Spawn")
	float SpawnOffset;

	/** Spawn Effect Outline */
	UPROPERTY(EditAnywhere, Category = "Bot|Material|Spawn")
	float SpawnEffectOutlineWidth;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Bot|Material")
	TArray<UMaterialInstanceDynamic*> DMI;

	/** if true, Dissolve is in progress */
	bool bDissolveInprogress;

	/** Total time expired of the dissolve process */
	float DissolveProgress;

	FTimerHandle DissolveDelay_TimerHandle;

	/** if true, Spawn Effect in progress */
	bool bSpawnEffectInprogress;

	/** Spawn Effect Progress */
	float SpawnEffectProgress;

	/** Spawn Effect VFX initial Relative Location */
	FVector SpawnEffectInitialRelativeLocation;

	/** Activate the Spawn Particle effect and initialize the variables */
	UFUNCTION()
	virtual void StartSpawnEffect();

	/** Set the Spawn Effect Blend Value for each DMI */
	UFUNCTION()
	virtual void SetSpawnEffectBlend(float Blend = 0.0f);

	/** Update the Particle effect location and the current spawn effect blend */
	UFUNCTION()
	void SpawnEffectTick();

	/** Clean up and reset the particle effect and variables */
	UFUNCTION()
	void FinishSpawnEffect();

	UFUNCTION(BlueprintCallable, Category = "Bot|Material")
	virtual void SetDMIColor();

	/** Start Dissolving the bot */
	UFUNCTION()
	void StartDissolve();

	UFUNCTION()
	void SetDissolveBlend();

public:
	/** Particle's to spawn during death */
	UPROPERTY(EditAnywhere, Category = "Bot|DeathVFX")
	UParticleSystem* DeathVFX;

	UFUNCTION(BlueprintCallable, Category = "Bot|DeathVFX")
	virtual void SpawnDeathVFX();

public:
	UFUNCTION()
	void SetPointOfInterestGroup(struct FPointOfInterestGroup& Group);

	/** Sort through the world actor iterator returning all PointOfInterest Actors */
	void GetPointsOfInterest(TArray<class APointOfInterest*>& Out);

public:
	/** Object Container that holds actors that can be dropped upon death */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bot|Loot")
	class ULootTable* LootTable;

protected:
	/** Check if anything can be dropped and spawn. Never a garentree anything will spawn */
	void SpawnLoot();

protected:
	UPROPERTY(EditAnywhere, Category = "Bot|PointOfInterestGroup")
	struct FPointOfInterestGroup PointOfInterestGroup;

public:
	UPROPERTY(EditAnywhere, Category = "Bot|Stats")
	FName DataTableRowName;

protected:
	static const FString DataTableContext;

	UPROPERTY()
	UDataTable* StatDataTable;

public:
	/** Flag the Fire SFX. if true, play the Weapon Fire SFX */
	UPROPERTY(EditAnywhere, Category = "Bot|Audio")
	bool bPlayFireSFX;

	UPROPERTY(EditAnywhere, Category = "Bot|Audio")
	bool bPlaySpawnSFX;

	/** Play the Current SFX Audio */
	UFUNCTION(BlueprintCallable, Category = "Bot|Audio")
	virtual void PlayFireWeaponSFX();

public:
	UFUNCTION(Exec)
	void ShowBotDamage();

protected:
	bool bShowBotDamage;

	/** Start the Ambient SFX */
	void StartAmbientSFX();

	FTimerHandle Ambient_TimerHandle;

};
