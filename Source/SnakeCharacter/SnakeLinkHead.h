// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SnakeCharacter/SnakeLink.h"
#include "SnakeLinkHead.generated.h"

/**
 * 
 */
USTRUCT()
struct FShapeMode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Shape")
	EShapeName Name;

	/** Relative Locations of the Body Targets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shape")
	TArray<FTransform> RelativeLocations;

	/** Player has full control of the character, if false, the character constantly moves forward */
	UPROPERTY(EditAnywhere, Category = "Shape")
	bool bFreeMovement;

	/** If true, the Shapes will try to maintain thier location based on the body targets.
		If False, the Bodies will follow the target that is specified during thier construction */
	UPROPERTY(EditAnywhere, Category = "Shape")
	bool bRidgidShape;

	/** Specify if the Head Can shoot in this Shape */
	UPROPERTY(EditAnywhere, Category = "Shape")
	bool bHeadCanShoot;

	/** Specify which bodies can shoot || Order = 1 2 3 4 */
	UPROPERTY(EditAnywhere, Category = "Shape")
	TArray<bool> bCanShoot;

	/** Override the Primary Weapon with the Secondary Weapon Class */
	UPROPERTY(EditAnywhere, Category = "Shape")
	bool bUseSecondaryFire;

	UPROPERTY(EditAnywhere, Category = "Shape")
	TSubclassOf<class AWeapon> SecondaryWeaponClass;

	/** If false, the Character will not follow the target while shooting */
	UPROPERTY(EditAnywhere, Category = "Shape")
	bool bFollowCursorAndShoot;

	/** Increase OR decrease the speed when in the new shape.
	*	Becasuse the character speed is based on 1, All positive values will increase the speed
	*	While all negitive values will decrease the speed
	*	Value is % based: IE. 0.5 = 50% faster
	*/
	UPROPERTY(EditAnywhere, Category = "Shape")
	float SpeedModifier;

	UPROPERTY(EditAnywhere, Category = "Shape")
	bool bUseShield;

	FShapeMode()
	{
		bFreeMovement = true;
		bRidgidShape = true;
		bHeadCanShoot = true;
		bUseSecondaryFire = false;
		bFollowCursorAndShoot = true;
		bUseShield = false;

		SpeedModifier = 0.0f;
	}
};

USTRUCT()
struct FReloadData
{
	GENERATED_BODY()

	UPROPERTY()
	float Progress;

	UPROPERTY()
	float FireRate;

	void Reset();

	FReloadData()
	{
		Progress = 0.0f;
		FireRate = 0.5f;
	}
};

UCLASS()
class SNAKE_PROJECT_API ASnakeLinkHead : public ASnakeLink
{
	GENERATED_BODY()
	
public:
	ASnakeLinkHead(const class FObjectInitializer& PCIP);

	/** Camera Boom, Set in ASnakeLinkHead Class */
	UPROPERTY(VisibleDefaultsOnly, Category = "SnakeLink|Camera")
	class USpringArmComponent* CameraBoom;

	/** Camera Component for this character,Set in ASnakeLinkHead Class */
	UPROPERTY(VisibleDefaultsOnly, Category = "SnakeLink|Camera")
	class UCameraComponent* CameraComp;

	/** Scene Component used for Rotation. Location set via code */
	UPROPERTY(VisibleDefaultsOnly, Category = "Target")
	USceneComponent* TargetCursor;

	UPROPERTY(VisibleDefaultsOnly, Category = "Audio")
	UAudioComponent* WeaponFireAudioComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Audio")
	UAudioComponent* ShapeAudioComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Audio")
	UAudioComponent* NullZoneAudioComponent;

	/** Audio Component to play non looping one off SFX. (Hit, Spawn, Death) */
	UPROPERTY(VisibleDefaultsOnly, Category = "Audio")
	UAudioComponent* OneOffAudioComponent;

	/** How fast the Actor Rotates towards the Target Cursor */
	UPROPERTY(EditAnywhere, Category = "Target Cursor Properties")
	float InterpRate;

	UPROPERTY(EditAnywhere, Category = "Head|Properties")
	float RotationRate;

	/** Targets the Body actors follow */
	UPROPERTY(VisibleDefaultsOnly, Category = "Body Targets")
	USceneComponent* BodyTargetComponent01;

	UPROPERTY(VisibleDefaultsOnly, Category = "Body Targets")
	USceneComponent* BodyTargetComponent02;

	UPROPERTY(VisibleDefaultsOnly, Category = "Body Targets")
	USceneComponent* BodyTargetComponent03;

	UPROPERTY(VisibleDefaultsOnly, Category = "Body Targets")
	USceneComponent* BodyTargetComponent04;

	/** Body class */
	UPROPERTY(EditAnywhere, Category = "Head|Body Properties")
	TSubclassOf<ASnakeLink> BodyClass;

	UPROPERTY(EditAnywhere, Category = "Head|Body Properties")
	TArray<FShapeMode> Shapes;

	/** Return value bRidgidShape From CurrentShape */
	bool IsRidgidShape() const;

	UFUNCTION(BlueprintPure, Category = "Head|Mode")
	int32 GetShapeIndex() const;

	virtual bool IsShapeName(EShapeName InName) override;

	bool IsShieldButtonHeld() const;

	/** Enable Speed Shape */
	UFUNCTION(BlueprintCallable, Category = "Head|Speed Shape")
	void EnterSpeedShape();

	/** Remove the Speed Shape */
	UFUNCTION(BlueprintCallable, Category = "Head|Speed Shape")
	void ExitSpeedShape();

	/** Prevent the player from being able to change shapes and increase the shot duration scale. */
	UFUNCTION(BlueprintCallable, Category = "Head|Null State")
	void EnterNullifyState();

	/** Return to normal operating state */
	UFUNCTION(BlueprintCallable, Category = "Head|Null State")
	void ExitNullifyState();

protected:
	/** The current shape the Character and bodies are using */
	UPROPERTY()
	FShapeMode CurrentShape;

	/** Store the current shape index for reference checking */
	int32 CurrentShapeIndex;

	/** Store the last shape so that we can switch back if needed */
	UPROPERTY()
	int32 LastShapeIndex;

	/** If true, Shapes can't be changed at all */
	UPROPERTY()
	bool bDisableShapeChanging;

	/** Track the button state for the shield. Controller Left Shoulder */
	UPROPERTY()
	bool bShieldButtonHeld;

	/** if true, the actor is in the nullified state (No shape changing and decreased fire rate)*/
	bool bNullifyState;

	UPROPERTY(EditAnywhere, Category = "Head|Null State")
	float NullifyDuration;

	float NullifyProgress;

	/** Change the Shape of the Character to one of the permade shapes */
	void ChangeShape(int32 NewShape);

	/** Index of the Wall Shape */
	static const int32 WallShapeIndex;

	/** Index of the Focus Shape */
	static const int32 FocusShapeIndex;

	/** Index of the Lazer Shape */
	static const int32 LazerShapeIndex;

	/** Index of the Shield Shape */
	static const int32 ShieldShapeIndex;

	/** Index of the Speed Shape */
	static const int32 SpeedShapeIndex;

	/** Index of the Snake Shape */
	static const int32 SnakeShapeIndex;

	/** Calls ChangeShape with a different Index */
	void SetShape01();
	void SetShape02();
	void SetShape03();
	void SetShape04();
	void SetShape05();
	void SetShape06();

	/** Set the bShieldButtonHeld to false as the button was released and is no longer being held */
	void ShieldButtonReleased();

public:
	/** If true, the character is using the focus shape */
	UFUNCTION(BlueprintPure, Category = "Head|Shape Helper")
	bool InFocusShape() const;

	/** If true, the character is using the lazer shape */
	UFUNCTION(BlueprintPure, Category = "Head|Shape Helper")
	bool InLazerShape() const;

	/** If true, the character is using the Shield Shape */
	UFUNCTION(BlueprintPure, Category = "Head|Shape Helper")
	bool InShieldShape() const;

	/** If true, the Character is using the Speed Shape */
	UFUNCTION(BlueprintPure, Category = "Head|Shape Helper")
	bool InSpeedShape() const;

public:
	/** Different Attack Types that the player can assume */
	UPROPERTY(EditAnywhere, Category = "Head|Body Properties")
	class UPlayerAttackTypeAsset* PlayerDataAsset;

	/** Current Attack Mode */
	EAttackMode GetAttackMode();

	/** Current Attack Type. Incudes both ColorData and Attack Mode */
	UFUNCTION(BlueprintPure, Category = "Head|Utility")
	virtual FAttackType GetAttackType() override;

	virtual int32 GetTeamNumber() override;

	/** Get the Current AttackType Index*/
	UFUNCTION(BlueprintPure, Category = "Head|Mode")
	int32 GetAttackTypeIndex() const;

protected:
	/** The current selected Attack Type */
	UPROPERTY()
	FAttackType CurrentAttackType;

	/** Attack type Index for reference checking */
	int32 AttackTypeIndex;

	void ChangeAttackType(int32 NewAttackType);

	/** Event Triggered when Attack Type Changes */
	UFUNCTION(BlueprintImplementableEvent, Category = "Head|AttackType")
	void OnAttackTypeChange();

	/** Calls ChangeAttackType with a different index */
	void SetAttackType01();
	void SetAttackType02();
	void SetAttackType03();
	void SetAttackType04();

public:
	/** Focus Unlocked Getter */
	UFUNCTION(BlueprintPure, Category = "Head|Unlocked Abilities")
	bool IsFocusUnlocked() const;

	/** Lazer Unlocked Getter */
	UFUNCTION(BlueprintPure, Category = "Head|Unlocked Abilities")
	bool IsLazerUnlocked() const;

	/** Speed Unlocked Getter */
	UFUNCTION(BlueprintPure, Category = "Head|Unlocked Abilities")
	bool IsSpeedUnlocked() const;

	/** Shield Unlocked Getter */
	UFUNCTION(BlueprintPure, Category = "Head|Unlocked Abilities")
	bool IsShieldUnlocked() const;

	/** Attack type 2 Unlocked Getter */
	UFUNCTION(BlueprintPure, Category = "Head|Unlocked Abilities")
	bool IsAttackType02Unlocked() const;

	/** Attack type 3 Unlocked Getter */
	UFUNCTION(BlueprintPure, Category = "Head|Unlocked Abilities")
	bool IsAttackType03Unlocked() const;

	/** Attack type 4 Unlocked Getter */
	UFUNCTION(BlueprintPure, Category = "Head|Unlocked Abilities")
	bool IsAttackType04Unlocked() const;

public:
	/** The maximum health the Character can have */
	UPROPERTY()
	int32 MaxHealth;

	/** [Server] Reduce the Health of this Link */
	UFUNCTION(BlueprintCallable, Category = "Head|Health")
	virtual void ReduceHealth(int32 Value, ASnakePlayerState* Damager) override;

	/** [Server] Increase the Current Health */
	UFUNCTION(BlueprintCallable, Category = "Head|Health")
	virtual void AddHealth(int32 Value) override;

	/** [Server] Set the Current Health */
	UFUNCTION(BlueprintCallable, Category = "Head|Health")
	virtual void SetHealth(int32 Value) override;

	/** Get the Current Health */
	UFUNCTION(BlueprintCallable, Category = "Head|Health")
	virtual int32 GetHealth() const override;

	/** Get the Max Health this character can have */
	UFUNCTION(BlueprintCallable, Category = "Head|Health")
	virtual int32 GetTotalHealth() const override;

	/** Dead Getter */
	virtual bool IsDead() const override;

	/** Death Event triggered when killed. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Head|Health")
	void OnDead();

	/** Damage the player regardless if they are invulnerable.
	*	This needs to be called before the damage is applied, because bDamageOverride is set false each tick. */
	UFUNCTION(BlueprintCallable, Category = "Head|Health")
	void OverrideInvulnerable();

	/** Immune to all damage. If true, */
	UFUNCTION(BlueprintPure, Category = "Head|Health")
	bool IsInvulnerable();

	/** Check if the Head is invulnerable and if it has been overridden */
	UFUNCTION(BlueprintPure, Category = "Head|Health")
	bool CanTakeDamage();

	virtual void StartMaterialFlicker() override;
	virtual void EndMaterialFlicker() override;

	virtual void DespawnAndDestroy() override;

protected:
	UPROPERTY()
	int32 Health;

	/** If true, damage will be applied even if invulnerable. This is set to false each tick */
	bool bDamageOverride;

	/** Is this Character Dead? */
	bool bDead;

	void Killed(ASnakePlayerState* Killer);

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	virtual void AddControllerYawInput(float Value) override;
	virtual void PossessedBy(class AController* InController) override;
	virtual void PostInitializeComponents() override;

	/** Depth of the Ground Orientation LineTrace */
	UPROPERTY(EditAnywhere, Category = "SnakeLink|LineTrace")
	float LineTraceDepth;

	/** Frequency of the Orientation LineTrace in Seconds */
	UPROPERTY(EditAnywhere, Category = "SnakeLink|LineTrace")
	float Frequency;

	/** Character Movement */
	void MoveForward(float Value);
	void MoveRight(float Value);

	/** return the component at Body Index */
	USceneComponent* GetBodyTarget(int32 BodyIndex);

protected:
	/** Orient the Player character to the Mouse Cursor OR the Thumbstick Direction. */
	UFUNCTION()
	void OrientToCursor();

	/** Minimum stick deflection to register input */
	UPROPERTY(EditAnywhere, Category = "SnakeLink|Orient")
	float MinThumbStickDeflection;

	UPROPERTY(EditAnywhere, Category = "SnakeLink|Orient")
	float LazerLerpBlend;

	/** Axix Key Names */
	static const FName RotateX;
	static const FName RotateY;

	/** Rotate the Character to the Orientation of the Ground */
	UFUNCTION()
	void OrientToGround();

	/** Progress till next trace */
	float TraceProgress;

	/** Slope Clamp */
	float MaxSlope;

	void Clamp(float& Axis);

	/** Easier to cycle through an array then the individual components */
	UPROPERTY()
	TArray<USceneComponent*> BodyTargetArray;

	UPROPERTY()
	TArray<ASnakeLink*> LinkBodies;

	/** Spawn the bodies that are attached to this head */
	void SpawnBodies();

	static const int32 MaxBodies;

public:
	/** The primary key was pressed. Set bSecondaryPressed flag true to track how long the button is held */
	UFUNCTION(BlueprintCallable, Category = "SnakeLink|Input")
	void SecondaryPressed();

	/** The Secondary Key Was released. Set bSecondaryPressed Flag false to signal the button is no longer being held. */
	UFUNCTION(BlueprintCallable, Category = "SnakeLink|Input")
	void SecondaryReleased();

	/** Is the Secondary Key being held? */
	UFUNCTION(BlueprintPure, Category = "SnakeLink|Input")
	bool SecondaryHeld() const;

protected:
	UPROPERTY()
	bool bSecondaryHeld;

	//Controller Only Binding.
	/** Controller Face Button Bottom */
	void ControllerFaceBottom();
	/** Controller Face Button Top */
	void ControllerFaceTop();
	/** Controller Face Button Right */
	void ControllerFaceRight();
	/** Controller Face Button Left */
	void ControllerFaceLeft();

	void ControllerDPadUp();

	void ControllerLeftShoulder();
	void ControllerRightShoulder();

public:
	/** Fire a basic projectile */
	UFUNCTION(BlueprintCallable, Category = SnakeLink)
	virtual void FirePrimary() override;

	/** Signal that the fire button has been released */
	void FireReleased();

	/** Fire the primary weapons using a thumbstick */
	void FirePrimaryThumbStick(float Delta);

	/** Destroy the Current Primary Weapon, and spawn new one */
	virtual void ResetPrimaryWeapon(TSubclassOf<class AWeapon> InPrimaryWeaponClass) override;

	/** Default Weapon Classes. Order is based on the Attack Type Array in the UPlayerAttackTypeAsset */
	UPROPERTY(EditAnywhere, Category = "SnakeLink|Weapons")
	TArray<TSubclassOf<class AWeapon>> PrimaryWeaponClasses;

	/** Cooldown duration after the lazer has been fired */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SnakeLink|Weapons")
	float LazerCooldown;

	/** Change the Primary Weapon for each body */
	virtual void ChangePrimary(TSubclassOf<class AWeapon> InWeapon,/* float Duration,*/ AActor* ProjectileOwner = nullptr) override;

	/** Spawn the Seconday Weapon. If this character doesn't have one set, it will default to the Default Weapon */
	void SpawnSecondary();

	/** Ignore the Projectiles that are spawned from this character */
	virtual void IgnoreProjectiles(class ABaseProjectile* InProjectile) override;

	/** Check if a lazer is being fired from any Link Body */
	virtual bool LazerBeingFired() override;

	/** Terminate the lazer projectile */
	virtual void TerminateProjectile() override;

	/** Start the Lazer Cooldown */
	void StartLazerCooldown();

	/** Finish the Cooldown */
	void FinishLazerCooldown();

	virtual void CleanUpWeapons() override;

	/** Get the Current Value of the Cooldown Time. If not on cooldown, retun total cooldown time */
	UFUNCTION(BlueprintPure, Category = "SnakeLink|Weapons")
	float GetLazerCooldown();

	/** Return the Lazer Cooldown state */
	UFUNCTION(BlueprintCallable, Category = "SnakeLink|Weapons")
	bool IsLazerOnCooldown();

	/** Can we fire one of the primary weapons? */
	virtual bool CanFire() override;

	virtual bool GetOverrideCanFire() const override;

	virtual void OverrideCanFire() override;

protected:
	/** Each weapon has a built in cooldown system which will tick once the projectiles are fired. However, due to some issues when firing multiple weapons togeather,
	*	they needed a single source. If true, this actor will calculate the weapons cooldown and fire when ready. if false, the weapons internal cooldown timer will be used. 
	*/
	UPROPERTY()
	bool bCanFireOverride;

	/** Flag to determine if the player can shoot */
	UPROPERTY()
	bool bCanFire;

	/** Weapon Shot cooldown. Each weapon has its own fire rate which is independent from the rest */
	UPROPERTY()
	TArray<FReloadData> ReloadData;

	/** Soft change the shot durations without hard changing the values */
	UPROPERTY()
	float ShotDurationScale;

	static const float BaseShotDuration;
	static const float SlowShotDuration;

	/** bCanFire Setter */
	void SetCanFire(bool InValue);

	/** Force bCanFire to true */
	void ResetCanFire();

	UPROPERTY()
	bool bFireHeld;

	/** Lazer Cooldown State */
	bool bLazerCooldownFlag;

	/** Recent Shape Switch */
	UPROPERTY()
	bool bShapeChanged;

	/** Shape Change Weapon Fire Delay */
	UPROPERTY()
	float ChangeFireDelay;

	UPROPERTY()
	float ChangeFireDelayProgress;

	//FTimerHandle CanFire_TimerHandle;
	FTimerHandle LazerCooldown_TimerHandle;

	/** Get the Weapon for the Current Attack Type*/
	TSubclassOf<AWeapon> GetAttackTypeWeapon();

public:
	/** Force Field Class */
	UPROPERTY(EditAnywhere, Category = "Head|Shield")
	TSubclassOf<class AForceField> ForceFieldClass;

	UFUNCTION(BlueprintPure, Category = "Head|Shield")
	class AForceField* GetForceField() const;

	/** Return the AttackType of the Force Field. Will return Default if ForceField Null */
	UFUNCTION()
	FAttackType GetForceFieldAttackType() const;

protected:
	/** Force Field Object */
	class AForceField* ForceField;

	/** Spawn the Force Field.*/
	void SpawnForceField();

	/** Toggle Force field activation based on the Current Shape */
	void ToggleForceField();

	/** If true, the force field has been shutdown and can't be used */
	bool IsShieldDown() const;

public:
	/** Maximum the camera can move from the base location */
	UPROPERTY(EditAnywhere, Category = "Camera Control")
	FVector MaxCameraDelta;

	void PowersTick(float DeltaTime);

	/** Max Speed Boost */
	UPROPERTY(EditAnywhere, Category = "Head|Speed")
	float MaxSpeedModifier;

	UFUNCTION(BlueprintCallable, Category = "Head|Speed")
	void SetSpeedBoostPower(FSpeedPower InSpeedBoost);

	virtual void ApplySpeed(FSpeedPower Speed) override;

	/** Override! Return speed modifier. 1 Based */
	virtual float GetSpeedModifier() const override;

protected:
	static const float BaseSpeedModifier;

	/** Speed boost power struct */
	struct FSpeedPower SpeedBoostPower;

	/** Speed Pickup Power Struct */
	struct FSpeedPower SpeedPickupPower;

	/** Apply any movement changes to each link */
	void ApplyMovementChanges();

public:
	UPROPERTY(EditAnywhere, Category = "Head|Speed")
	float MaxSlowModifier;

	UFUNCTION(BlueprintCallable, Category = "Head|Speed")
	virtual void ApplySlow(FSpeedPower SlowModifier) override;

	UFUNCTION(BlueprintCallable, Category = "Head|Speed")
	void AddStasisSlowModifier(FSpeedPower StatisModifier);

	UFUNCTION(BlueprintCallable, Category = "Head|Speed")
	void RemoveStasisSlowModifier();

	virtual float GetSlowModifier() const override;

protected:
	/** Array of all the slow modifiers currently effecting the character */
	UPROPERTY()
	TArray<FSpeedPower> SlowModifiers;

	static const float BaseSlowModifier;

	/** Statis Effect slow. Seperate from array to simpiler More control when its removed */
	FSpeedPower StasisSlow;

protected:
	UFUNCTION()
	virtual void CleanUpBody() override;

	UFUNCTION()
	void SetCameraRotation();

public:
	/** bUseController Getter. If true, the player is using a gamepade controller. */
	UFUNCTION(BlueprintPure, Category = "Control")
	bool IsUsingController() const;

	/** Aquire the Saved Data and Store it locally */
	void RetrieveControllerSettings();

	/** bTwinStickMode Getter. If true, the player can use the look stick as the primary weapon trigger */
	UFUNCTION(BlueprintPure, Category = "Control")
	bool GetTwinStickMode() const;

protected:
	/** If true, the primary player is using a gamepad controller. Note: Players 2-4 can ONLY use a controller */
	UPROPERTY()
	bool bUseController;

	/** If true, The look thumb stick will also shoot the primary weapon */
	UPROPERTY()
	bool bTwinStickMode;

public:
	virtual class ASnakePlayer* GetSnakePlayer() const override;

public:
	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundCue*> WeaponFireSoundCues;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* LazerRechargeSFX;

	/** Play the Audio Component Sound*/
	virtual void PlayAudio(int32 AudioCompIndex) override;

	/** Play the Audio Component Sound using the Base Sound */
	virtual void PlayAudio(int32 AuidoCompIndex, USoundBase* BaseSound) override;

	/** Stop the Audio Components Sound */
	virtual void StopAudio(int32 AudioCompIndex) override;

	/** if true, play the weapon fire sound cue */
	virtual bool CanPlayWeaponAudio() const override;

	/** Weapon has changed, flag to change the audio cue on next fire. */
	bool bSetNextWeaponCue;

	/** Assign a new Sound Cue to the Weapon Fire Audio Component */
	void SetWeaponSoundCue(USoundCue* InCue);

	UAudioComponent* GetAudioComponent(int32 AudioCompIndex);

	UPROPERTY()
	bool bCanPlayShapeAudio;

protected:
	UPROPERTY(EditAnywhere, Category = "Head|DataTable")
	FName CharacterRowName;

	UPROPERTY(EditAnywhere, Category = "Head|DataTable")
	UDataTable* CharacterDataTable;

	UPROPERTY(EditAnywhere, Category = "Head|DataTable")
	FName ForceFieldRowName;

	UPROPERTY(EditAnywhere, Category = "Head|DataTable")
	UDataTable* ForceFieldDataTable;

	UPROPERTY(EditAnywhere, Category = "Head|DataTable")
	TArray<FName> WeaponRowNames;

	UPROPERTY(EditAnywhere, Category = "Head|DataTable")
	UDataTable* WeaponDataTable;

	static const FString CharacterHealthContext;
	static const FString ForceFieldContext;
	static const FString WeaponContext;

public:
	/** Set the variables for Spawn effect
	*	False = Spawn In
	*	True = Spawn Out */
	UFUNCTION()
	virtual void InitializeSpawnEffect(bool InDirection) override;

	/** Activate the Spawn Particle effect and initialize the variables
	*	False = Spawn In
	*	True = Spawn Out */
	virtual void StartSpawnEffect(bool InDirection) override;

};
