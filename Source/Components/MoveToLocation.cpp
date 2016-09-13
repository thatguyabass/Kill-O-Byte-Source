// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "MoveToLocation.h"


// Sets default values for this component's properties
UMoveToLocation::UMoveToLocation()
{
	bReplicates = true;

	bLoop = true;
	bCompleted = false;
	bPlayAtStart = true;
	bMoveToOverride = false;
	bUseWorldLocation = false;

	StartDelay = 0.0f;

	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UMoveToLocation::InitializeComponent()
{
	Super::InitializeComponent();

	if (Waypoints.Num() > 0)
	{
		if (!bPlayAtStart || StartDelay > 0.0f)
		{
			bCompleted = true;
		}
		else
		{
			bCompleted = false;
		}

		bCanMove = true;
		InitialLocation = GetOwner()->GetActorLocation();
		InitializeWaypoints();
	}
}

void UMoveToLocation::BeginPlay()
{
	Super::BeginPlay();

	if (bPlayAtStart)
	{
		Start();
	}
}

void UMoveToLocation::Start()
{
	if (bCompleted)
	{
		GetOwner()->GetWorldTimerManager().SetTimer(Start_TimerHandle, this, &UMoveToLocation::StartMovement, StartDelay, false);
	}
	else if(bWait)
	{
		Wait();
	}
}

void UMoveToLocation::SetWaypoints(TArray<FVector> InWaypoints)
{
	Waypoints.Empty();

	Waypoints = InWaypoints;
}

void UMoveToLocation::InitializeWaypoints()
{
	InitialWaypoints.Empty();

	if (Waypoints.Num() > 0)
	{
		for (int32 c = 0; c < Waypoints.Num(); c++)
		{
			InitialWaypoints.Add(Waypoints[c]);
			Waypoints[c] += InitialLocation;
		}
	}
}

// Called every frame
void UMoveToLocation::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (!bCompleted)
	{
		MoveTo(DeltaTime);
	}
}

void UMoveToLocation::MoveTo(float DeltaTime)
{
	if (bCanMove && !bWait)
	{
		Progress += DeltaTime;
		if (Progress >= TravelTime)
		{
			GetNextWaypoint();
		}

		if (!bWait)
		{
			float Blend = Progress / TravelTime;

			FVector TargetLocation = GetWaypointLocation();
			FVector L = FMath::Lerp(InitialLocation, TargetLocation, Blend);

			FHitResult Hit(ForceInit);
			GetOwner()->SetActorLocation(L, bSweep, &Hit);

			if (Hit.IsValidBlockingHit() && Hit.GetActor())
			{
				AActor* Other = Hit.GetActor();
				const FVector Normal = Hit.Normal.GetSafeNormal();
				const FVector Deflection = FVector::VectorPlaneProject(L, Normal) * (1.0f - Hit.Time);
				Other->SetActorLocation(Deflection);
			}
		}
	}
}

void UMoveToLocation::GetNextWaypoint()
{
	InitialLocation = GetOwner()->GetActorLocation();

	Progress = 0.0f;

	if (bMoveToOverride)
	{
		bMoveToOverride = false;
		bCompleted = true;
		OnFinished.Broadcast(WaypointIndex);
	}

	WaypointIndex++;
	if (WaypointIndex >= Waypoints.Num())
	{
		WaypointIndex = 0;

		if (!bLoop)
		{
			bCompleted = true;
			OnFinished.Broadcast(WaypointIndex);
		}
	}

	Wait();
	//GetOwner()->ForceNetUpdate();
}

void UMoveToLocation::Wait()
{
	if (WaitTime <= 0.0f)
	{
		EndWait();
		return;
	}

	bWait = true;
	GetOwner()->GetWorldTimerManager().SetTimer(Wait_TimerHandle, this, &UMoveToLocation::EndWait, WaitTime, false);
}

void UMoveToLocation::EndWait()
{
	bWait = false;
}

void UMoveToLocation::MultiCastMoveTo_Implementation(FVector Location)
{
	GetOwner()->SetActorLocation(Location);
}

void UMoveToLocation::StartMovementCycle()
{
	bCompleted = false;
}

void UMoveToLocation::StartMovement()
{
	OnStart.ExecuteIfBound(this);
	StartMovementCycle();
}

void UMoveToLocation::SetNextWaypoint(int32 Index)
{
	if (!bCanMove && !bLoop)
	{
		return;
	}

	bMoveToOverride = true;

	Progress = 0.0f;
	InitialLocation = GetOwner()->GetActorLocation();

	if (Waypoints.IsValidIndex(Index))
	{
		WaypointIndex = Index;
		StartMovementCycle();
	}
}

void UMoveToLocation::MoveToImmediately(int32 Index)
{
	if (bCanMove)
	{
		SetNextWaypoint(Index);
		GetOwner()->SetActorLocation(Waypoints[WaypointIndex]);
		GetNextWaypoint();
	}
}

void UMoveToLocation::MoveActorAndWaypointsDelta(FVector Delta, bool StartCycle)
{
	if (!bCanMove)
	{
		return;
	}

	MoveWaypointsDelta(Delta);

	FVector Location = GetOwner()->GetActorLocation();
	InitialLocation = Location + Delta;

	if (StartCycle)
	{
		StartMovementCycle();
	}
}

void UMoveToLocation::MoveWaypointsDelta(FVector Delta)
{
	if (!bCanMove)
	{
		return;
	}

	for (int32 c = 0; c < Waypoints.Num(); c++)
	{
		Waypoints[c] += Delta;
	}
}

void UMoveToLocation::MoveActorAndWaypoints(FVector NewLocation, bool StartCycle)
{
	if (!bCanMove)
	{
		return;
	}

	MoveWaypoints(NewLocation);

	InitialLocation = NewLocation;

	if (StartCycle)
	{
		StartMovementCycle();
	}
}

void UMoveToLocation::MoveWaypoints(FVector NewLocation)
{
	if (!bCanMove)
	{
		return;
	}

	for (int32 c = 0; c < Waypoints.Num(); c++)
	{
		Waypoints[c] = InitialWaypoints[c] + NewLocation;
	}
}

bool UMoveToLocation::IsCompleted() const
{
	return bCompleted;
}

bool UMoveToLocation::HasStarted() const
{
	return GetOwner()->GetWorldTimerManager().GetTimerRemaining(Start_TimerHandle) <= 0.0f;
}

FVector UMoveToLocation::GetWaypointLocation() const
{
	return bUseWorldLocation ? GetOwner()->GetActorLocation() + Waypoints[WaypointIndex] : Waypoints[WaypointIndex];
}

float UMoveToLocation::GetTravelProgress() const
{
	return Progress;
}