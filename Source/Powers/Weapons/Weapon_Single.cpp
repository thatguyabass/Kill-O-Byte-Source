// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "Weapon_Single.h"
#include "SnakeCharacter/SnakeLink.h"

AWeapon_Single::AWeapon_Single()
	: Super()
{

}

void AWeapon_Single::Fire(FVector FireOffset)
{
	if (!bCanFire)
	{
		return;
	}

	FireSingle(FireOffset);

	ServerFire(Location, Rotation);

	Rotation.Reset();
}

void AWeapon_Single::Fire(USceneComponent* InComponent, FRotator ActorRotation)
{
	if (!bCanFire || InComponent == nullptr)
	{
		return;
	}

	Location = InComponent->GetComponentLocation();


	FRotator Rot = ActorRotation + InComponent->GetRelativeTransform().Rotator();
	Rotation.Add(Rot);
	FireWeapon(Location, Rotation, 1);

	Rotation.Reset();
}

void AWeapon_Single::FireSingle(FVector FireOffset)
{
	if (bCanFire)
	{
		FRotator Rot = GetInitialDirection();

		FVector RotOffset = Rot.RotateVector(FireOffset);
		Location = WeaponOwner->GetActorLocation() + RotOffset;

		Rotation.Add(Rot);
	}
}

void AWeapon_Single::ServerFire_Implementation(const FVector& InLocation, const TArray<FRotator>& InRotations, int32 Index)
{
	if (bCanFire && Index >= 0)
	{
		FireWeapon(InLocation, InRotations, Index);
	}
}