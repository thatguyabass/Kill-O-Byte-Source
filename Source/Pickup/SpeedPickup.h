// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup/Pickup.h"
#include "SpeedPickup.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ASpeedPickup : public APickup
{
	GENERATED_BODY()
	
public:
	ASpeedPickup(const class FObjectInitializer& PCIP);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|PowerType")
	struct FSpeedPower Speed;
	
	virtual void GiveTo(class ASnakeLink* Link) override;
};
