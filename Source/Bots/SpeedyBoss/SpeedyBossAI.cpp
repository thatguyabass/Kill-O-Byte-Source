// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SpeedyBossAI.h"
#include "Bots/SpeedyBoss/SpeedyBoss.h"
#include "SnakeCharacter/SnakeLinkHead.h"
//#include "Bots/Controller/TargetActor.h"

ASpeedyBossAI::ASpeedyBossAI(const FObjectInitializer& PCIP)
	:	AGruntAI(PCIP)
{
	WaitDuration = 10.0f;
	PhaseIndex = 0;

	DamageThreshold = 0.33f;
	StoredDamage = 0.0f;
	
	FSpeedBossPhase Phase = FSpeedBossPhase();
	for (int32 c = 0; c < 3; c++)
	{
		BossPhase.Add(Phase);
	}

	WaitProgress = 0.0f;
	FireProgress = 0.0f;

	SecondaryFiringDuration = 3.0f;
	SecondaryFiringProgress = 0.0f;
}

void ASpeedyBossAI::InitializeBlackboardKeys()
{
	Super::InitializeBlackboardKeys();

	EncounterKey = BlackboardComponent->GetKeyID("EncounterStarted");
	SecondaryFiringKey = BlackboardComponent->GetKeyID("SecondaryFiring");
}

void ASpeedyBossAI::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	SetEncounterStarted(false);
}

bool ASpeedyBossAI::WaitAndFire(float DeltaTime)
{
	ASpeedyBoss* Boss = Cast<ASpeedyBoss>(GetPawn());
	if (!Boss || !GetEncounterStarted())
	{
		return true;
	}

	bool bFinished = false;

	WaitProgress += DeltaTime;
	if (WaitProgress > WaitDuration)
	{
		WaitProgress = 0.0f;

		Boss->SetSlowImmunity(false);

		bFinished = true;
	}

	FireProgress += DeltaTime;
	if (FireProgress > BossPhase[PhaseIndex].FireRate)
	{
		FireProgress = 0.0f;
		Boss->FireWeapon();
	}

	return bFinished;
}

bool ASpeedyBossAI::FireSecondaryWeapon(float DeltaTime)
{
	ASpeedyBoss* Boss = Cast<ASpeedyBoss>(GetPawn());
	if (!Boss)
	{
		return true;
	}

	bool bFinished = false;

	Boss->FireSecondaryWeapon();

	SecondaryFiringProgress += DeltaTime;
	if (SecondaryFiringProgress > SecondaryFiringDuration)
	{
		SecondaryFiringProgress = 0.0f;

		SetSecondaryFiring(false);
		bFinished = true;
	}

	return bFinished;
}

void ASpeedyBossAI::FindTarget()
{
	if (GetTarget())
	{
		return;
	}

	for (TActorIterator<ASnakeLinkHead> Iter(GetWorld()); Iter; ++Iter)
	{
		if (*Iter)
		{
			SetTarget(*Iter);
			return;
		}
	}
}

AActor* ASpeedyBossAI::GetBossSpawnArea()
{
	ASpeedyBoss* Boss = Cast<ASpeedyBoss>(GetPawn());
	if (!Boss)
	{
		return nullptr;
	}

	return Boss->SpawnArea;
}

void ASpeedyBossAI::StartEncounter()
{
	SetEncounterStarted(true);
}

bool ASpeedyBossAI::GetEncounterStarted() const
{
	if (BlackboardComponent)
	{
		return BlackboardComponent->GetValue<UBlackboardKeyType_Bool>(EncounterKey);
	}

	return false;
}

void ASpeedyBossAI::SetEncounterStarted(bool Value)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValue<UBlackboardKeyType_Bool>(EncounterKey, Value);
	}
}

bool ASpeedyBossAI::GetSecondaryFiring() const
{
	if (BlackboardComponent)
	{
		return BlackboardComponent->GetValue<UBlackboardKeyType_Bool>(SecondaryFiringKey);
	}

	return false;
}

void ASpeedyBossAI::SetSecondaryFiring(bool Value)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValue<UBlackboardKeyType_Bool>(SecondaryFiringKey, Value);
	}
}

void ASpeedyBossAI::IncreaseBossPhase()
{
	ASpeedyBoss* Boss = Cast<ASpeedyBoss>(GetPawn());
	if (!Boss)
	{
		return;
	}

	PhaseIndex++;
	if (PhaseIndex >= BossPhase.Num())
	{
		PhaseIndex = BossPhase.Num() - 1;
	}

	Boss->OverrideWeaponFireRate(BossPhase[PhaseIndex].FireRate);
}

void ASpeedyBossAI::DamageTaken(float Damage)
{
	ASpeedyBoss* Boss = Cast<ASpeedyBoss>(GetPawn());
	if (!Boss)
	{
		return;
	}

	StoredDamage += Damage;

	float Max = Boss->GetMaxHealth();
	if (StoredDamage > Max * DamageThreshold)
	{
		StoredDamage -= Max * DamageThreshold;
		
		IncreaseBossPhase();
		Boss->RemoveSlow();
	}

	if (Boss->IsSlowed())
	{
		StoredSlowDamage += Damage;

		if (StoredSlowDamage > Max * SlowDamageThreshold)
		{
			StoredSlowDamage = 0.0f;

			SetSecondaryFiring(true);
			Boss->RemoveSlow();
		}
	}
}