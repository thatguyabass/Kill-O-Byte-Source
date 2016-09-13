// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "DirectorBoss.h"
#include "GameMode/PlayerState/SnakePlayerState.h"
#include "Bots/DirectorBoss/DirectorController.h"
#include "Bots/DirectorBoss/DeathSpeedBot.h"
#include "Bots/Bot/Bot.h"
#include "Powers/Weapons/Weapon.h"
#include "Bots/LazerBoss/LazerBossHead.h"

ADirectorBoss::ADirectorBoss()
	: Super()
{
	BarrageFireRate = 0.25f;
	BarrageProgress = 0.0f;

	MortarBotIndex = 0;
	bFireNext = false;
	bBarrageInprogress = false;

	bHeadMovement = false;
	ReturnToSpawnInterpRate = 150.0f;

	bTrackDamageTaken = false;
	DamageTaken = 0;
	DamageThresholdPercent = 0.35f;
}

void ADirectorBoss::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ADirectorBoss::BeginPlay()
{
	Super::BeginPlay();

	if (LazerBossHead)
	{
		LazerBossHead->StartEncounter();
		LazerBossHead->GetAIController()->SetBossState(ELazerBossState::None);
		LazerBossHead->OnLazerStateEnd.BindUObject(this, &ADirectorBoss::StartHeadCleanup);
	}
}

void ADirectorBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MortarBots.Num() > 0)
	{
		MortarHealthCheck();
	}

	if (bBarrageInprogress)
	{
		BarrageTick(DeltaTime);
	}

	if (bHeadMovement)
	{
		ReturnHeadToSpawn();
	}
}

float ADirectorBoss::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	ADirectorController* AI = Cast<ADirectorController>(Controller);
	if (!AI || bDead)
	{
		return 0.0f;
	}

	if (AI->CanDamageDirector())
	{
		Health -= Damage;

		if (bTrackDamageTaken && AI->GetBattleState() != EDirectorBattleState::Lazer)
		{
			int32 DamageThreshold = GetMaxHealth() * DamageThresholdPercent;
			DamageTaken += Damage;

			if (DamageTaken > DamageThreshold)
			{
				//Override the VulnerableTick and transition to Post Vulnerable. Ability will also be triggered
				AI->VulnerableTick(0.0f, true);
			}
		}

		if (Health <= 0)
		{
			if (!AI->IsFinalBattleState())
			{
				AI->IncreaseBattleState();
				OnTransitionStart();
			}
			else
			{
				Dead(Cast<ASnakePlayerState>(DamageCauser));
			}
		}
	}

	return 0.0f;
}

void ADirectorBoss::StartMovementState()
{
	StartDirectorMovement();
}

void ADirectorBoss::SetDecisionState(EDirectorDecisionState InState)
{
	ADirectorController* AI = Cast<ADirectorController>(Controller);
	if (!AI)
	{
		return;
	}

	AI->SetDecisionState(InState);
}

void ADirectorBoss::InitializeVulnerableState(EDirectorBattleState InBattleState)
{
	PlayFireWeaponSFX();

	ResetDamageTaken(true);

	switch (InBattleState)
	{
	case EDirectorBattleState::Speed: SpawnDeathBots(); break;
	case EDirectorBattleState::Mortar: SpawnInnerMortars(); break;
	case EDirectorBattleState::Lazer: StartLazerBossSequence(); break;
	}
}

void ADirectorBoss::InitializePostVulnerableState()
{
	FireWeaponSFXComponent->Stop();

	ResetDamageTaken(false);
}

void ADirectorBoss::SpawnDeathBots()
{
	SpawnBots(DeathBotClass, InnerSpawnLocations, DeathBots);
	for (int32 c = 0; c < DeathBots.Num(); c++)
	{
		DeathBots[c]->SetPointOfInterestGroup(PointOfInterestGroup);
	}
}

void ADirectorBoss::ActivateDeathBots()
{
	for (int32 c = 0; c < DeathBots.Num(); c++)
	{
		Cast<ADeathSpeedBot>(DeathBots[c])->SetBotState(EDeathSpeedState::Moving);
	}
}

void ADirectorBoss::CleanupDeathBots()
{
	for (int32 c = 0; c < DeathBots.Num(); c++)
	{
		//The bots will kill themselves 
		Cast<ADeathSpeedBot>(DeathBots[c])->SetBotState(EDeathSpeedState::Cleanup);
	}

	//Clear the reference
	DeathBots.Reset();
}

bool ADirectorBoss::AllBotsCleared()
{
	bool bCleared = true;

	ADirectorController* AI = Cast<ADirectorController>(Controller);
	if (AI)
	{
		switch(AI->GetBattleState())
		{
		case EDirectorBattleState::Speed: break;
		case EDirectorBattleState::Mortar:
		{
			if (MortarBots.Num() > 0)
			{
				bCleared = false;
			}
			break;
		}
		case EDirectorBattleState::Lazer: break;
		}
	}

	return bCleared;
}

void ADirectorBoss::SpawnOuterMortars()
{
	SpawnBots(MortarBotClass, OuterSpawnLocations, MortarBots);
	OnSpawnOuterMortars();
}

void ADirectorBoss::SpawnInnerMortars()
{
	SpawnBots(InnerMortarBotClass, InnerSpawnLocations, MortarBots);
}

void ADirectorBoss::StartBarrage()
{
	bBarrageInprogress = true;
	BarrageProgress = 0.0f;
}

void ADirectorBoss::BarrageTick(float DeltaTime)
{
	if (!bBarrageInprogress)
	{
		return;
	}

	BarrageProgress += DeltaTime;
	if (BarrageProgress > BarrageFireRate)
	{
		BarrageProgress = 0.0f;
		FireMortar();
	}
}

void ADirectorBoss::FinishBarrage()
{
	bBarrageInprogress = false;
	BarrageProgress = 0.0f;
	bFireNext = false;

	for (int32 c = 0; c < MortarBots.Num(); c++)
	{
		MortarBots[c]->bInvulnerable = false;
		
		FDamageEvent Event;
		int32 Health = MortarBots[c]->GetMaxHealth() * 2;
		MortarBots[c]->TakeDamage(Health, Event, nullptr, nullptr);
	}
}

void ADirectorBoss::FireMortar()
{
	if (MortarBots.Num() > 0 && MortarBots.IsValidIndex(MortarBotIndex))
	{
		if (bFireNext)
		{
			MortarBotIndex++;
			if (MortarBotIndex >= MortarBots.Num())
			{
				MortarBotIndex = 0;
			}
		}

		MortarBots[MortarBotIndex]->GetWeapon()->OverrideCanFire(true);
		MortarBots[MortarBotIndex]->FireWeapon();
	
		bFireNext = true;
	}
}

void ADirectorBoss::MortarHealthCheck()
{
	for (int32 c = 0; c < MortarBots.Num(); c++)
	{
		if (MortarBots[c]->IsDead())
		{
			MortarBots.RemoveAt(c);
			c--;
		}
	}
}

void ADirectorBoss::SpawnBots(TSubclassOf<ABot> InBotClass, TArray<AActor*>& SpawnLocations, TArray<ABot*>& OutBots)
{
	if (InBotClass)
	{
		for (int32 c = 0; c < SpawnLocations.Num(); c++)
		{
			FVector Location = SpawnLocations[c]->GetActorLocation();
			FRotator Rotation = SpawnLocations[c]->GetActorRotation();
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			ABot* Bot = GetWorld()->SpawnActor<ABot>(InBotClass, Location, Rotation, Params);
			OutBots.Add(Bot);
		}
	}
}

void ADirectorBoss::StartLazerBossSequence()
{
	if (LazerBossHead && LazerBossHead->GetAIController())
	{
		LazerBossHead->GetAIController()->SetBossState(ELazerBossState::Lazer);
		LazerBossHead->GetAIController()->bOverrideMovement = false;
	}
}

void ADirectorBoss::StartHeadCleanup()
{
	if (LazerBossHead && LazerBossHead->GetAIController())
	{
		LazerBossHead->GetAIController()->SetBossState(ELazerBossState::None);
		LazerBossHead->GetAIController()->bOverrideMovement = true;
		bHeadMovement = true;
	}
}

void ADirectorBoss::ReturnHeadToSpawn()
{
	if (LazerBossHead && HeadSpawnLocation)
	{
		FVector SpawnLocation = HeadSpawnLocation->GetActorLocation();
		float DeltaTime = GetWorld()->GetDeltaSeconds();
		FVector Location = FMath::VInterpConstantTo(LazerBossHead->GetActorLocation(), SpawnLocation, DeltaTime, ReturnToSpawnInterpRate);
		LazerBossHead->SetActorLocation(Location);

		if (Location.Equals(SpawnLocation, 10))
		{
			CleanupLazerBossHead();
		}
	}
}

void ADirectorBoss::CleanupLazerBossHead()
{
	if (LazerBossHead)
	{
		bHeadMovement = false;
	}
}

void ADirectorBoss::ResetDamageTaken(bool TrackDamageFlag)
{
	bTrackDamageTaken = TrackDamageFlag;

	DamageTaken = 0;
}