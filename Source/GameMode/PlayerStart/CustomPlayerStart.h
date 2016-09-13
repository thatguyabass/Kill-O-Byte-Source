// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerStart.h"
#include "CustomPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ACustomPlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
public:
	ACustomPlayerStart(const class FObjectInitializer& PCIP);
	
	/** which team can use this spawn point? */
	UPROPERTY(EditInstanceOnly, Category = "Team")
	int32 SpawnTeamNumber;

	/** if true, this spawnpoint has been used recently */
	bool HasBeenRecentlyUsed();

	/** Flag this point as recently used */
	void SetRecentlyUsed();

private:
	FTimerHandle TimerHandle;

	/** Has a player used this Recently? */
	UPROPERTY(Replicated)
	bool bRecentlyUsed;

	/** Time before this spawn point can be used again */
	float CooldownTimer;

	void Reset();

};
