// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BaseBoss.h"
#include "Powers/Weapons/Weapon.h"
#include "Bots/Bot/Component/WeakPoint.h"

// Sets default values
ABaseBoss::ABaseBoss()
{
	BaseHealth = 1000;

	bNextIndex = false;

	bPlaySpawnSFX = false;

	FireMode = EFireMode::Simultaneous;
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseBoss::StartEncounter()
{
	//Override this 
}

void ABaseBoss::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

// Called every frame
void ABaseBoss::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void ABaseBoss::SpawnWeapon()
{
	//Override is required for correct placement on the bot. Prevent the Base bot class from spawning an unnecessary weapon 
}

void ABaseBoss::FireWeapon()
{
	switch (FireMode)
	{
	case EFireMode::Simultaneous: SimultaneousFire(); break;
	case EFireMode::Sequential: SequentialFire(); break;
	}
}

void ABaseBoss::SimultaneousFire()
{
	bool bCanFire = false;

	for (int32 c = 0; c < Weapons.Num(); c++)
	{
		if (Weapons[c]->CanFire())
		{
			bCanFire = true;
			PreProjectileSpawn(c);
		}
	}

	if (bCanFire)
	{
		PlayFireWeaponSFX();
	}
}

void ABaseBoss::SequentialFire()
{
	if (Weapons.IsValidIndex(FireIndex) && Weapons[FireIndex]->CanFire())
	{
		if (bNextIndex)
		{
			SetNextFireIndex();
		}

		PlayFireWeaponSFX();

		PreProjectileSpawn(FireIndex);
		bNextIndex = true;
	}
}

void ABaseBoss::SetNextFireIndex()
{
	FireIndex++;
	if (FireIndex >= Weapons.Num())
	{
		FireIndex = 0;
	}
}

/** Override as required */
void ABaseBoss::PreProjectileSpawn(int32 WeaponIndex)
{
	if (Weapons.IsValidIndex(WeaponIndex))
	{
		Weapons[WeaponIndex]->Fire(FVector::ZeroVector);
	}
}

void ABaseBoss::SetWeakPoints(TArray<class AWeakPoint*> InArray)
{
	WeakPoints.Reset();

	WeakPoints = InArray;
	for (int32 c = 0; c < WeakPoints.Num(); c++)
	{
		WeakPoints[c]->SetWeakPointOwner(this);
	}
}

void ABaseBoss::TargetInstigator(AController* Instigator)
{

}