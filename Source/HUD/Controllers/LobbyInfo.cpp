// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "LobbyInfo.h"


// Sets default values
ALobbyInfo::ALobbyInfo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bLaunchStarted = false;
	LevelIndex = 0;
	GameModeIndex = 0;

	bReplicates = true;
}

FMapItemData ALobbyInfo::GetMapData(int32 InIndex)
{
	if (MapData.IsValidIndex(InIndex))
	{
		return MapData[InIndex];
	}
	else
	{
		return FMapItemData();
	}
}

FMapItemData ALobbyInfo::GetCurrentMapData()
{
	return MapData[LevelIndex];
}

void ALobbyInfo::SetLevelIndex(int32 NewIndex)
{
	LevelIndex = NewIndex;
}

void ALobbyInfo::SetGameMode(int32 Index)
{
	GameModeIndex = Index;
}

FGameModeData ALobbyInfo::GetGameModeData(int32 Index)
{
	if (GameModeData.IsValidIndex(Index))
	{
		return GameModeData[Index];
	}
	else
	{
		return FGameModeData();
	}
}

FString ALobbyInfo::GetGameModeName() const
{
	return GameModeData[GameModeIndex].GameModeName;
}

FString ALobbyInfo::GetGameModeShortHand() const
{
	return GameModeData[GameModeIndex].GameModeShortHand;
}

FTeamColorData ALobbyInfo::GetColorData(int32 Index)
{
	if (TeamColorData.IsValidIndex(Index))
	{
		return TeamColorData[Index];
	}
	else
	{
		return FTeamColorData();
	}
}

void ALobbyInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyInfo, bLaunchStarted);
	DOREPLIFETIME(ALobbyInfo, LevelIndex);
	DOREPLIFETIME(ALobbyInfo, GameModeIndex);
}