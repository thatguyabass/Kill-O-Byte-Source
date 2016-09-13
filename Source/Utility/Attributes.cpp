// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "Attributes.h"

UAttributes::UAttributes(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	BaseHealth = 1;
	BaseDamage = 1;
}

int32 UAttributes::CalculateHealth(const int32 InHealth, bool Bypass)
{
	int32 Health = BaseHealth * InHealth;
	
	if (!Bypass)
	{
		ensure(Health > 0);
	}

	return Health;
}

int32 UAttributes::CalculateDamage(const int32 InDamage)
{
	int32 Damage = BaseDamage * InDamage;

	if (ensure(Damage < 0))
	{
		Damage = 0;
	}

	return Damage;
}
