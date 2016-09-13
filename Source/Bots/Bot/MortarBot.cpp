// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "MortarBot.h"
#include "Bots/Controller/MortarAI.h"
#include "Powers/Weapons/Weapon.h"
#include "Powers/Weapons/Weapon_Single.h"
#include "Bots/Misc/RocketTankTargetDecal.h"

AMortarBot::AMortarBot()
	: Super()
{
	IdealDistance = 1500.0f;
	MissFireRadius = 250.0f;

	WeaponPlacement = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponPlacement"));
	WeaponPlacement->AttachTo(RootComponent);
}

void AMortarBot::SpawnWeapon()
{
	Super::SpawnWeapon();

	if (TargetClass)
	{
		ProjectileTarget = GetWorld()->SpawnActor<ARocketTankTargetDecal>(TargetClass);
	}
}

void AMortarBot::FireWeapon()
{
	AMortarAI* AI = Cast<AMortarAI>(Controller);
	if (Weapon && Weapon->CanFire() && AI)
	{
		PlayFireWeaponSFX();

		AActor* Target = ProjectileTarget;
		if (ProjectileTarget->IsActive())
		{
			Target = GetWorld()->SpawnActor<ARocketTankTargetDecal>(TargetClass);
			Target->SetLifeSpan(5.0f);
		}

		AI->SetWeaponTarget(Target, MissFireRadius);
		Weapon->SetHomingProjectile(Target, IdealDistance);
		Cast<AWeapon_Single>(Weapon)->Fire(WeaponPlacement, GetActorRotation());
	}
}