// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"
#include "Engine/DataAsset.h"
#include "PlayerAttackTypeAsset.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API UPlayerAttackTypeAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPlayerAttackTypeAsset(const class FObjectInitializer& PCIP);

	UPROPERTY(EditAnywhere, Category = "Data")
	TArray<FAttackType> Data;
	
};
