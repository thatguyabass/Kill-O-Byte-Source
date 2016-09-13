// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "CustomPlayerStart.h"

ACustomPlayerStart::ACustomPlayerStart(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	CooldownTimer = 5.0f;
	bRecentlyUsed = false;

	bReplicates = true;
}

bool ACustomPlayerStart::HasBeenRecentlyUsed()
{
	return bRecentlyUsed;
}

void ACustomPlayerStart::SetRecentlyUsed()
{
	bRecentlyUsed = true;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ACustomPlayerStart::Reset, CooldownTimer, false);
}

void ACustomPlayerStart::Reset()
{
	bRecentlyUsed = false;
}

void ACustomPlayerStart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACustomPlayerStart, bRecentlyUsed);
}