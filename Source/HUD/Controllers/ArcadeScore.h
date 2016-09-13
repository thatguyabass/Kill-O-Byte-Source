// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "ArcadeScore.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API UArcadeScore : public UDataAsset
{
	GENERATED_BODY()

public:
	UArcadeScore(const class FObjectInitializer& PCIP);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Score")
	int32 LowestWave;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Score")
	int32 MedWave;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Score")
	int32 HighestWave;
	
public:
	UFUNCTION()
	int32 GetLowestWave();

	UFUNCTION()
	int32 GetMediumWave();

	UFUNCTION()
	int32 GetHighestWave();

	UFUNCTION()
	int32 GetScore(int32 WaveCount);

};
