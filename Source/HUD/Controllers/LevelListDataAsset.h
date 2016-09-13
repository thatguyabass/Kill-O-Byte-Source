// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"
#include "Engine/DataAsset.h"
#include "LevelListDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ULevelListDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	ULevelListDataAsset(const FObjectInitializer& PCIP);

	/** Map information */
	UPROPERTY(EditAnywhere, Category = "Map Information")
	TArray<FMapItemData> MapData;

	/** Get the map info at this index */
	UFUNCTION()
	FMapItemData GetMapData(int32 Index);

	/** Return the map info at the Current Index */
	UFUNCTION()
	FMapItemData GetCurrentMapData();

	/** Return the Editor Map Name. Will return teh Current level Index */
	FString GetTrueName();

	/** Return the Editor Map Name. Return at index */
	FString GetTrueName(int32 Index);

	/** Set the Level Index to the Passed Value. The new index is validated before set */
	UFUNCTION()
	void SetLevelIndex(int32 Index);

	UFUNCTION()
	int32 GetLevelIndex() const;

	int32 GetLevelCount() const;

protected:
	static int32 LevelIndex;
	
};
