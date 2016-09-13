// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BeamTurret.h"
#include "Powers/Weapons/Weapon.h"
#include "Powers/Weapons/Weapon_Special.h"
#include "Powers/Projectiles/Lazor.h"

ABeamTurret::ABeamTurret()
	: ATurret()
{
	bMultiFire = false;
	bForceDamageOverride = false;
}

void ABeamTurret::SpawnWeapon()
{
	if (!bMultiFire)
	{
		ABot::SpawnWeapon();

		if (bFireOnSpawn)
		{
			FireWeapon();
		}
	}
	else
	{
		ATurret::SpawnWeapon();
	}
}

void ABeamTurret::PostSpawnWeapon()
{
	AWeapon_Special* Special = Cast<AWeapon_Special>(Weapon);
	if (Special)
	{
		Special->bForceDamageOverride = bForceDamageOverride;
	}
}

void ABeamTurret::FireWeapon()
{
	Super::FireWeapon();
}

void ABeamTurret::TerminateProjectile()
{
	AWeapon_Special* Special = Cast<AWeapon_Special>(Weapon);
	if (Special)
	{
		Special->TerminateProjectile();
	}
}

ABeamTarget* ABeamTurret::GetBeamTarget()
{
	AWeapon_Special* Special = Cast<AWeapon_Special>(Weapon);
	if (Special)
	{
		ALazor* L = Special->GetLazer();
		if (L)
		{
			return L->GetBeamTarget();
		}
	}

	return nullptr;
}