// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Controller/GruntAI.h"
#include "BeamBotAI.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EBeamBotLazerState : uint8
{
	FireReady = 0,//State where weapon is ready to be fired 
	FireInprogress = 1,//State during the lazer being fired
	Charging = 2,//Charge the lazer for the next shot Also the relocation state
	PostCharge = 3,//Relocation continues before next shot
	PreFire = 4//State between Fire Ready and FireInprogress
};

UCLASS()
class SNAKE_PROJECT_API ABeamBotAI : public AGruntAI
{
	GENERATED_BODY()
	
public:
	ABeamBotAI(const class FObjectInitializer& PCIP);

	/** Timelimit for the lazer to fully recharge */
	UPROPERTY(EditAnywhere, Category = "BeamBot|Properties")
	float LazerRechargeTime;

	/** Range of the delay after the lazer has charged again */
	UPROPERTY(EditAnywhere, Category = "BeamBot|Properties")
	FVector2D PostChargeDelayRange;

	/** set the lazer state for this beam bot */
	UFUNCTION(BlueprintCallable, Category = "BeamBot|State")
	void SetState(EBeamBotLazerState InState);

	/** Get the current BeamBotLazerState */
	UFUNCTION(BlueprintCallable, Category = "BeamBot|State")
	EBeamBotLazerState GetState() const;

	virtual void BotDead(ASnakePlayerState* State) override;

	/** If a target is found. Do not lose this target */
	virtual void FindTarget() override;

protected:
	UPROPERTY()
	EBeamBotLazerState BeamBotLazerState;

	UPROPERTY(EditAnywhere, Category = "BeamBot|State")
	float PreFireDelay;

	void HandleStateChange();

	FTimerHandle PreFire_TimerHandle;
	FTimerHandle Fire_TimerHandle;
	FTimerHandle Charge_TimerHandle;
	FTimerHandle PostCharge_TimerHandle;

	/** Set the BeamBotLazerState to Charging */
	void SetStateCharging();

	/** Set the BeamBotLazerState To Post Charge */
	void SetStatePostCharge();

	/** Set the BeamBotLazerState to Pre Fire */
	void SetStatePreFire();

	/** Set the BeamBotLazerState to Fire Ready */
	void SetStateFireReady();

public:
	UFUNCTION(BlueprintCallable, Category = "BeamBot|Shield")
	void ActivateShield();

	UFUNCTION(BlueprintCallable, Category = "BeamBot|Shield")
	void DeactivateShield();

protected:
	void FireWeaponInternal();

};
