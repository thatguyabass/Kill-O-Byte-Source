// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "PointOfInterest.h"


// Sets default values
APointOfInterest::APointOfInterest()
{
	BotsAtLocation = 0;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void APointOfInterest::IncreaseAtLocation()
{
	BotsAtLocation++;
}

void APointOfInterest::ReduceAtLocation()
{
	BotsAtLocation--;
}

int32 APointOfInterest::GetAtLocation()
{
	return BotsAtLocation;
}