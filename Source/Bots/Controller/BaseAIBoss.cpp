// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BaseAIBoss.h"
#include "SnakeCharacter/SnakeLinkHead.h"
#include "Bots/Controller/TargetActor.h"

ABaseAIBoss::ABaseAIBoss(const FObjectInitializer& PCIP)
	: ABaseAIController(PCIP)
{

}

void ABaseAIBoss::StartEncounter()
{
	//Override this
}

void ABaseAIBoss::FindTarget()
{
	if (GetTarget())
	{
		return;
	}

	for (TActorIterator<ASnakeLinkHead> Iter(GetWorld()); Iter; ++Iter)
	{
		if (*Iter)
		{
			SetTarget(*Iter);
			return;
		}
	}
}

bool ABaseAIBoss::GetEncounterStarted()
{
	return bEncounterStarted;
}