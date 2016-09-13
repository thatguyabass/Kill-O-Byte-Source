// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "MultiBeamTurret.h"
#include "Powers/Projectiles/Lazor.h"
#include "GameMode/PlayerState/SnakePlayerState.h"
#include "Utility/DataTables/StatData.h"

const FString AMultiBeamTurret::WeaponDataContext("MultiBeamTurretWeaponContext");

AMultiBeamTurret::AMultiBeamTurret()
	: ABeamTurret()
{
	WeaponCount = 4;
	WeaponSpread = 90.0f;

	LazerDamage = 0;
	LazerDuration = 0.0f;

	bFireOnSpawn = false;
	bPlayOnce = false;

	bUsePreFire = true;
	PreFireDuration = 2.5f;

	LifeTime = 0.0f;
}

void AMultiBeamTurret::SpawnWeapon()
{
	if (!LazerClass || !WeaponDataTable)
	{
		return;
	}

	FLazerStatDataRow* LazerData = WeaponDataTable->FindRow<FLazerStatDataRow>(WeaponRowName, WeaponDataContext, true);
	if (LazerData)
	{
		LazerDamage = LazerData->Damage;
		LazerDuration = LazerData->Duration;
	}

	if (bFireOnSpawn)
	{
		FireWeapon();
	}
}

void AMultiBeamTurret::FireWeapon()
{
	if (bUsePreFire && SpawnEffectVFX)
	{
		SpawnEffectVFX->ActivateSystem();

		GetWorldTimerManager().SetTimer(Fire_TimerHandle, this, &AMultiBeamTurret::FireWeaponInternal, PreFireDuration, false);
	}
	else
	{
		FireWeaponInternal();
	}
}

void AMultiBeamTurret::FireWeaponInternal()
{
	if (!LazerClass)
	{
		return;
	}

	if (bUsePreFire && SpawnEffectVFX)
	{
		SpawnEffectVFX->DeactivateSystem();
	}

	FRotator Rotation = GetActorForwardVector().Rotation();
	FRotator Spread = FRotator(0.0f, WeaponSpread, 0.0f);

	for (int32 c = 0; c < WeaponCount; c++)
	{
		FVector Location = GetActorLocation();
		Rotation += Spread * c;

		FVector Offset = Rotation.RotateVector(FireOffset);

		ALazor* Lazer = GetWorld()->SpawnActor<ALazor>(LazerClass, Location, Rotation);
		Lazer->SetLazerOwner(this, Offset, this);
		Lazer->SetDamage(LazerDamage);
		Lazer->Duration = LazerDuration;
		Lazer->AttachRootComponentToActor(this, NAME_None, EAttachLocation::KeepWorldPosition);

		Lazers.Add(Lazer);
	}

	if (LazerDuration > 0.0f)
	{
		GetWorldTimerManager().SetTimer(Purge_TimerHandle, this, &AMultiBeamTurret::PurgeLazerArray, LazerDuration, false);
	}

	if (LifeTime > 0.0f)
	{
		GetWorldTimerManager().SetTimer(Dead_TimerHandle, this, &AMultiBeamTurret::FadeOutLazers, LifeTime, false);
	}
}

void AMultiBeamTurret::Dead(ASnakePlayerState* Killer)
{
	for (int32 c = 0; c < Lazers.Num(); c++)
	{
		Lazers[c]->HideAndDestroy();
	}

	ABot::Dead(Killer);
}

void AMultiBeamTurret::FadeOutLazers()
{
	PurgeLazerArray();

	if (bPlayOnce)
	{
		//Clean up the rest of the Actor
		GetWorldTimerManager().SetTimer(Dead_TimerHandle, this, &AMultiBeamTurret::HideAndDestroy, 2.5f, false);
	}
}

void AMultiBeamTurret::HideAndDestroy()
{
	Super::HideAndDestroy();

	for (int32 c = 0; c < Lazers.Num(); c++)
	{
		Lazers[c]->SetLifeSpan(2.0f);
	}
}

void AMultiBeamTurret::PurgeLazerArray()
{
	if (Lazers.Num() > 0)
	{
		for (int32 c = 0; c < Lazers.Num(); c++)
		{
			Lazers[c]->TerminateLazer();
		}

		Lazers.Reset();
	}
}

float AMultiBeamTurret::GetLazerDuration() const
{
	return LazerDuration;
}