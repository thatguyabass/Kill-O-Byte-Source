// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "ScoreDataAsset.generated.h"

UENUM(BlueprintType)
enum class EScoreCondition : uint8
{
	Greater,
	Compare,
	Lesser
};

USTRUCT(BlueprintType)
struct FScoreContent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Score")
	EScoreCondition Condition;

	UPROPERTY(EditAnywhere, Category = "Score")
	int32 MinValue;

	UPROPERTY(EditAnywhere, Category = "Score")
	int32 MaxValue;

	FScoreContent()
	{
		Condition = EScoreCondition::Compare;

		MinValue = -1;
		MaxValue = -1;
	}

	FScoreContent(EScoreCondition InCondition)
	{
		Condition = InCondition;

		MinValue = -1;
		MaxValue = -1;
	}
};

USTRUCT(BlueprintType)
struct FScoreContentArray
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Score Array")
	TArray<FScoreContent> ScoreContent;

	FScoreContentArray()
	{
		ScoreContent.Add(FScoreContent(EScoreCondition::Greater));
		ScoreContent.Add(FScoreContent(EScoreCondition::Compare));
		ScoreContent.Add(FScoreContent(EScoreCondition::Lesser));
	}
};

UCLASS()
class SNAKE_PROJECT_API UScoreDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UScoreDataAsset(const class FObjectInitializer& PCIP);
	
	UPROPERTY(EditAnywhere, Category = "Score Data")
	TArray<FScoreContentArray> ScoreContent;
	
	/** Get the Score Content Array At LevelIndex */
	UFUNCTION()
	FScoreContentArray GetScoreContentArray(int32 LevelIndex);

	/** Get the Score Content at Level Index and Content Index */
	UFUNCTION()
	FScoreContent GetScoreContent(int32 LevelIndex, int32 ContentIndex);

	/** Get the Score for the Level using the players deaths */
	UFUNCTION()
	int32 GetScore(int32 LevelIndex, int32 PlayerDeaths);

	UFUNCTION()
	FString BuildString(int32 LevelIndex, int32 ContentIndex);

	UFUNCTION()
	TArray<FString> BuildStrings(int32 LevelIndex);

};
