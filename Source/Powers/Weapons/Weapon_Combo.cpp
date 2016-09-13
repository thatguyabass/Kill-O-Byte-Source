// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "Weapon_Combo.h"

AWeapon_Combo::AWeapon_Combo()
	: Super()
{
	//Index = -1;
}

void AWeapon_Combo::Fire(FVector FireOffset, int32 Index)
{
	if (!Inventory.IsValidIndex(Index) || !bCanFire)
	{
		return;
	}

	FireCombo(FireOffset, Index);
}

void AWeapon_Combo::FireCombo(FVector FireOffset, int32 Index)
{
	if (!Inventory.IsValidIndex(Index))
	{
		GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Red, WeaponOwner->GetName() + " : " + GetName() + ": Invalid Index");
		return;
	}

	FWeaponData Data = Inventory[Index];
	ProjectileClass = Data.Projectile;
	FireRate = Data.FireRate;
	Damage = Data.Damage;

	if (!ProjectileClass)
	{
		return;
	}

	switch (Data.Type)
	{
		case EFireType::Single:
		{
			ProjectileCount = 1;
			FireSingle(FireOffset);
		}
		break;

		case EFireType::Multi:
		{
			ProjectileCount = Data.ProjectileCount;
			RotationSpread = Data.RotationSpread;
			FireDirectionOverride = Data.FireDirection;
			FireMulti(FireOffset);
		}
		break;

	default: break;
	}

	ServerFireCustom(Location, Rotation, ProjectileCount, ProjectileClass, FireRate, Damage);

	Rotation.Reset();
}

void AWeapon_Combo::ServerFireCustom_Implementation(const FVector& InLocation, const TArray<FRotator>& InRotations, int32 Index, TSubclassOf<ABaseProjectile> InProjectileClass, float InFireRate, int32 InDamage)
{
	if (bCanFire && Index >= 0)
	{
		ProjectileClass = InProjectileClass;
		FireRate = InFireRate;
		Damage = InDamage;

		FireWeapon(InLocation, InRotations, Index);
	}
}