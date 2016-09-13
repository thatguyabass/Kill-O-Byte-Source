// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "TargetActor.h"


// Sets default values
ATargetActor::ATargetActor()
{
	HeightOffset = 110.0f;
	LineTraceDepth = 500.0f;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ATargetActor::FindGround()
{
	const FVector Up = GetActorUpVector() * LineTraceDepth;
	const FVector Start = GetActorLocation() + Up;
	const FVector End = GetActorLocation()  + (Up * -1);
	const FName TraceName("FindGround");

	FCollisionQueryParams Params(TraceName, false, this);

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params);

	if (Hit.GetActor() != nullptr)
	{
		FVector Loc = Hit.Location;
		Loc.Z += HeightOffset;
		SetActorLocation(Loc);
	}
}