// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "ShieldMortarBot.h"
#include "Powers/Weapons/Weapon.h"
#include "Powers/Shield/ForceField.h"

AShieldMortarBot::AShieldMortarBot()
	: Super()
{

}

void AShieldMortarBot::BeginPlay()
{
	Super::BeginPlay();

	SpawnForceField();

	if (Weapon)
	{
		float FireRate = Weapon->GetFireRate();
		FireRate += FMath::FRandRange(-0.5f, 0.5f);
		Weapon->SetFireRate(FireRate);
	}
}

void AShieldMortarBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ForceField)
	{
		FRotator Rotation = ForceField->GetActorRotation();

		Rotation.Pitch = 0.0f;
		Rotation.Roll = 0.0f;

		ForceField->SetActorRotation(Rotation);
	}
}

float AShieldMortarBot::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Damage <= 0 || bDead || (IsShieldActive() && !IsShieldShutdown()))
	{
		if (!bOverrideInvulnerable)
		{
			Damage = 0.0f;
		}
	}

	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void AShieldMortarBot::SpawnForceField()
{
	if (ForceFieldClass)
	{
		const FVector Location = GetActorLocation();
		const FRotator Rotation = GetActorRotation();
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ForceField = GetWorld()->SpawnActor<AForceField>(ForceFieldClass, Location, Rotation, Params);
		ForceField->SetShieldOwner(this);
		ForceField->SetAttackType(GetAttackType());
		ForceField->bIsActive = false;
		ForceField->bCanRegen = false;

		ForceField->OnShutdown.BindUObject(this, &AShieldMortarBot::OnShieldShutdown);
	}
}

void AShieldMortarBot::DeployShield()
{
	if (ForceField && !ForceField->bIsActive)
	{
		ForceField->ActivateForceField();
	}
}

void AShieldMortarBot::RemoveShield()
{
	if (ForceField && ForceField->bIsActive)
	{
		if (ForceField->IsShutdown())
		{
			ForceField->bIsActive = false;
		}
		ForceField->DeactivateForceField();
	}
}

void AShieldMortarBot::OnShieldShutdown()
{
	RemoveShield();
}

void AShieldMortarBot::Dead(ASnakePlayerState* Killer)
{
	Super::Dead(Killer);

	if (ForceField)
	{
		ForceField->HideAndDestroy(false);
	}
}

void AShieldMortarBot::SetShieldActive(bool Active)
{
	ensure(ForceField);

	if (Active)
	{
		DeployShield();
	}
	else
	{
		RemoveShield();
	}
}

bool AShieldMortarBot::IsShieldActive() const
{
	return ForceField ? ForceField->bIsActive : false;
}

bool AShieldMortarBot::IsShieldShutdown() const
{
	return ForceField ? ForceField->IsShutdown() : true;
}

AForceField* AShieldMortarBot::GetForceField() const
{
	return ForceField;
}