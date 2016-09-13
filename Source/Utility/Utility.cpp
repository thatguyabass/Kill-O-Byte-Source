// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "Utility.h"
#include "Powers/Shield/Repulsion.h"

void FShieldPower::ClearFields()
{
	if (RepulsionFields.Num() > 0)
	{
		for (int32 c = 0; c < RepulsionFields.Num(); c++)
		{
			RepulsionFields[c]->HideAndDestroy();
		}
		RepulsionFields.Reset();
	}
}

void FShieldPower::TriggerTimers()
{
	float RemainingTime = Duration - Progress;
	for (int32 c = 0; c < RepulsionFields.Num(); c++)
	{
		RepulsionFields[c]->TriggerTimer(RemainingTime);
	}
}

ARepulsion* FShieldPower::GetField(AActor* RepulsionOwner)
{
	for (int32 c = 0; c < RepulsionFields.Num(); c++)
	{
		if (RepulsionFields[c]->RepulsionOwner && RepulsionOwner && RepulsionFields[c]->RepulsionOwner == RepulsionOwner)
		{
			return RepulsionFields[c];
		}
	}
	return nullptr;
}