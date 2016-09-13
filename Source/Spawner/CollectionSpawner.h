// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Spawner/BaseSpawner.h"
#include "CollectionSpawner.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ACollectionSpawner : public ABaseSpawner
{
	GENERATED_BODY()
	
public:
	ACollectionSpawner(const class FObjectInitializer& PCIP);

	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	
};
