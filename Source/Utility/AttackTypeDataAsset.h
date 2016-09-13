// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"
#include "Engine/DataAsset.h"
#include "AttackTypeDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API UAttackTypeDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UAttackTypeDataAsset(const class FObjectInitializer& PCIP);
	
	UPROPERTY(EditAnywhere, Category = "Data")
	FAttackType Data;

	FLinearColor GetPrimaryColor() const;
	FLinearColor GetSecondaryColor() const;
	EAttackMode GetAttackMode() const;

};
