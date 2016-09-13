// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SeekerAI.h"
#include "Bots/Bot/SeekerBot.h"
#include "BotUtility.h"
#include "Powers/AttackHelper.h"

ASeekerAI::ASeekerAI(const FObjectInitializer& PCIP)
	: AGruntAI(PCIP)
{
	AccelerationDistance = 1500.0f;
	AcceptableDistance = 150.0f;
	SpeedModifier = 3.0f;

	ExplodeDelay = 2.5f;
}

void ASeekerAI::FindTarget()
{
	ABot* MyBot = Cast<ABot>(GetPawn());
	if (!MyBot)
	{
		return;
	}

	const float SearchRadius = MyBot->SearchRadius;

	FName AISearch = FName(TEXT("AISearch"));
	FCollisionQueryParams Params(AISearch, false, this);

	FCollisionObjectQueryParams ObjParams = GetObjectQuery();

	TArray<AActor*> PossibleTargets;
	UBotUtility::GetTeamBasedTargets(MyBot, PossibleTargets, SearchRadius, Params, ObjParams);

	AActor* ClosestTarget = UBotUtility::GetClosestTarget(MyBot, PossibleTargets);

	SetTarget(ClosestTarget);
}

void ASeekerAI::Explode()
{
	ASeekerBot* Seeker = Cast<ASeekerBot>(GetPawn());
	if (Seeker == nullptr)
	{
		return;
	}

	FName TestMove = FName(TEXT("TestMove"));
	FCollisionQueryParams Params(TestMove, false, this);
	FCollisionObjectQueryParams ObjParams = GetObjectQuery();
	TArray<FOverlapResult> OverlapOut;

	GetWorld()->OverlapMultiByObjectType(OverlapOut, Seeker->GetActorLocation(), FQuat::Identity, ObjParams, FCollisionShape::MakeSphere(Seeker->ExplosionRadius), Params);

	//Convert the Results to Actors
	TArray<AActor*> OverlapActors;
	for (int32 c = 0; c < OverlapOut.Num(); c++)
	{
		OverlapActors.Add(OverlapOut[c].GetActor());
	}
	OverlapOut.Empty();

	//Loop through the overlaped actors and remove components that belong to the same player.
	//Removes the chances of damaging a player multiple times with one seeker. 
	TArray<ASnakeLink*> PlayersHit;
	for (int32 c = 0; c < OverlapActors.Num(); c++)
	{
		ASnakeLink* Link = Cast<ASnakeLink>(OverlapActors[c]);
		if (Link)
		{
			bool bFound = false;
			ASnakeLink* Head = Link->GetHead();
			for (int32 v = 0; v < PlayersHit.Num(); v++)
			{
				if (Head == PlayersHit[v])
				{
					bFound = true;
					break;
				}
			}

			if (!bFound)
			{
				PlayersHit.Add(Head);
			}
			else
			{
				OverlapActors.RemoveAt(c);
				c--;
			}
		}
	}

	UBotUtility::SortTargets(Seeker, OverlapActors);
	for (int32 c = 0; c < OverlapActors.Num(); c++)
	{
		FDamageEvent Event = AttackHelper::CreateDamageEvent(Seeker->DamageType, Seeker->GetAttackType());
		OverlapActors[c]->TakeDamage(Seeker->Damage, Event, this, Seeker);
	}

	Seeker->SpawnDeathVFX();

	FDamageEvent WhiteEvent = AttackHelper::CreateDamageEvent(Seeker->DamageType, Seeker->GetAttackType(), true, EAttackMode::Attack05_White);
	Seeker->TakeDamage(100000, WhiteEvent, this, Seeker);
}

bool ASeekerAI::MoveToTarget()
{
	ABot* MyBot = Cast<ABot>(GetPawn());
	if (MyBot == nullptr)
	{
		return true;
	} 

	AActor* Target = GetValue<AActor, UBlackboardKeyType_Object>(TargetKey);
	if (Target)
	{
		const FVector TargetLocation = Target->GetActorLocation();
		const FVector MyLocation = MyBot->GetActorLocation();

		FVector Direction = TargetLocation - MyLocation;
		const float Distance = Direction.Size();

		if (Distance < AccelerationDistance)
		{
			if (BaseMovement <= 0.0f)
			{
				BaseMovement = MyBot->GetCharacterMovement()->MaxWalkSpeed;
				GetWorldTimerManager().SetTimer(Explode_TimerHandle, this, &ASeekerAI::Explode, ExplodeDelay, false);
			}
			MyBot->GetCharacterMovement()->MaxWalkSpeed = BaseMovement * SpeedModifier;
		}

		EPathFollowingRequestResult::Type T = MoveToActor(Target, AcceptableDistance, false, true, true);

		return (T == EPathFollowingRequestResult::AlreadyAtGoal);
	}
	
	return true;
}

FCollisionObjectQueryParams ASeekerAI::GetObjectQuery()
{
	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjParams.AddObjectTypesToQuery(ECC_SnakeTraceChannel);

	return ObjParams;
}

void ASeekerAI::BotDead(ASnakePlayerState* Killer)
{
	//Clear the explode timer to prevent the explosion from happening after the bot has already died. 
	GetWorldTimerManager().ClearTimer(Explode_TimerHandle);
}