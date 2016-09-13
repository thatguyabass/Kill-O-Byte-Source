// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "Weapon_UniqueMulti.h"

AWeapon_UniqueMulti::AWeapon_UniqueMulti()
	: Super()
{

}

void AWeapon_UniqueMulti::Fire(FVector Offset)
{
	ProjectileCount = ProjectileClasses.Num();
	if (ProjectileCount <= 0 || !bCanFire)
	{
		return;
	}

	FireMulti(Offset);

	ServerFireCustomMulti(Location, Rotation, ProjectileClasses);

	Rotation.Reset();
}

void AWeapon_UniqueMulti::ServerFireCustomMulti_Implementation(const FVector& InLocation, const TArray<FRotator>& InRotations, const TArray<TSubclassOf<ABaseProjectile>>& InProjectileClasses)
{
	if (bCanFire && InProjectileClasses.Num() > 0)
	{
		for (int32 c = 0; c < InProjectileClasses.Num(); c++)
		{
			ProjectileClass = InProjectileClasses[c];
			FRotator Rot = InRotations[c];

			FireWeapon(InLocation, Rot);
		}

		bCanFire = false;
	}
}