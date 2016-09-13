// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
* SnakeController is the base class of Actors that hold and manage the Snake Links. 
*
* SnakeControllers are non-physical actors that are spawned From the game mode 
* and attached to the Player Controllers
* SnakeControllers manage the entire snake health, speed, weapons and powerups 
* SnakeControllers are spawned and replicated to each player
*
* This class handles all actor spawning for the entire snake.
*
*/

#include "Powers/Projectiles/BuildProjectile.h"

#include "GameMode/PlayerState/SnakePlayerState.h"
#include "Utility/Utility.h"
#include "GameFramework/Actor.h"
#include "SnakeController.generated.h"

class ASnakeLink;

UCLASS()
class SNAKE_PROJECT_API ASnakeController : public AActor
{
	GENERATED_BODY()

public:
	ASnakeController();

	/** Initialize the Health Value */
	virtual void PostInitializeComponents() override;

	/** Update the Timers for the Powers */
	virtual void Tick(float DeltaTime) override;

	/** Clean the Controller and Destroy */
	void CleanAndDestroy();

	////////////////////////////////////////////////////////////////
	//Snake Body Methods

	/** Links that are spawned when the snake is extended */
	UPROPERTY(EditAnywhere, Category = "SnakeController|ObjectsToSpawn")
	TSubclassOf<ASnakeLink> BodyClass;

	/** Default Weapon Class */
	UPROPERTY(EditAnywhere, Category = "SnakeController|ObjectsToSpawn")
	TSubclassOf<class AWeapon> PrimaryWeaponClass;

	/** Secondary Weapon Class */
	UPROPERTY(EditAnywhere, Category = "SnakeController|ObjectsToSpawn")
	TSubclassOf<class AWeapon_Combo> SecondaryWeaponClass;

	/** The Entire list of SnakeLinks */
	UPROPERTY(BlueprintReadWrite, Transient, Replicated, Category = "SnakeController|Property")
	TArray<ASnakeLink*> Snake;

	/** The number of bodies the players start with. This doesn't include the head */
	UPROPERTY(EditAnywhere, Category = "SnakeLink|Property")
	int32 StartingBodyCount;

	/** What is the max size the body can reach */
	UPROPERTY(EditAnywhere, Category = "SnakeLink|Property")
	int32 MaxBodyCount;
	
private:
	int32 CurrentBodyCount;

	/** Return true if not at the max body count */
	bool CanSpawnBody();
	
	void IncreaseBodyCount();
	void DecreaseBodyCount();

public:
	/** Set the Snake Head for this Snake */
	void SetHead(ASnakeLink* NewHead);

	UFUNCTION(BlueprintCallable, Category = "SnakeController|Method")
	ASnakeLink* GetHead();

	/** [Server] Spawn a new Body Link to the Snake */
	void SpawnBody();

	/** Add an existing Body to the Snake */
	void AddBody(ASnakeLink* Link);

	/** Clean up the entire Snake when it has been destroyed */
	void CleanUpBody();

	/** Clean a Link before being destroyed */
	void CleanLink(ASnakeLink* Link);

	/** [Server] Remove a Body Link from the Snake */
	void RemoveBody(ASnakeLink* Actor, ECutType Type, ASnakePlayerState* Damager);

	/** Freeze the passed link and all the following links */
	void CutLink(ASnakeLink* Link, ASnakePlayerState* Damager);

	/** Check the First two links in the Snake */
	bool CheckFirstLink(ASnakeLink* Link);

	/** ### DEBUG ### */
	UFUNCTION(BlueprintCallable, Category = "SnakeController|Debug")
	void TriggerRemoveBody(int32 Index, ECutType Type);

	/** Apply movement changes to the entire Snake */
	void ApplyMovementChange();

	/** Apply a Shiled Change to the Entire Snake */
	void ApplyShieldChange();

	/** Spawn a New Weapon. This will override the Default Weapon. 
	*	@Param Duration - How long the weapon will remain equiped before resting to the default
	*/
	void ChangeWeapon(AWeapon* InWeaponClass, float Duration);

	/** Spawn the Weapons */
	void SpawnWeapons();

	/** Get the Color Data */
	FTeamColorData& GetTeamColorData();

	/** Set the Team Color Data */
	void SetTeamColorData(FTeamColorData InColorData);

	int32 GetTeamNumber();

private:
	/** Head is the Link that Drives the Snake */
	UPROPERTY(Transient, Replicated)
	ASnakeLink* Head;

	/** Team Color Data for this Snake */
	UPROPERTY(Replicated)
	FTeamColorData ColorData;

	/** Remove the Link from the Snake, but doesn't affect the following links */
	void AdjustSnake(ASnakeLink* Link);

	UFUNCTION()
	void OnRep_PrimaryWeapon();

	/** The Current Default Weapon */
	UPROPERTY(ReplicatedUsing = OnRep_PrimaryWeapon)
	class AWeapon* PrimaryWeapon;

	UPROPERTY(Replicated)
	class AWeapon* StoredWeapon;

	/** The Secondary Weapon. This will spawn bombs. Who could have guessed? */
	UPROPERTY(Replicated)
	class AWeapon_Combo* SecondaryWeapon;

	/** Equip the Primary Weapon */
	void EquipPrimary(AWeapon* Default);

	/** Equip The Secondary Weapon */
	void EquipSecondary(AWeapon_Combo* Secondary);

	/** Set the Primary Weapon */
	void SetPrimary(AWeapon* NewPrimaryWeapon);

	/** Set the Seconary Weapons */
	void SetSecondary(AWeapon_Combo* Secondary);

	/** Start the Destruction Sequence */
	void CleanUpWeapons();

	/** Reset the Primary Weapon */
	void ResetPrimaryWeapon();

	/** Activate A Blast Projectile's secondary effect */
	bool ActivateBlastSecondary();

	////////////////////////////////////////////////////////////////
	////// Snake Health Methods 

public:
	/** [Server] Reduce the Total Health of the snake */
	void ReduceHealthTotal();

	/** [Server] Reduce the Snake Health */
	void ReduceHealth(int32 Damage, ASnakePlayerState* Damager);

	/** [Server] Add To Total Health */
	void AddToTotalHealth(int32 InHealth);

	/** Get the Current Health of the Snake */
	int32 GetHealth() const;

	/** Get the Current Health Total of the Snake */
	int32 GetHealthTotal() const;

	/** Health per Link */
	int32 GetHealthPerLink() const;

	/** [Server] Set the player as dead */
	void Killed(ASnakePlayerState* Killer);

	/** Return True if the player is dead */
	bool IsDead();

	UPROPERTY(EditAnywhere, Category = "Attributes")
	class UAttributes* Attributes;

	/** The Initial Health Per Link. Pre health calculation */
	UPROPERTY(EditAnywhere, Category = "Attributes")
	int32 BaseHealthPerLink;

	/** bLastStand Getter. If true, the snake is in last stand */
	bool GetLastStand();

protected:
	/** Add to the Snake Killed and Deaths */
	void ScoreSnakeKill(ASnakePlayerState* Killer);

	/** Track a Link Kill */
	void ScoreLinkKill(ASnakePlayerState* Killer);
	
	void FloatyTextHead(FVector WorldLocation);
	void FloatyTextLink(FVector WorldLocation);
	void CreateFloatyText(int32 Score, FVector WorldLocation);

	/** Get the Floaty Text Manager to display the score */
	class AFloatyTextManager* GetFloatyText();

private:
	/** [Server] Re-evaluate Health after Cut
		@Param PreLegnth  The number of links before they were cut */
	void ReevaluateHealth(int32 PreLength, ASnakePlayerState* Damager);

	/** Snake Health */
	UPROPERTY(Replicated)
	int32 Health;

	/** Snake Total Health */
	UPROPERTY(Replicated)
	int32 HealthTotal;

	/** Health Per Link Body */
	int32 HealthPerLink;

	/** Player is Dead */
	UPROPERTY(Replicated)
	bool bDead;

	/** Is the Snake in last stand more? */
	UPROPERTY(Replicated)
	bool bLastStand;

	/** Last Stand Flag last tick. Used to check if last stand was entered this frame. */
	bool bLastStandLastTick;

public:
	////////////////////////////////////////////////////////////////
	////// Power Methods and Variables 

	/** [Server + Client]
	* Increase the Power Level of the power Type OR reset the powerlevel if the types are different.
	*
	* @param Type Power Type to be assigned to PowerType
	*/
	UFUNCTION(exec, BlueprintCallable, Category = "SnakeController|Power Type")
	void SetPowerType(EPowerType Type);

	UFUNCTION(BlueprintCallable, Category = "SnakeController|Power Type")
	EPowerType GetPowerType();

	/** Return the Current Power level. */
	UFUNCTION(BlueprintCallable, Category = "SnakeController|Power Type")
	int32 GetPowerLevel();

private:
	/** Current Power Type */
	UPROPERTY(Replicated)
	EPowerType PowerType;

	/** Timer Handle */
	FTimerHandle Reload_TimerHandle;
	FTimerHandle WeaponReset_TimerHandle;

	void ClearTimer(FTimerHandle& Handle);

	/** Increase the Power level of the Current Power Type. This will also Reset the power level first if the Type is different then the previous type */
	void HandleNewType();

	/** Max Power Level */
	static const int32 MaxPowerLevel;

	/** No Power Level, Safe to use when reseting power level */
	static const int32 NoPowerLevel;

	/** Current Power Level */
	UPROPERTY(Replicated)
	int32 PowerLevel;

	/** Ensure the Powerlevel is valid. If true, the powerlevel is INVALID */
	bool IsPowerLevelInvalid();

	/** Turns off all active powers. Shiled, Speed, Slow. */
	void DeactivatePowers(bool bApplyMovement = false);

	////////////////////////////////////////////////////////////////
	////// Fire Methods and Variables
public:

	/** Projectile Spawn Offset */
	UPROPERTY(EditAnywhere, Category = "SnakeController|Projectile")
	FVector FireOffset;

	/** [Server] Fire the Seoncary Weapon. An offset is applied based on the current power type which is used to choose teh correct projectile */
	bool FireSecondaryWeapon();

	/** [Server] Fire the Primary Weapon. Flag that the Fire Button is down */
	void FirePrimary();

	/** [Server] Fire the Secondary Power. Nothing will occur if there is no power level */
	void FireSecondary();

	/** The Fire Button has been released */
	void FireReleased();

private:
	/** Is the Fire Key being held. This is only tracking the Primary Fire */
	UPROPERTY(Replicated)
	bool bFireHeld;

	////////////////////////////////////////////////////////////////
	//////Slow Methods
public:
	/** Percentage point where the Snake will not Slow down any further */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SnakeController|Slow")
	float MaxSlowModifier;

	/** Add Slow Modifier to the Array */
	void AddSlowModifier(FSpeedPower SlowPower);

	/** Get the Sum of the Slow Modifiers and subtract them from a base of one */
	UFUNCTION(BlueprintPure, Category = "SnakeController|Movement")
	float GetSlowModifier();

	/** Get the Slow Modifier Sum */
	float GetSlowModifierRaw();

	/** Activate the Stasis Slow. The Stasis slow will last while the player remains inside the field */
	void AddStasisSlow(FSpeedPower StasisSlow);

	/** Set the Stasis slow to inactive. */
	void RemoveStasisSlow();

private:
	/** Slow Modifiers that have been collected AND that are currently active */
	UPROPERTY(Replicated)
	TArray<FSpeedPower> SlowPowers;

	UPROPERTY(Replicated)
	FSpeedPower StasisPower;

	/** Base Slow Modifier. This must = 1 */
	static const float BaseSlowModifier;

	////////////////////////////////////////////////////////////////
	//////Speed Methods and Variables

	/** Base Speed Modifier. This must = 1 */
	static const float BaseSpeedModifier;

public:
	/** Speed Power Tiers */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SnakeController|Speed")
	TArray<FSpeedPower> SpeedPowerTiers;

	/** Max Speed that the Snake can be increased by. Percentage based */
	UPROPERTY(EditAnywhere, Category = "SnakeController|Speed")
	float MaxSpeedModifier;

	UPROPERTY(EditAnywhere, Category = "SnakeController|Speed")
	float TeleportDistance;

	/** Get the Speed Modifier */
	UFUNCTION(BlueprintPure, Category = "SnakeController|Movement")
	float GetSpeedModifier() const;

	/** Get the Current Speed Power. */
	FSpeedPower& GetSpeedPower();

	/** Set a new Speed Power. Speed Power = Manual-Activate */
	void SetSpeedPower(FSpeedPower Power);

	/** Set a new Speed Boost Power. SpeedBoost = Auto-Activated */
	UFUNCTION(BlueprintCallable, Category = "SnakeController|Speed")
	void SetSpeedBoostPower(FSpeedPower Power);

private:
	/* Current Speed Power, This will hold the values when speed has been activated */
	UPROPERTY(Replicated)
	FSpeedPower SpeedPower;

	/** Speed Boost Power. Activated when the user runs over a speed boost area */
	UPROPERTY(Replicated)
	FSpeedPower SpeedBoostPower;

	/** Activate the Speed Power */
	bool ActivateSpeed();

	/** Can Speed be acitvated? */
	bool CanActivateSpeed() const;

	/** Teleport the Entire Snake. If the snake is too long the end may not move at all */
	void ActivateTeleport();

	/** Teleport a link to its target. */
	void TeleportLinkToTarget(ASnakeLink* Link);

public:
	////////////////////////////////////////////////////////////////
	//////Shield Power Methods and Variables

	/** Shield Power Tiers */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SnakeController|Shield")
	TArray<FShieldPower> ShieldPowerTier;

	/* if true, a shield has been applied to the snake */
	bool IsShieldActive();

	FShieldPower& GetShieldPower();

private:
	UPROPERTY(Replicated)
	FShieldPower ShieldPower;

	/** Activate the Shield Power for the Entire Snake */
	bool ActivateShield();

	/** Can Shield be activated? */
	bool CanActivateShield() const;

	/** Spawn a Repultion field. Only if at the Max Power Level */
	void SpawnRepulsionField(ASnakeLink* Link);

	/** Remove the repulsion field from the link and remove it from the array */
	void RemoveRepulsionField(ASnakeLink* Link);

public:
	/** Get Game Mode */
	class ABaseGameMode* GetGameMode() const;

protected:
	/** Get Head PlayerState */
	ASnakePlayerState* GetPlayerState() const;

public:
	////////////////////////////////////////////////////////////////
	//////Server, Client, and NetMultiCast Methods 

	//Server Spawn Body
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSpawnBody(FVector Origin, FRotator Rotation);
	virtual void ServerSpawnBody_Implementation(FVector Origin, FRotator Rotation);
	virtual bool ServerSpawnBody_Validate(FVector Origin, FRotator Rotation) { return true; }

	// Server Remove Body
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerRemoveBody(ASnakeLink* Link, ECutType Type, ASnakePlayerState* Damager);
	virtual void ServerRemoveBody_Implementation(ASnakeLink* Link, ECutType Type, ASnakePlayerState* Damager);
	virtual bool ServerRemoveBody_Validate(ASnakeLink* Link, ECutType Type, ASnakePlayerState* Damager) { return true; }

	//Server Set Power Type
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSetPowerType(EPowerType Type);
	virtual void ServerSetPowerType_Implementation(EPowerType Type);
	virtual bool ServerSetPowerType_Validate(EPowerType Type) { return true; }

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerFireSecondary();
	virtual void ServerFireSecondary_Implementation();
	virtual bool ServerFireSecondary_Validate() { return true; }

	//Reduce the Total health
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerReduceHealthTotal();
	virtual void ServerReduceHealthTotal_Implementation();
	virtual bool ServerReduceHealthTotal_Validate() { return true; }

	//Reduce the Current Health
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerReduceHealth(int32 Damage, ASnakePlayerState* Damager);
	virtual void ServerReduceHealth_Implementation(int32 Damage, ASnakePlayerState* Damager);
	virtual bool ServerReduceHealth_Validate(int32 Damage, ASnakePlayerState* Damager) { return true; }
	
	// Add to the Total and Current health.
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerAddToTotalHealth(int32 InHealth);
	virtual void ServerAddToTotalHealth_Implementation(int32 InHealth);
	virtual bool ServerAddToTotalHealth_Validate(int32 InHealth) { return true; }

	// Kill the Player on the Server
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerKilled(ASnakePlayerState* Killer);
	virtual void ServerKilled_Implementation(ASnakePlayerState* Killer);
	virtual bool ServerKilled_Validate(ASnakePlayerState* Killer) { return true; }

	// Reevaluate Health 
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerReevaluateHealth(int32 PreLength, ASnakePlayerState* Damager);
	virtual void ServerReevaluateHealth_Implementation(int32 PreLength, ASnakePlayerState* Damager);
	virtual bool ServerReevaluateHealth_Validate(int32 PreLength, ASnakePlayerState* Damager) { return true; }

	/** Server Equip Primary Weapons */
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerEquipPrimaryWeapon(AWeapon* Primary);
	virtual void ServerEquipPrimaryWeapon_Implementation(AWeapon* Primary);
	virtual bool ServerEquipPrimaryWeapon_Validate(AWeapon* Primary) { return true; }

	/** Server Equip Secondary Weapons */
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerEquipSecondaryWeapons(AWeapon_Combo* Secondary);
	virtual void ServerEquipSecondaryWeapons_Implementation(AWeapon_Combo* Secondary);
	virtual bool ServerEquipSecondaryWeapons_Validate(AWeapon_Combo* Secondary) { return true; }

};