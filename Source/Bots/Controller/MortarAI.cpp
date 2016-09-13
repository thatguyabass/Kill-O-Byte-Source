// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "MortarAI.h"
#include "Bots/Bot/MortarBot.h"

AMortarAI::AMortarAI(const FObjectInitializer& PCIP)
	: AGruntAI(PCIP)
{

}

void AMortarAI::SetWeaponTarget(AActor* WeaponTarget, float SearchRadius)
{
	AMortarBot* Bot = Cast<AMortarBot>(GetPawn());
	AActor* Target = GetTarget();
	if (!Bot || !Target || !WeaponTarget)
	{
		return;
	}

	const FVector SearchOrigin = Target->GetActorLocation();
	FVector Location = UNavigationSystem::GetRandomReachablePointInRadius(Bot, SearchOrigin, SearchRadius);
	if (Location != FVector::ZeroVector)
	{
		WeaponTarget->SetActorLocation(Location);
	}
}

void AMortarAI::SetTarget(AActor* InTarget)
{
	Super::SetTarget(InTarget);

	AMortarBot* Bot = Cast<AMortarBot>(GetPawn());
	if (Bot && InTarget)
	{
		//A target has been found. Dont lose them!
		Bot->SearchRadius = 10000.0f;
	}
}