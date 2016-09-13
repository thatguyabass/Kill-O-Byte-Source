// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Controller/BaseAIBoss.h"
#include "RocketTankAI.generated.h"

USTRUCT(BlueprintType)
struct FRocketTankPhases
{
	GENERATED_BODY()

	//Charge

	/** Can the tank charge during this phase? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	bool bCanCharge;

	/** Recharge time before charge can be used again */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	float ChargeCooldown;

	//Barrage
	/**  Number of Waves fired per Barrage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	int32 BarrageWaveCount;

	/** Recharge time before another barrage can be fired */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	float BarrageCooldown;

	/** Barrage Count before charge can be executed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	int32 MinBarrageCount;

	/** Duration of delay before charging */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	float PreChargeDuration;

	/** Duration of delay after charging */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	float PostChargeDuration;

	FRocketTankPhases()
	{
		bCanCharge = false;
		ChargeCooldown = 10.0f;
		BarrageWaveCount = 2;
		BarrageCooldown = 10.0f;
		MinBarrageCount = 2;
		PreChargeDuration = 5.0f;
		PostChargeDuration = 20.0f;
	}
};

UENUM(BlueprintType)
enum class ERocketTankState : uint8
{
	Idle,
	Barrage,
	Charge,
	PostCharge,
	PreCharge
};

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ARocketTankAI : public ABaseAIBoss
{
	GENERATED_BODY()
	
public:
	ARocketTankAI(const class FObjectInitializer& PCIP);
	
	/** Initialize Blackboard Keys */
	virtual void InitializeBlackboardKeys() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Possess(APawn* InPawn) override;

	UFUNCTION(BlueprintCallable, Category = "Barrage Phase")
	void SetWeaponTarget(AActor* WeaponTarget);

	/** Start the Barrage */
	UFUNCTION(BlueprintCallable, Category = "Barrage Phase")
	void StartBarrage();

	/** Fire the Main weapons a set number of times. The number of waves that fire is scaled by the Phase */
	UFUNCTION(BlueprintCallable, Category = "Barrage Phase")
	void FireBarrage();

	/** Increase the Current Barrage Wave Count */
	void IncreaseBarrageWave();

protected:
	/** Waves already fired this barrage */
	int32 CurrentBarrageCount;

	/** Total number of waves the barrage will fire */
	int32 MaxBarrageCount;

	/** Barrages fired since last charge */
	int32 BarrageFiredCount;

	/** Barrage Cooldown before it can be Fired again */
	float GetBarrageCooldown() const;

	FTimerHandle BarrageCooldown_TimerHandle;

	/** Method for BarrageCooldown_TimerHandle, to set the Barrage to the Ready State */
	void SetBarrageReadyTimer();

public:
	/** Length of the Charge Line trace and ultimately the Charge */
	UPROPERTY(EditAnywhere, Category = "Charge")
	float ChargeLength;

	/** Line Trace from the Origin towards teh target
	*	Find the contact location, store it, and wait 
	*	for a duration while the player gets out of the way
	*/
	UFUNCTION(BlueprintCallable, Category = "Charge")
	void StartCharge();

	/** Finish the Charge State and Transition to the Post Charge State */
	UFUNCTION(BlueprintCallable, Category = "Charge")
	void FinishCharge();

protected:
	/** Charge recharge before it is used again */
	float GetChargeCooldown() const;

	FTimerHandle ChargeCooldown_TimerHandle;

	/** Get the Cooldown time for charge via the current phase */
	void SetChargeReadyTimer();

public:

	/** Initialize the Pre Charge State */
	UFUNCTION(blueprintCallable, Category = "Pre Charge")
	void StartPreCharge();

	/** Initialize the Post Charge State */
	UFUNCTION(BlueprintCallable, Category = "Post Charge")
	void StartPostCharge();

	UFUNCTION()
	void DamageTaken(int32 Damage);

protected:
	/** Damage Taken while in the PostCharge State */
	int32 StoredDamage;

	/** Damage Percent before force leaveing the PostCharge State */
	float DamageThreshold;

	FTimerHandle PostChargeDuration_TimerHandle;
	FTimerHandle PreChargeDuration_TimerHandle;

	/** Get the Duration of the Post Charge State */
	float GetPostChargeDuration();
	
	/** Get the Duration of the Pre Charge State */
	float GetPreChargeDuration();

public:
	/** Phases of the Rocket Tank Encounter. Each phase has different values to change up the fight as it progresses. */
	UPROPERTY(EditDefaultsOnly, Category = "Phase")
	TArray<FRocketTankPhases> Phases;

	/** Set Phase Index */
	UFUNCTION(BlueprintCallable, Category = "Phase")
	void SetPhaseIndex(int32 Index);

	UFUNCTION(BlueprintPure, Category = "Phase")
	int32 GetCurrentPhaseIndex() const;

protected:
	/** Current Phase */
	int32 CurrentPhaseIndex;

	UFUNCTION(BlueprintPure, Category = "Phase")
	FRocketTankPhases GetCurrentPhase() const;

public:
	UFUNCTION(BlueprintCallable, Category = "State")
	void SetState(ERocketTankState State);

	UFUNCTION(BlueprintPure, Category = "State")
	ERocketTankState GetState() const;

protected:
	UPROPERTY()
	ERocketTankState CurrentState;

	/** Delay before Idle starts to tick */
	bool bIdleDelay;

	FTimerHandle IdleState_TimerHandle;

	/** Buffer zone between state changes. */
	UPROPERTY(EditAnywhere, Category = "State")
	float IdleStateBuffer;

	/** Reset bIdleDelay to False */
	void ResetIdleDelay();

	/** Set the Currnet State to Idle */
	void SetStateToIdle();
	/** Set the Current State to Charge */
	void SetStateToCharge();

public:
	/** Barrage Set Getter */
	bool GetBarrageSet() const;

	/** Barrage Set Setter */
	void SetBarrageSet(bool InValue);

	/** Barrage Ready Getter */
	bool GetBarrageReady() const;

	/** Barrage Ready Setter */
	void SetBarrageReady(bool InValue);

	/** Charge Set Getter */
	bool GetChargeSet() const;

	/** Charge Set Setter */
	void SetChargeSet(bool InValue);

	bool GetChargeReady() const;
	void SetChargeReady(bool InValue);

protected:
	bool bBarrageReady;
	int32 BarrageSetKey;

	bool bChargeReady;
	int32 ChargeSetKey;
	
private:
	/** Tick Method while in the Idle State */
	void IdleTick();

public:
	UPROPERTY(EditAnywhere, Category = "Rotation Rate")
	float FaceTargetRotationRate;

	/** Stop the timers for both Barrage and Charge while reseting the state to idle */
	UFUNCTION(BlueprintCallable, Category = "Control")
	void StopActionAbilities();

	/** Start the timers for both Barrage and Charge. State remains unchanged */
	UFUNCTION(BlueprintCallable, Category = "Control")
	void StartActionAbilities();

private:
	virtual void SetTarget(AActor* InTarget) override;

public:
	virtual void StartEncounter() override;

};
