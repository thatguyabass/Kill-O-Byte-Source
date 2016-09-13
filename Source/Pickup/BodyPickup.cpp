// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BodyPickup.h"
#include "SnakeCharacter/SnakeLink.h"

ABodyPickup::ABodyPickup(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	BodyCount = 1;

	PrimaryActorTick.bCanEverTick = true;
}

void ABodyPickup::GiveTo(ASnakeLink* Link)
{
	for (int32 c = 0; c < BodyCount; c++)
	{
		//Link->SpawnBody();
	}

	bCollected = true;
	OnRep_Collected(bCollected);
}