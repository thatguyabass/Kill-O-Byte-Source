// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "WallerAI.h"
#include "Bots/Bot/Bot.h"
#include "Boundary/Boundary.h"
#include "Components/MoveToLocation.h"
#include "TargetActor.h"

AWallerAI::AWallerAI(const FObjectInitializer& PCIP)
	: AGruntAI(PCIP)
{
	MinPillars = 3;
	MaxPillars = 10;
	StartDelay = 0.2f;

	bHasBeenInitialized = false;
	bDead = false;
}

void AWallerAI::BeginPlay()
{
	Super::BeginPlay();

	InitializePillars();
}

void AWallerAI::InitializePillars()
{
	if (!PillarClass || !TargetClass)
	{
		return;
	}

	if (!GetPawn())
	{
		//Recall this method if the pawn is null.
		GetWorldTimerManager().SetTimer(Initialize_TimerHandle, this, &AWallerAI::InitializePillars, 0.2f, false);
		return;
	}

	const FVector Location = GetPawn()->GetActorLocation();
	const FRotator Rotation = GetPawn()->GetActorRotation();

	Target = GetWorld()->SpawnActor<ATargetActor>(TargetClass, Location, Rotation);
	MoveTarget();
	SetTarget(Target);

	for (int32 c = 0; c < MaxPillars; c++)
	{
		FVector Pos =  Location + SpawnOffset;
		ABoundary* Wall = GetWorld()->SpawnActor<ABoundary>(PillarClass, Pos, Rotation);
		GetPawn()->MoveIgnoreActorAdd(Wall);

		TArray<UActorComponent*> Components = Wall->GetComponents();
		for (int32 i = 0; i < Components.Num(); i++)
		{
			UMoveToLocation* Loc = Cast<UMoveToLocation>(Components[i]);
			if (Loc)
			{
				Loc->StartDelay = StartDelay * c;
				Loc->OnStart.BindUObject(this, &AWallerAI::StartWallMovement);
				MoveToComponents.Add(Loc);

				//Start the MoveToLocation Component
				Loc->Start();
			}
		}
	}

	bHasBeenInitialized = true;
}

void AWallerAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HealthCheck();

	for (int32 c = 0; c < MoveToComponents.Num(); c++)
	{
		UMoveToLocation* Loc = MoveToComponents[c];
		if (Loc && Loc->HasStarted() && Loc->IsCompleted())
		{
			MoveWall(Loc);
		}
	}
}

void AWallerAI::MoveWall(UMoveToLocation* MoveTo)
{
	if (MoveTo)
	{
		const FVector Location = GetPawn()->GetActorLocation() + SpawnOffset;

		MoveTo->MoveActorAndWaypoints(Location, true);
	}
}

void AWallerAI::StartWallMovement(UMoveToLocation* Caller)
{
	if (Caller)
	{
		MoveWall(Caller);
	}
}

void AWallerAI::MoveTarget()
{
	if (Target)
	{
		FVector Location = GetPawn()->GetActorLocation();
		FVector Right = GetPawn()->GetActorRightVector();

		int32 Direction = FMath::RandRange(0, 1);
		float Delta = Distance;
		if (Direction != 0)
		{
			Delta *= -1;
		}

		Location += (Right * Delta);
		const FRotator Rotation = GetPawn()->GetActorRotation();

		GetWorld()->FindTeleportSpot(Target, Location, Rotation);

		Target->SetActorLocation(Location);
		Target->FindGround();
	}
}

void AWallerAI::HealthCheck()
{
	// Verify that this has initialized OR if We are dead
	if (!bHasBeenInitialized || bDead)
	{
		return;
	}

	ASnakePlayerState* Killer = nullptr;

	for (int32 c = 0; c < MoveToComponents.Num(); c++)
	{
		ABoundary* Wall = Cast<ABoundary>(MoveToComponents[c]->GetOwner());
		if(Wall && Wall->IsDead())
		{
			Killer = Wall->GetKiller();
			MoveToComponents.RemoveAt(c);
			c--;//Reset the index for the next item

		}
	}
	if (MoveToComponents.Num() <= MinPillars)
	{
		//This bot has been killed 
		ABot* Bot = Cast<ABot>(GetPawn());
		if (Bot)
		{
			//Apply a ludicrous Amount of damage to kill the bot.
			FDamageEvent Event;
			Bot->TakeDamage(10000, Event, nullptr, Killer);
		}
	}
}

void AWallerAI::BotDead(ASnakePlayerState* Killer)
{
	bDead = true;

	FDamageEvent Event;
	for (int32 c = 0; c < MoveToComponents.Num(); c++)
	{
		ABoundary* Wall = Cast<ABoundary>(MoveToComponents[c]->GetOwner());
		if (Wall)
		{
			Wall->TakeDamage(10000, Event, nullptr, Killer);
		}
	}

	if (Target)
	{
		Target->Destroy();
	}
}