// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BeamBotAI.h"
#include "Bots/Bot/BeamBot.h"
#include "Powers/Weapons/Weapon_Special.h"

ABeamBotAI::ABeamBotAI(const FObjectInitializer& PCIP)
	: AGruntAI(PCIP)
{
	BeamBotLazerState = EBeamBotLazerState::FireReady;

	PreFireDelay = 1.0f;
	LazerRechargeTime = 5.0f;
	PostChargeDelayRange = FVector2D(1.0f, 10.0f);
}

void ABeamBotAI::SetState(EBeamBotLazerState InState)
{
	if (InState != BeamBotLazerState)
	{
		BeamBotLazerState = InState;
		HandleStateChange();
	}
}

EBeamBotLazerState ABeamBotAI::GetState() const
{
	return BeamBotLazerState;
}

void ABeamBotAI::HandleStateChange()
{
	ABeamBot* Beam = Cast<ABeamBot>(GetPawn());
	if (!Beam)
	{
		return;
	}

	switch (BeamBotLazerState)
	{
		case EBeamBotLazerState::FireReady: break;
		case EBeamBotLazerState::PreFire:
		{
			GetWorldTimerManager().ClearTimer(PreFire_TimerHandle);
			GetWorldTimerManager().SetTimer(PreFire_TimerHandle, this, &ABeamBotAI::FireWeaponInternal, PreFireDelay, false);
			break;
		}
		case EBeamBotLazerState::FireInprogress:
		{
			float FireDuration = Beam->GetWeapon()->GetLazerDuration();

			GetWorldTimerManager().ClearTimer(Fire_TimerHandle);
			GetWorldTimerManager().SetTimer(Fire_TimerHandle, this, &ABeamBotAI::SetStateCharging, FireDuration, false);
			break; 
		}
		case EBeamBotLazerState::Charging:
		{
			GetWorldTimerManager().ClearTimer(Charge_TimerHandle);
			GetWorldTimerManager().SetTimer(Charge_TimerHandle, this, &ABeamBotAI::SetStatePostCharge, LazerRechargeTime, false);
			break;
		}
		case EBeamBotLazerState::PostCharge:
		{
			ActivateShield();
			float PostChargeDelay = FMath::FRandRange(PostChargeDelayRange.X, PostChargeDelayRange.Y);

			GetWorldTimerManager().ClearTimer(PostCharge_TimerHandle);
			GetWorldTimerManager().SetTimer(PostCharge_TimerHandle, this, &ABeamBotAI::SetStateFireReady, PostChargeDelay, false);
			break;
		}
	}
}

void ABeamBotAI::SetStateCharging()
{
	SetState(EBeamBotLazerState::Charging);
}

void ABeamBotAI::SetStatePostCharge()
{
	SetState(EBeamBotLazerState::PostCharge);
}

void ABeamBotAI::SetStatePreFire()
{
	SetState(EBeamBotLazerState::PreFire);
}

void ABeamBotAI::SetStateFireReady()
{
	SetState(EBeamBotLazerState::FireReady);
}

void ABeamBotAI::ActivateShield()
{
	ABeamBot* Beam = Cast<ABeamBot>(GetPawn());
	if (Beam)
	{
		Beam->ActivateShield();
	}
}

void ABeamBotAI::DeactivateShield()
{
	ABeamBot* Beam = Cast<ABeamBot>(GetPawn());
	if (Beam)
	{
		Beam->DeactivateShield();
	}
}

void ABeamBotAI::BotDead(ASnakePlayerState* State)
{
	Super::BotDead(State);

	if (GetWorld())
	{
		FTimerManager& Manager = GetWorldTimerManager();
		Manager.ClearTimer(Fire_TimerHandle);
		Manager.ClearTimer(Charge_TimerHandle);
		Manager.ClearTimer(PostCharge_TimerHandle);
		Manager.ClearTimer(PreFire_TimerHandle);
	}
}

void ABeamBotAI::FireWeaponInternal()
{
	ABeamBot* Beam = Cast<ABeamBot>(GetPawn());
	if (Beam)
	{
		Beam->FireWeaponInternal();
	}
}

void ABeamBotAI::FindTarget()
{
	Super::FindTarget();

	if (GetTarget())
	{
		ABot* Bot = Cast<ABot>(GetPawn());
		if (Bot)
		{
			Bot->SearchRadius = 10000;
		}
	}
}