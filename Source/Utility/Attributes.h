// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "Attributes.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class SNAKE_PROJECT_API UAttributes : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UAttributes(const class FObjectInitializer& PCIP);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	int32 BaseHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	int32 BaseDamage;

	/** In Health value
	@ Bypass the assert that will fire if the actor has 0 health. Set to true on objects that Can have 0 health
	*/
	int32 CalculateHealth(const int32 InHealth, bool Bypass = false);
	
	int32 CalculateDamage(const int32 InDamage);

};
