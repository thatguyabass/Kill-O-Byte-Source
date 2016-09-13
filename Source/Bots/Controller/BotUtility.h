// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Turret/Turret.h"
#include "Bots/Bot/Bot.h"
#include "SnakeCharacter/SnakeLink.h"
#include "Object.h"
#include "BotUtility.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API UBotUtility : public UObject
{
	GENERATED_BODY()

public:
	/** Get the team numbers of each actor and check if they are the same. If true, the actors are enemies */
	static bool TargetCheck(AActor* Self, AActor* Target);

	/** Find the Object type and get the team number, Default returns -1 */
	static int32 GetTeamNumber(AActor* Actor);

	/** Get all the targets that are not on out team in the search radius */
	static void GetTeamBasedTargets(AActor* Owner, TArray<AActor*>& Out, float SearchRadius, FCollisionQueryParams& Params, FCollisionObjectQueryParams& ObjParams);

	/** Find the Closest Target to the owner */
	static AActor* GetClosestTarget(AActor* Owner, TArray<AActor*>& Targets);

	/** Sort through an array of targets, eliminating targets on the same team as the owner */
	static void SortTargets(AActor* Owner, TArray<AActor*>& Out);

	/** Cast the actors */
	static ATurret* TurretCast(AActor* Turret);
	static ABot* BotCast(AActor* Bot);
	static ASnakeLink* LinkCast(AActor* Link);
};