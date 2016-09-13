// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "HealthPickup.h"
#include "SnakeCharacter/SnakeLink.h"

AHealthPickup::AHealthPickup(const class FObjectInitializer& PCIP)
	: APickup(PCIP)
{
	HealthPercent = 0.25f;
}

void AHealthPickup::GiveTo(ASnakeLink* Link)
{
	float MaxHealth = Link->GetTotalHealth();
	float Heal = MaxHealth * HealthPercent;

	Link->AddHealth(Heal);

	bCollected = true;

	OnCollected();
}
