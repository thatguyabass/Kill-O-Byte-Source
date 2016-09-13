// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"

#include "GameFramework/Character.h"
#include "SnakeLink.generated.h"

/**
* SnakeLink is based off ACharacter 
*
* SnakeLinks are the base class for all players that play the game
* SnakeLinks can be in 2 stages:
*	Head - This is the possesed character and the one the player controls, it
*		Is also responcible for activating the weapons and powerups 
*	Body - These links follow the target (The link ahead of them) around the level.
*		Link movement is scaled based on the distance between them and the target
*
* Head acts as the central control for the attached bodies.
* 
*/

UENUM(BlueprintType)
enum class ESnakeState : uint8
{
	Head,
	Body
};

UENUM(BlueprintType)
enum class EShapeName : uint8
{
	Wall,
	Focus,
	Tank,
	Shield,
	SuperSpeedGuy,
	Snake
};

UCLASS()
class SNAKE_PROJECT_API ASnakeLink : public ACharacter
{
	GENERATED_BODY()

public:
	ASnakeLink(const class FObjectInitializer& PCIP);

	/** New Unit Collision */
	UPROPERTY(VisibleDefaultsOnly, Category = "SnakeLink|Collision")
	class UBoxComponent* BoxComponent;

	/** Sphere Collider used to collect powerups and Frozen Links, Set in ASnakeLinkHead Class */
	UPROPERTY(VisibleDefaultsOnly, Category = "SnakeLink|Collision")
	class USphereComponent* CollectionSphere;

	UPROPERTY(VisibleDefaultsOnly, Category = "SnakeLink|Component")
	UDestructibleComponent* DestructibleComponent;
	//UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "SnakeLink|Component")
	UParticleSystemComponent* SpawnEffectVFX;

	/** the ideal distance the link is away from its target */
	UPROPERTY(EditAnywhere, Category = "SnakeLink|Property")
	float IdealDistance;

	/** Distance before the Links are Cut from the snake */
	UPROPERTY(EditAnywhere, Category = "SnakeLink|Property")
	float MaxDistance;

	UPROPERTY(EditAnywhere, Category = "SnakeLink|Property")
	float DestroyTime;

	UFUNCTION(BlueprintPure, Category = "SnakeLink|Helper")
	ASnakeLink* GetHead();

	/** Return teh Head as ASnakeLinkHead */
	UFUNCTION(BlueprintPure, Category = "SnakeLink|Helper")
	class ASnakeLinkHead* GetHeadCast();

public:
	UPROPERTY(EditAnywhere, Category = "SnakeLink|MaterialInstance")
	FName PrimaryParamName;

	UPROPERTY(EditAnywhere, Category = "SnakeLink|MaterialInstance")
	FName SecondaryParamName;

	/** Update this body to the Correct Teams Color */
	void UpdateColors(const FTeamColorData& ColorData);

	/** Return the Team number from the snake controller */
	virtual int32 GetTeamNumber();

	virtual FAttackType GetAttackType();

public:
	UPROPERTY(EditAnywhere, Category = "SnakeLink|MaterialInstance")
	FName AlphaParamName;

	UPROPERTY(EditAnywhere, Category = "SnakeLink|MaterialInstance")
	UCurveFloat* AlphaCurve;

	/** Start the Material Fade Timeline. */
	virtual void StartMaterialFlicker();

	/** Stop the Material Fade Timeline and restore the Alpha to 1.0f */
	virtual void EndMaterialFlicker();

	/** Set the alpha on the material to the InValue */
	void SetMaterialAlpha(float InValue);

protected:
	/** Material Instance for this Link */
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DMI;

	FTimeline AlphaTimeline;

public:
	/** Play the Spawn Effect and destroy this character */
	virtual void DespawnAndDestroy();

	/** Prepare this character for destruction. */
	void HideAndDestroy();

	/** Clean up the Character before Destroy */
	void CleanupCharacter();

	/** Take Damage Override. This calls Reduce Health */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	/** [Server] Reduce the Health of this Link */
	UFUNCTION(BlueprintCallable, Category = "Link|Health")
	virtual void ReduceHealth(int32 Value, ASnakePlayerState* Damager);

	/** [Server] Increase the Current Health */
	UFUNCTION(BlueprintCallable, Category = "Link|Health")
	virtual void AddHealth(int32 Value);

	/** [Server] Set the Current Health */
	UFUNCTION(BlueprintCallable, Category = "Link|Health")
	virtual void SetHealth(int32 Value);

	/** Get the Current Health */
	UFUNCTION(BlueprintCallable, Category = "Link|Health")
	virtual int32 GetHealth() const;

	/** Get the Max Health of the Character */
	UFUNCTION(BlueprintCallable, Category = "Link|Health")
	virtual int32 GetTotalHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Link|Health")
	virtual bool IsDead() const;

protected:
	virtual void CleanUpBody();

public:
	/** Fire a basic projectile */
	UFUNCTION(BlueprintCallable, Category = SnakeLink)
	virtual void FirePrimary();

	UFUNCTION()
	virtual bool CanFire();

	virtual bool GetOverrideCanFire() const;

	/** Override out primary weapon's CanFire */
	virtual void OverrideCanFire();

	/** Spawn a New Weapon. This will override the Default Weapon.
	*	@Param Duration - How long the weapon will remain equiped before resting to the default
	*	@ Calls the head to change the weapons for each Link Including the Head
	*/
	void ChangeWeapon(TSubclassOf<class AWeapon> InWeaponClass/*, float Duration*/);

	/** Change the weapon of this Link 
	*	@ ProjetileOwner - ProjectileOwner should equal the Actor that controls the character to ensure the 
	*	projectiles don't collide with other projectiles from the same character
	*/
	virtual void ChangePrimary(TSubclassOf<class AWeapon> InWeaponClass, /*float Duration,*/AActor* ProjectileOwner = nullptr);

	/** Ignore projectiles that have been spawned by this Character */
	virtual void IgnoreProjectiles(class ABaseProjectile* InProjectile);

	/** Reset the Primary Weapon to the base default weapon */
	virtual void ResetPrimaryWeapon(TSubclassOf<class AWeapon> InPrimaryWeaponClass);

	/** Check if a lazer is being fired from any Link Body */
	UFUNCTION(BlueprintCallable, Category = "SnakeLink|Weapons")
	virtual bool LazerBeingFired();

	virtual void TerminateProjectile();

	/** Start the Destruction Sequence */
	virtual void CleanUpWeapons();

	/** Get the Current Primary Weapon */
	class AWeapon* GetPrimaryWeapon() const;

protected:
	UPROPERTY()
	class AWeapon* PrimaryWeapon;

	/** Set the Primary Weapon */
	void SetPrimary(AWeapon* NewPrimaryWeapon, AActor* ProjectileOwner);

	/** Vector offset from the Weapon. */
	UPROPERTY(EditAnywhere, Category = "SnakeLink|Weapon")
	FVector FireOffset;

public:
	/** Global Speed Modifier */
	UPROPERTY(BlueprintReadWrite, Category = "SnakeLink|Debug")
	float GlobalSpeedModifier;

private:
	/** The base movement speed. Used to return to the Initial Speed after Modifiers are applied */
	UPROPERTY(Replicated)
	float BaseMovementSpeed;

public:
	/** Final Movement speed after applying the movement modifiers */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "SnakeLink|Movement")
	float MovementSpeed;

	/** If true, input will be ignored */
	UPROPERTY(BlueprintReadWrite, Category = "SnakeLink|Movement")
	bool bSuppressInput;

	/** Apply a Slow Effect to the Snake */
	UFUNCTION()
	virtual void ApplySlow(FSpeedPower Slow);

	UFUNCTION(BlueprintCallable, Category = "SnakeLink|Movement")
	void AdjustMovementSpeed();

	UFUNCTION()
	virtual void ApplySpeed(FSpeedPower Speed);

	/** Get the Speed Modifier. 1 Based */
	UFUNCTION()
	virtual float GetSpeedModifier() const;

	UFUNCTION()
	virtual float GetSlowModifier() const;

	FVector GetMovementVelocity();

public:
	UFUNCTION(BlueprintCallable, Category = "SnakeLink|Method")
	void SetTarget(ASnakeLink* Link);

	UFUNCTION(BlueprintPure, Category = "SnakeLink|Method")
	ASnakeLink* GetTarget();

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "SnakeLink|Method")
	void StateChange(ESnakeState State);

	UFUNCTION(BlueprintCallable, Category = "SnakeLink|Method")
	ESnakeState GetCurrentState();

	UFUNCTION(BlueprintCallable, Category = "SnakeLink|Method")
	void SetCurrentState(ESnakeState State);

	UFUNCTION(BlueprintCallable, Category = "SnakeLink|Method")
	bool IsHead();
	
	UFUNCTION()
	bool IsHeadConst() const;

	UFUNCTION(BlueprintCallable, Category = "SnakeLink|Method")
	bool IsBody();

protected:
	/* Current State of this Link
	* Head - The Character that has movement input and the centeral control
	* Body - Attached to the Head, follows the body targets of the head
	*/
	ESnakeState CurrentState;

public:
	/** Compare the Current shape's name to the passed in value */
	UFUNCTION(BlueprintCallable, Category = "SnakeLink|Method")
	virtual bool IsShapeName(EShapeName InName);

public:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComponent, class AActor* other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;

public:
	//This Links current Target
	UPROPERTY(Transient, BlueprintReadOnly, Replicated)
	ASnakeLink* Target;

	UPROPERTY()
	int32 BodyIndex;

	UPROPERTY()
	class ASnakeLinkHead* LinkBrain;

protected:
	//Follow Target. Only valid during snake mode 
	void FollowTarget();

	/** Follow A Body Target on the Head */
	void FollowComponent();

protected:
	/** Execute after the state has changed */
	void HandleNewState();

	/** Line Trace to the Link brain to see if any static objects are in the way. */
	UFUNCTION()
	void LineTraceToHead();

	UPROPERTY(EditAnywhere, Category = "SnakeLink|TraceToHead")
	TArray<TEnumAsByte<ECollisionChannel>> LineTraceToHeadObjectChannels;

protected:
	/** Was thier an obstical between us and the head last frame? */
	UPROPERTY()
	bool bLastTraceCheck;

	/** Move this link to the Heads Position. Fixes displacement problems */
	void TeleportToHead();

	/** Teleport Timer Handle*/
	FTimerHandle Teleport_TimerHandle;

	FTimerHandle WeaponReset_TimerHandle;

	/** Clear the Handle Reference */
	void ClearTimer(FTimerHandle& Handle);
	
	virtual void PostNetReceiveLocationAndRotation() override;
	
protected:
	APlayerController* GetController();

public:
	virtual class ASnakePlayer* GetSnakePlayer() const;

	/** Play the Audio component in SnakeLinkHead */
	virtual void PlayAudio(int32 AudioCompIndex);

	/** Play the Audio component in SnakeLinkHead using BaseSound */
	virtual void PlayAudio(int32 AudioCompIndex, USoundBase* BaseSound);

	/** Stop the Audio Component in SnakeLinkHead */
	virtual void StopAudio(int32 AudioCompIndex);

	/** if true, play the weapon fire sound cue */
	virtual bool CanPlayWeaponAudio() const;

	/** Index of the Audio Component to reference in SnakeLinkHead */
	static const int32 WeaponFireAudioIndex;
	static const int32 ShapeAudioIndex;
	static const int32 NullAudioIndex;
	static const int32 OneOffAudioIndex;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* SpawnSFX;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* HitSFX;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* DeathSFX;

public:
	UPROPERTY(EditAnywhere, Category = "Material|Dissolve")
	FName DissolveColorName;

	/** Dissolve Blend Param Name */
	UPROPERTY(EditAnywhere, Category = "Material|Dissolve")
	FName DissolveBlendName;

	/** Dissolve Outline Param Name */
	UPROPERTY(EditAnywhere, Category = "Material|Dissolve")
	FName DissolveOutlineWidthName;

	UPROPERTY(EditAnywhere, Category = "Material|Dissolve")
	float DissolveOutlineWidth;

	/** Total time for the duration to complete from start to end */
	UPROPERTY(EditAnywhere, Category = "Material|Dissolve")
	float DissolveDuration;

	/** Delay before the dissolve takes place */
	UPROPERTY(EditAnywhere, Category = "Material|Dissolve")
	float DissolveStartDelay;

	/** Start Dissolving the Character */
	UFUNCTION()
	void StartDissolve();

	UFUNCTION()
	void SetDissolveBlend();

protected:
	/** if true, Dissolve is in progress */
	bool bDissolveInprogress;

	/** Total time expired of the dissolve process */
	float DissolveProgress;

	FTimerHandle DissolveDelay_TimerHandle;

public:
	/** Set the variables for Spawn effect
	*	False = Spawn In
	*	True = Spawn Out */
	UFUNCTION()
	virtual void InitializeSpawnEffect(bool InDirection);

	/** Activate the Spawn Particle effect and initialize the variables
	*	False = Spawn In 
	*	True = Spawn Out */
	UFUNCTION()
	virtual void StartSpawnEffect(bool InDirection);

	/** Set the Spawn Effect Blend Value for each DMI */
	UFUNCTION()
	void SetSpawnEffectBlend(float Blend);

	/** Update the Particle effect location and the current spawn effect blend */
	UFUNCTION()
	void SpawnEffectTick();

	/** Clean up and reset the particle effect and variables */
	UFUNCTION()
	void FinishSpawnEffect();

protected:
	/** If false, Spawn effect will go from top to bottom (Character appears)
	*	If True, Spawn Effect will go from bottom to top (Character Dissappears) */
	UPROPERTY()
	bool bSpawnEffectDirection;

	/** If true, the Spawn Effext VFX will play on begin play. Else, it will not play at all. */
	UPROPERTY(EditAnywhere, Category = "Material|Spawn")
	bool bPlaySpawnVFX;

	/** Actor Half Height Param Name. */
	UPROPERTY(EditAnywhere, Category = "Material|Spawn")
	FName ActorHalfHeightName;

	/** Spawn Blend Param Name */
	UPROPERTY(EditAnywhere, Category = "Material|Spawn")
	FName SpawnBlendName;

	/** Spawn Origin Offset Param Name */
	UPROPERTY(EditAnywhere, Category = "Material|Spawn")
	FName SpawnOriginOffsetName;

	UPROPERTY(EditAnywhere, Category = "Material|Spawn")
	FName SpawnEffectOutlineWidthName;

	/** Duration of the Spawn Effect */
	UPROPERTY(EditAnywhere, Category = "Material|Spawn")
	float SpawnEffectDuration;

	/** Spawn Offset from the parent actor of the static mesh */
	UPROPERTY(EditAnywhere, Category = "Material|Spawn")
	float ActorHalfHeight;

	/** Spawn offset for the Spawn effect. This will normally equal the Opposite Z Value of the static meshes relative location
	*	Example: 1Z Location = -1 Offset */
	UPROPERTY(EditAnywhere, Category = "Material|Spawn")
	float SpawnOffset;

	/** Spawn Effect Outline */
	UPROPERTY(EditAnywhere, Category = "Material|Spawn")
	float SpawnEffectOutlineWidth;

	/** if true, Spawn Effect in progress */
	bool bSpawnEffectInprogress;

	/** Spawn Effect Progress */
	float SpawnEffectProgress;

	/** Spawn Effect VFX initial Relative Location */
	FVector SpawnEffectInitialRelativeLocation;

};