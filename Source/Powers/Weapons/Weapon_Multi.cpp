// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "Weapon_Multi.h"

AWeapon_Multi::AWeapon_Multi()
	: Super()
{
	ProjectileCount = 3;
	RotationSpread = 30.0f;

	FireDirectionOverride = EFireDirection::Forward;
}

void AWeapon_Multi::Fire(FVector FireOffset)
{
	if (!bCanFire)
	{
		return;
	}

	FireMulti(FireOffset);

	ServerFire(Location, Rotation, ProjectileCount);

	Rotation.Reset();
}

void AWeapon_Multi::Fire(USceneComponent* InComponent, FRotator InRotation)
{
	if (!bCanFire)
	{
		return;
	}

	FireMulti(FVector::ZeroVector, InComponent);

	ServerFire(Location, Rotation, ProjectileCount);

	Rotation.Reset();
}

void AWeapon_Multi::FireMulti(FVector FireOffset, USceneComponent* InComponent)
{
	if (ProjectileCount <= 0 || !bCanFire)
	{
		return;
	}

	USceneComponent* Comp = InComponent ? InComponent : nullptr;

	FireDirection = FireDirectionOverride;

	// Get the lowest int to half of the projectiles that will spawn
	int32 Min = FMath::FloorToInt(ProjectileCount / 2);
	FRotator Start = Comp ? Comp->GetForwardVector().Rotation() : GetInitialDirection();
	FRotator RotSpread = FRotator(0.0f, RotationSpread, 0.0f);

	//If the Projectile Count is even, offset the starting rotation by half of the spread
	if (ProjectileCount % 2 == 0)
	{
		Start += (RotSpread * 0.5f);
	}
	//Adjust the Start to half of the projectile count
	Start -= (RotSpread * Min);

	for (int32 c = 0; c < ProjectileCount; c++)
	{

		//Add the Rotation spread for each projectile that will spawn
		FRotator Next = Start + (RotSpread * c);
		Rotation.Add(Next);
	}

	FRotator Rot = Comp ? Comp->GetComponentRotation() : WeaponOwner->GetActorRotation();		
	FVector RotOffset = Rot.RotateVector(FireOffset);
	FVector TempLocation = Comp ? Comp->GetComponentLocation() : WeaponOwner->GetActorLocation();
	Location = TempLocation + RotOffset;
}