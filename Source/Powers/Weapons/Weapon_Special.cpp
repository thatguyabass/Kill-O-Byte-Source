// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "Weapon_Special.h"
#include "Utility/DataTables/StatData.h"
#include "Powers/Projectiles/Lazor.h"

AWeapon_Special::AWeapon_Special()
	: AWeapon()
{
	Duration = 2.0f;
	bCanFire = true;
	bForceDamageOverride = false;
}

void AWeapon_Special::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Lazer && Lazer->IsFinished())
	{
		if (Lazer->IsFinished())
		{
			Lazer = nullptr;
			OnProjectileFinishedFire.ExecuteIfBound();
		}
	}
}

void AWeapon_Special::Fire(FVector FireOffset)
{
	if (!LazerClass || !bCanFire)
	{
		return;
	}

	FireLazer(FireOffset);
}

void AWeapon_Special::FireLazer(FVector FireOffset)
{
	if (LazerClass && !Lazer)
	{
		FRotator OwnerRotation = WeaponOwner->GetActorRotation();
		FVector RotOffset = OwnerRotation.RotateVector(FireOffset);
		FVector OwnerLocation = WeaponOwner->GetActorLocation() + RotOffset;

		FTransform Trans = FTransform(OwnerRotation, OwnerLocation);
		Lazer = Cast<ALazor>(UGameplayStatics::BeginSpawningActorFromClass(this, LazerClass, Trans));
		
		Lazer->SetLazerOwner(WeaponOwner, FireOffset, ProjectileOwner);
		Lazer->SetDamage(Damage);
		Lazer->Duration = Duration;
		Lazer->bForceDamageOverride = bForceDamageOverride;

		UGameplayStatics::FinishSpawningActor(Lazer, Trans);
	}
}

bool AWeapon_Special::ProjectileBeingFired()
{
	return (Lazer != nullptr);
}

float AWeapon_Special::GetLazerDuration() const
{
	if (Lazer)
	{
		return Lazer->Duration;
	}

	return 0.0f;
}

void AWeapon_Special::TerminateProjectile()
{
	if (Lazer)
	{
		Lazer->TerminateLazer();
	}
}

void AWeapon_Special::GetTableData()
{
	if (!WeaponDataTable)
	{
		FString N = GetName();
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, "Weapon Data Table Missing for: " + N);
		return;
	}

	FLazerStatDataRow* LazerData = WeaponDataTable->FindRow<FLazerStatDataRow>(WeaponRow, WeaponDataContext, true);

	if (LazerData)
	{
		Damage = LazerData->Damage;
		Duration = LazerData->Duration;
	}
}

ALazor* AWeapon_Special::GetLazer()
{
	return Lazer;
}