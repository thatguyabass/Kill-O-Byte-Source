// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "ShieldMortarAI.h"
#include "Bots/Bot/ShieldMortarBot.h"

AShieldMortarAI::AShieldMortarAI(const FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

void AShieldMortarAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FaceTarget(0.25f);
}

void AShieldMortarAI::SetTarget(AActor* Target)
{
	Super::SetTarget(Target);

	SetFocus(nullptr);
}

void AShieldMortarAI::SetShieldActive(bool Active)
{
	AShieldMortarBot* Bot = Cast<AShieldMortarBot>(GetPawn());
	ensure(Bot);

	Bot->SetShieldActive(Active);
}

bool AShieldMortarAI::IsShieldActive() const
{
	AShieldMortarBot* Bot = Cast<AShieldMortarBot>(GetPawn());
	if (!Bot)
	{
		return false;
	}

	return Bot->IsShieldActive();
}