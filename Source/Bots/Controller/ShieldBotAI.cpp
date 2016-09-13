// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "ShieldBotAI.h"
#include "Bots/Bot/ShieldBot.h"

AShieldBotAI::AShieldBotAI(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	bCanUseNullEMP = true;
	NullDelayProgress = 0.0f;
	NullDelayDuration = 10.0f;

	bFaceTarget = false;
	TurnRotationRate = 20.0f;
}

void AShieldBotAI::InitializeBlackboardKeys()
{
	Super::InitializeBlackboardKeys();

	RetreatKey = BlackboardComponent->GetKeyID("Retreat");
}

void AShieldBotAI::SetTarget(AActor* InTarget)
{
	Super::SetTarget(InTarget);

	if (InTarget == nullptr)
	{
		AShieldBot* Bot = Cast<AShieldBot>(GetPawn());
		if (Bot)
		{
			Bot->RemoveShield();
		}
	}
}

void AShieldBotAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bFaceTarget)
	{
		FaceTarget(TurnRotationRate * DeltaTime);
	} 

	if (!bCanUseNullEMP)
	{
		NullDelayProgress += DeltaTime;
		if (NullDelayProgress > NullDelayDuration)
		{
			bCanUseNullEMP = true;
			NullDelayProgress = 0.0f;
		}
	}
}

void AShieldBotAI::DeployShieldOnSight()
{
	AShieldBot* Bot = Cast<AShieldBot>(GetPawn());
	AActor* Target = GetTarget();
	if (!Bot || !Target)
	{
		return;
	}

	bool bSight = CheckLineOfSightAdvanced(Bot, Target);
	if (bSight)
	{
		Bot->DeployShield();
	}
	else
	{
		Bot->RemoveShield();
	}
}

bool AShieldBotAI::ActivateNullifyingEMP()
{
	AShieldBot* Bot = Cast<AShieldBot>(GetPawn());
	AActor* Target = GetTarget();
	if (!Bot || !Target)
	{
		return false;
	}

	if (bCanUseNullEMP)
	{
		Bot->ActivateNullifyingEMP(Target);
		bCanUseNullEMP = false;

		return true;
	}

	return false;
}

void AShieldBotAI::FindFocus()
{
	AActor* Target = GetTarget();
	if (Target)
	{
		SetFocus(Target);
		bFaceTarget = false;
	}
}

void AShieldBotAI::RemoveFocus()
{
	SetFocus(nullptr);
	bFaceTarget = true;
}

bool AShieldBotAI::IsShieldShutdown()
{
	AShieldBot* Bot = Cast<AShieldBot>(GetPawn());
	if (!Bot)
	{
		return false;
	}

	return Bot->IsShieldShutdown();
}

void AShieldBotAI::SetRetreat(bool InValue)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValue<UBlackboardKeyType_Bool>(RetreatKey, InValue);
	}
}

bool AShieldBotAI::GetRetreat() const
{
	return BlackboardComponent ? BlackboardComponent->GetValue<UBlackboardKeyType_Bool>(RetreatKey) : false;
}

bool AShieldBotAI::GetNullEMPState() const
{
	return bCanUseNullEMP;
}