// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SmartTurret.h"
#include "Bots/Controller/TurretAI.h"

ASmartTurret::ASmartTurret()
	: ATurret()
{
	AIControllerClass = ATurretAI::StaticClass();

	BaseHealth = 2;

	WaitTime = 0.0f;
	bCanMove = false;
	WaypointIndex = 0;

	bReplicateMovement = true;
	NetPriority = 3.0f;
}

void ASmartTurret::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Waypoints.Num() > 0)
	{
		bCanMove = true;
		InitialLocation = GetActorLocation();
		InitializeWaypoints();
	}	
}

void ASmartTurret::InitializeWaypoints()
{
	if (Waypoints.Num() > 0)
	{
		for (int32 c = 0; c < Waypoints.Num(); c++)
		{
			Waypoints[c] += InitialLocation;
		}
	}
}

void ASmartTurret::BeginPlay()
{
	ATurret::BeginPlay();
}

void ASmartTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector ASmartTurret::GetNextWaypoint()
{
	if (bCanMove)
	{
		WaypointIndex++;
		if (WaypointIndex >= Waypoints.Num())
		{
			WaypointIndex = 0;
		}

		return Waypoints[WaypointIndex];
	}

	return GetActorLocation();
}