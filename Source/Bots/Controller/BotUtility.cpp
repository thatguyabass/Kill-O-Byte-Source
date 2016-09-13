// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BotUtility.h"
#include "Powers/Shield/ForceField.h"
#include "Powers/Projectiles/BaseProjectile.h"

bool UBotUtility::TargetCheck(AActor* Self, AActor* Target)
{
	// Get the target's team number 
	int32 TargetTeamNumber = GetTeamNumber(Target);

	// Target is on an invalid team
	if (TargetTeamNumber == -1)
	{
		TargetTeamNumber = 5;
		//return false;
	}

	// now get the owners team number
	int32 SelfTeamNumber = GetTeamNumber(Self);

	if (SelfTeamNumber != TargetTeamNumber)
	{
		return true;
	}

	return false;
}

int32 UBotUtility::GetTeamNumber(AActor* Actor)
{
	ABot* Bot = BotCast(Actor);
	if (Bot)
	{
		return Bot->TeamNumber;
	}

	ATurret* Turret = TurretCast(Actor);
	if (Turret)
	{
		return Turret->TeamNumber;
	}

	ASnakeLink* Link = LinkCast(Actor);
	if (Link)
	{
		return Link->GetTeamNumber();
	}

	AForceField* Field = Cast<AForceField>(Actor);
	if (Field)
	{
		return Field->GetTeamNumber();
	}

	ABaseProjectile* Projectile = Cast<ABaseProjectile>(Actor);
	if (Projectile)
	{
		return Projectile->GetTeamNumber();
	}

	return -1;
}

void UBotUtility::GetTeamBasedTargets(AActor* Owner, TArray<AActor*>& Out, float SearchRadius, FCollisionQueryParams& Params, FCollisionObjectQueryParams& ObjParams)
{
	TArray<FOverlapResult> OverlapOut;

	Owner->GetWorld()->OverlapMultiByObjectType(OverlapOut, Owner->GetActorLocation(), FQuat::Identity, ObjParams, FCollisionShape::MakeSphere(SearchRadius), Params);

	for (int32 c = 0; c < OverlapOut.Num(); c++)
	{
		AActor* Actor = OverlapOut[c].GetActor();
		//Self Check
		if (Actor && Actor != Owner)
		{
			//Team Check
			bool Check = TargetCheck(Owner, Actor);
			if (Check)
			{
				Out.Add(Actor);
			}
		}
	}
}

void UBotUtility::SortTargets(AActor* Owner, TArray<AActor*>& Out)
{
	TArray<AActor*> PossibleTargets;

	for (int32 c = 0; c < Out.Num(); c++)
	{
		AActor* Actor = Out[c];
		//Self Check
		if (Actor && Actor != Owner)
		{
			//Team Check
			bool Check = TargetCheck(Owner, Actor);
			if (Check)
			{
				PossibleTargets.Add(Actor);
			}
		}
	}

	Out.Empty();

	Out = PossibleTargets;
}

AActor* UBotUtility::GetClosestTarget(AActor* Owner, TArray<AActor*>& Targets)
{
	float Max = FLT_MAX;
	float Distance;
	int32 Index = 0;

	for (int32 c = 0; c < Targets.Num(); c++)
	{
		//Check Line of sight
		Distance = (Targets[c]->GetActorLocation() - Owner->GetActorLocation()).Size();
		if (Distance < Max)
		{
			Max = Distance;
			Index = c;
		}
	}

	if (Targets.IsValidIndex(Index))
	{
		return Targets[Index];
	}

	return nullptr;
}

ATurret* UBotUtility::TurretCast(AActor* Turret)
{
	return Cast<ATurret>(Turret);
}

ABot* UBotUtility::BotCast(AActor* Bot)
{
	return Cast<ABot>(Bot);
}

ASnakeLink* UBotUtility::LinkCast(AActor* Link)
{
	return Cast<ASnakeLink>(Link);
}