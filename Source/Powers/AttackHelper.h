// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"

#include "Bots/Bot/BaseBoss.h"
#include "Bots/Turret/TubeTurret.h"
#include "Bots/Bot/Bot.h"
#include "SnakeCharacter/SnakeLinkHead.h"
#include "Powers/DamageTypes/DamageType_AttackMode.h"

namespace AttackHelper
{
	FORCEINLINE FAttackType FindAttackTypeInActor(AActor* InActor)
	{
		if (InActor == nullptr)
		{
			return FAttackType(EAttackMode::Attack05_White);
		}

		ASnakeLinkHead* Head = Cast<ASnakeLinkHead>(InActor);
		if (Head)
		{
			return Head->GetAttackType();
		}

		ABot* Bot = Cast<ABot>(InActor);
		if (Bot)
		{
			return Bot->GetAttackType();
		}

		ATubeTurret* Turret = Cast<ATubeTurret>(InActor);
		if (Turret)
		{
			return Turret->GetAttackType();
		}

		return FAttackType(EAttackMode::Attack05_White);
	}

	FDamageEvent CreateDamageEvent(TSubclassOf<UDamageType> DamageType, FAttackType AttackType, bool Override = false, EAttackMode OverrideType = EAttackMode::Attack05_White)
	{
		FDamageEvent Event;
		if (DamageType.GetDefaultObject() == nullptr)
		{
			return Event;
		}

		UDamageType_AttackMode* Temp = Cast<UDamageType_AttackMode>(DamageType->GetDefaultObject());
		if (Temp)
		{
			Temp->AttackMode = Override ? OverrideType : AttackType.AttackMode;
			Event.DamageTypeClass = Temp->GetClass();
		}

		return Event;
	}

	EAttackMode GetAttackMode(const FDamageEvent& DamageEvent)
	{
		if (DamageEvent.DamageTypeClass)
		{
			const UDamageType_AttackMode* DamageType = Cast<UDamageType_AttackMode>(DamageEvent.DamageTypeClass->GetDefaultObject());
			return DamageType ? DamageType->AttackMode : EAttackMode::Attack05_White;
		}

		return EAttackMode::Attack05_White;
	}

	static float NormalDifficultyScale = 0.45f;
	static float HardDifficultyScale = 0.2f;
	static float DifficultyScale = 0.45f;
	static float NeutralScale = 0.8f;

	void SetDifficultyScale(bool InDiff = false)
	{
		DifficultyScale = InDiff ? HardDifficultyScale : NormalDifficultyScale;
	}

	/** Calculate the Damage using the Attack Mode
	*	@Param Damage - Damage Value that has been dealt to this actor
	*	@Param DamageEvent - Damage Event passed from the Attacking Actor. Will be converted to a UDamageType_AttackMode to gather the attack mode from the attacking actor
	*	@Param AttackType - AttackType of the damaged actor.
	*
	*	Compare the AttackModes and scale the damage appropriately based on the difficulty of the gamemode. 
	*/
	void CalculateDamage(int32& Damage, const FDamageEvent& DamageEvent, FAttackType AttackType, bool DamageSameType = false, float SameDamagePercent = 0.4f)
	{
		EAttackMode Mode = GetAttackMode(DamageEvent);

		bool SameType = AttackType.CompareAttackMode(Mode);
		if (AttackType.AttackMode == EAttackMode::Attack05_White)
		{
			Damage *= NeutralScale;
		}
		else if (!SameType)
		{
			//Reduce the Damage by the Difficulty Scale
			Damage *= DifficultyScale;
		}
		else if (SameType && DamageSameType)
		{
			Damage = FMath::CeilToInt(Damage * SameDamagePercent);
		}
	}
}