// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PointOfInterest.generated.h"

UCLASS()
class SNAKE_PROJECT_API APointOfInterest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APointOfInterest();
	
	int32 BotsAtLocation;

	/** Increase the Number of Bots at location */
	void IncreaseAtLocation();

	/** Decrease the Number for BotsAtLocation */
	void ReduceAtLocation();

	/** Get the number of bots at location */
	int32 GetAtLocation();

};
