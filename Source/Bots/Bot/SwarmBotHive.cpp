// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SwarmBotHive.h"
#include "SwarmBotBee.h"
#include "Powers/Weapons/Weapon.h"
#include "GameMode/PlayerState/SnakePlayerState.h"

ASwarmBotHive::ASwarmBotHive()
	: ABot()
{
	bNextIndex = false;

	FireIndex = 0;
	HealthThreshold = 0.5f;
}

void ASwarmBotHive::BeginPlay()
{
	Super::BeginPlay();
}

void ASwarmBotHive::FireWeapon()
{
	if (AttachedActors.Num() < 0 || bDead)
	{
		return;
	}

	//Ensure the Fire Index is valid
	if (!AttachedActors.IsValidIndex(FireIndex))
	{
		//Reset the fire index. With the last check this should always be valid
		FireIndex = 0;
	}

	if (FireMode != EFireMode::Sequential)
	{
		for (int32 c = 0; c < AttachedActors.Num(); c++)
		{
			AttachedActors[c]->FireWeapon();
		}
	}
	else if(AttachedActors[FireIndex]->GetWeapon())
	{
		if (AttachedActors[FireIndex]->GetWeapon()->CanFire())
		{
			if (bNextIndex)
			{
				SetNextFireIndex();
			}

			AttachedActors[FireIndex]->FireWeapon();
			bNextIndex = true;
		}
	}
	//else
	//{
	//	FString AttachedName = AttachedActors[FireIndex]->GetName();
	//	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Weapon NULL for actor: " + AttachedName);
	//}
}

void ASwarmBotHive::ReduceHealth(int32 Damage, ASnakePlayerState* Killer)
{
	int32 PreHealth = Health;
	ABot::ReduceHealth(Damage, Killer);

	int32 DamageTaken = PreHealth - Health;

	if (bDestroyOnDamage && !bDead && AttachedActors.Num() > 0)
	{
		CheckAttached(DamageTaken);
	}
}

void ASwarmBotHive::Dead(ASnakePlayerState* Killer)
{
	//Remove Bots here 
	for (auto Bot : AttachedActors)
	{
		Bot->HideAndDestroy();
	}

	ABot::Dead(Killer);
}

void ASwarmBotHive::CheckAttached(int32 Damage)
{
	if (AttachedActors.Num() > 0)
	{
		CurrentDamageTaken += Damage;
		float MaxHealth = GetMaxHealth();
		while (CurrentDamageTaken >= (HealthThreshold * MaxHealth))
		{
			if (AttachedActors.Num() <= 0)
			{
				break;
			}

			CurrentDamageTaken -= (HealthThreshold * MaxHealth);
			ASwarmBotBee* Last = AttachedActors.Last();
			Last->HideAndDestroy();

			AttachedActors.Remove(Last);
			SetNextFireIndex();
		}
	}
}

USceneComponent* ASwarmBotHive::GetSceneTarget(int32 Index)
{
	if (TargetSceneComponents.IsValidIndex(Index))
	{
		return TargetSceneComponents[Index];
	}

	return nullptr;
}

void ASwarmBotHive::SetNextFireIndex()
{
	FireIndex++;
	if (FireIndex >= AttachedActors.Num())
	{
		FireIndex = 0;
	}
}

void ASwarmBotHive::StartSpawnEffect()
{
	Super::StartSpawnEffect();
	
	for (int32 c = 0; c < AttachedActors.Num(); c++)
	{
		AttachedActors[c]->StartSpawnEffect(ActorHalfHeight, SpawnOffset, SpawnEffectOutlineWidth);
	}
}

void ASwarmBotHive::SetSpawnEffectBlend(float Blend)
{
	Super::SetSpawnEffectBlend(Blend);

	for (int32 c = 0; c < AttachedActors.Num(); c++)
	{
		AttachedActors[c]->SetSpawnEffectBlend(Blend);
	}
}