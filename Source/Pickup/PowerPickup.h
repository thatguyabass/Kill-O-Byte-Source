// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"
#include "Pickup/Pickup.h"
#include "PowerPickup.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API APowerPickup : public APickup
{
	GENERATED_BODY()

public:
	APowerPickup(const class FObjectInitializer& PCIP);

	UPROPERTY(EditAnywhere, Category = "Pickup|PowerType")
	EPowerType PowerType;

	virtual void GiveTo(class ASnakeLink* Link) override;
	
};
