// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BlastProjectile.h"
#include "Powers/Weapons/Weapon.h"
#include "SnakeCharacter/SnakeLink.h"

ABlastProjectile::ABlastProjectile(const FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

void ABlastProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SpawnWeapon();
}

void ABlastProjectile::OnImpact(const FHitResult& Hit)
{
	Super::OnImpact(Hit);

	Detonate();
}

void ABlastProjectile::Detonate()
{
	if (Weapon)
	{
		Weapon->Fire(FVector(0,0,0));
		Weapon->StartDestroy();

		ASnakeLink* Link = Cast<ASnakeLink>(ProjectileOwner);
		//if (Link && Link->SnakeController)
		//{
		//	if (Link->SnakeController->GetPowerType() == EPowerType::BlastSecondary)
		//	{
		//		Link->SnakeController->SetPowerType(EPowerType::None);
		//	}
		//}
	}
}

void ABlastProjectile::SpawnWeapon()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (World && WeaponClass)
	{
		Weapon = Cast<AWeapon>(World->SpawnActor(WeaponClass));
		Weapon->SetWeaponOwner(this, ProjectileOwner);
	}
}