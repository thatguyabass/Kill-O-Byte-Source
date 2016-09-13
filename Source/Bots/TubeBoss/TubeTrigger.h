// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TubeTrigger.generated.h"

UCLASS()
class SNAKE_PROJECT_API ATubeTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATubeTrigger();

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UBoxComponent* BoxComponent;

	void FireTurretsInOverlap();
	
};
