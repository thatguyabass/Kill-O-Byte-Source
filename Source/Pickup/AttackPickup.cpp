// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "AttackPickup.h"
#include "SnakeCharacter/SnakeLink.h"
#include "Powers/Weapons/Weapon.h"

AAttackPickup::AAttackPickup(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	Duration = 10.0f;
}

void AAttackPickup::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AAttackPickup::GiveTo(ASnakeLink* Link)
{
	Link->ChangeWeapon(WeaponClass);

	bCollected = true;
	OnRep_Collected(bCollected);
}