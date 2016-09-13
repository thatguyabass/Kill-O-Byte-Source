// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BeamBot.h"
#include "Powers/Shield/Repulsion.h"
#include "Powers/Weapons/Weapon_Special.h"
#include "Bots/Controller/BeamBotAI.h"

ABeamBot::ABeamBot()
	: ABot()
{
	PreFireParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PreFireParticleComponent"));
	PreFireParticle->AttachTo(RootComponent);

	ChargeSFXComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ChargeSFXComponent"));
	ChargeSFXComponent->bAutoActivate = false;
	ChargeSFXComponent->bAlwaysPlay = false;
	ChargeSFXComponent->AttachTo(RootComponent);
}

void ABeamBot::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SpawnShield();

	PreFireParticle->DeactivateSystem();
}


void ABeamBot::FireWeapon()
{
	ABeamBotAI* AI = Cast<ABeamBotAI>(Controller);
	if (AI && AI->GetState() == EBeamBotLazerState::FireReady)
	{
		ChargeSFXComponent->Play();
		PreFireParticle->ActivateSystem();
		AI->SetState(EBeamBotLazerState::PreFire);
	}
}

void ABeamBot::FireWeaponInternal()
{
	ABeamBotAI* AI = Cast<ABeamBotAI>(Controller);
	if (AI && AI->GetState() == EBeamBotLazerState::PreFire)
	{
		PreFireParticle->DeactivateSystem();
		ChargeSFXComponent->Stop();

		Super::FireWeapon();
		AI->SetState(EBeamBotLazerState::FireInprogress);
		DeactivateShield();
	}
}

AWeapon_Special* ABeamBot::GetWeapon() const
{
	return Cast<AWeapon_Special>(Weapon);
}

void ABeamBot::SpawnShield()
{
	if (RepulsionClass)
	{
		FVector Location = GetActorLocation();
		FRotator Rotation = FRotator::ZeroRotator;

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ARepulsion* Repel = GetWorld()->SpawnActor<ARepulsion>(RepulsionClass, Location, Rotation, Params);
		if (Repel)
		{
			RepulsionField = Repel;
			RepulsionField->AttachRootComponentToActor(this, NAME_None, EAttachLocation::KeepWorldPosition, false);
		}
	}
}

void ABeamBot::ActivateShield()
{
	if (RepulsionField)
	{
		RepulsionField->Show();
	}
}

void ABeamBot::DeactivateShield()
{
	if (RepulsionField)
	{
		RepulsionField->Hide();
	}
}

void ABeamBot::Dead(ASnakePlayerState* Killer)
{
	Super::Dead(Killer);

	PreFireParticle->DeactivateSystem();
	ChargeSFXComponent->Stop();

	if (GetWeapon() && GetWeapon()->ProjectileBeingFired())
	{
		GetWeapon()->TerminateProjectile();
	}

	if (RepulsionField)
	{
		RepulsionField->Hide();
		RepulsionField->SetLifeSpan(DestroyTime);
	}
}

void ABeamBot::PlayFireWeaponSFX()
{
	//Dont play Fire Audio. The Audio is attached to the lazer projectile!
}