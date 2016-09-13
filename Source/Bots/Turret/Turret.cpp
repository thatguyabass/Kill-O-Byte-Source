// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "Turret.h"

ATurret::ATurret()
	: ABot()
{
	AIControllerClass = nullptr;

	//TeamNumber = 5;
	Points = 50.0f;
	BaseHealth = 1;

	FireOffset = FVector::ZeroVector;
	FireRate = 1.0f;
}

void ATurret::SpawnWeapon()
{
	ABot::SpawnWeapon();

	if (bFireOnSpawn)
	{
		StartFire();
	}
}

void ATurret::StartFire()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ATurret::FireWeapon, FireRate, true);
}

void ATurret::Dead(ASnakePlayerState* Killer)
{
	ABot::Dead(Killer);

	GetWorldTimerManager().ClearTimer(TimerHandle);
}