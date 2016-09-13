// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SpeedyBot.h"

ASpeedyBot::ASpeedyBot()
	: Super()
{

}

void ASpeedyBot::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StartEncounter();
}

float ASpeedyBot::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Damage <= 0 || bDead)
	{
		return 0.0f;
	}

	ABot::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	return 0.0f;
}

void ASpeedyBot::HideAndDestroy()
{
	ABot::HideAndDestroy();
}