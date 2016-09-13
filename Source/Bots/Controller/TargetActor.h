// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TargetActor.generated.h"

UCLASS()
class SNAKE_PROJECT_API ATargetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATargetActor();

	UPROPERTY(EditAnywhere, Category = "Properties")
	float HeightOffset;

	UPROPERTY(EditAnywhere, Category = "Properties")
	float LineTraceDepth;

	void FindGround();
	
};
