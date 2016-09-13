// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup/Pickup.h"
#include "HealthPickup.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AHealthPickup : public APickup
{
	GENERATED_BODY()
	
public:
	AHealthPickup(const class FObjectInitializer& PCIP);
	
	/** Percent of Max Health Restored to the player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Health")
	float HealthPercent;

	/** Give the Health to the Link */
	virtual void GiveTo(class ASnakeLink* Link) override;
	
};
