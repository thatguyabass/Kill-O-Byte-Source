// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup/Pickup.h"
#include "BodyPickup.generated.h"

class ASnakeLink;

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ABodyPickup : public APickup
{
	GENERATED_BODY()

public:
	ABodyPickup(const class FObjectInitializer& PCIP);
	
	UPROPERTY(EditAnywhere, Category = BodyCount)
	int32 BodyCount;

	virtual void GiveTo(class ASnakeLink* Link) override;
};
