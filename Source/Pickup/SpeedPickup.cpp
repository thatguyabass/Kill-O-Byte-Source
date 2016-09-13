// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SpeedPickup.h"
#include "Utility/Utility.h"
#include "SnakeCharacter/SnakeLink.h"

ASpeedPickup::ASpeedPickup(const FObjectInitializer& PCIP)
	: APickup(PCIP)
{
	Speed.SpeedModifier = 0.10f;
}

void ASpeedPickup::GiveTo(ASnakeLink* Link)
{
	Link->ApplySpeed(Speed);

	bCollected = true;
}