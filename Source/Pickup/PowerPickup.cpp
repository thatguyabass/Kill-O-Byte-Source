// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "PowerPickup.h"
#include "SnakeCharacter/SnakeLink.h"

APowerPickup::APowerPickup(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	PowerType = EPowerType::None;
}

void APowerPickup::GiveTo(ASnakeLink* Link)
{
	//Link->SnakeController->SetPowerType(PowerType);

	bCollected = true;
	OnRep_Collected(bCollected);
}