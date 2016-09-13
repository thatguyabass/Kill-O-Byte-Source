// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "LevelListDataAsset.h"

int32 ULevelListDataAsset::LevelIndex = 0;

// Sets default values for this component's properties
ULevelListDataAsset::ULevelListDataAsset(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	//LevelIndex = 0;
}

FMapItemData ULevelListDataAsset::GetMapData(int32 Index)
{
	if (MapData.IsValidIndex(Index))
	{
		return MapData[Index];
	}
	else
	{
		return FMapItemData();
	}
}

FMapItemData ULevelListDataAsset::GetCurrentMapData()
{
	return MapData[LevelIndex];
}

FString ULevelListDataAsset::GetTrueName()
{
	return GetCurrentMapData().TrueName;
}

FString ULevelListDataAsset::GetTrueName(int32 InIndex)
{
	return GetMapData(InIndex).TrueName;
}

void ULevelListDataAsset::SetLevelIndex(int32 Index)
{
	if (MapData.IsValidIndex(Index))
	{
		LevelIndex = Index;
	}
}

int32 ULevelListDataAsset::GetLevelIndex() const
{
	return LevelIndex;
}

int32 ULevelListDataAsset::GetLevelCount() const
{
	return MapData.Num();
}