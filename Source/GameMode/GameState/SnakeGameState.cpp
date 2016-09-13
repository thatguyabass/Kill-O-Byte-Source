// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SnakeGameState.h"
#include "GameMode/BaseGameMode.h"
#include "SnakeCharacter/SnakePlayer.h"
#include "Online/SnakeInstance.h"
#include "Utility/Utility.h"

const int32 ASnakeGameState::AllPlayers = -1;

ASnakeGameState::ASnakeGameState(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	TeamCount = 0;
	RemainingTime = 0;
	bTimerPaused = false;
}

void ASnakeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASnakeGameState, TeamCount);
	DOREPLIFETIME(ASnakeGameState, TeamScores);
	DOREPLIFETIME(ASnakeGameState, RemainingTime);
	DOREPLIFETIME(ASnakeGameState, bTimerPaused);
	DOREPLIFETIME(ASnakeGameState, GameModeData);
}

void ASnakeGameState::RequestFinishAndExitToMainMenu()
{
	if (AuthorityGameMode)
	{
		//We are the Server, Tell the Game Mode
		ABaseGameMode* const GameMode = Cast<ABaseGameMode>(AuthorityGameMode);
		if (GameMode)
		{
			GameMode->RequestFinishAndExitToMainMenu();
		}
	}
	else
	{
		//We are Client
		USnakeInstance* Instance = Cast<USnakeInstance>(GetGameInstance());
		if (Instance)
		{
			//Remove Split screen
		}

		AMasterController* const Primary = Cast<AMasterController>(GetGameInstance()->GetFirstLocalPlayerController());
		if (Primary)
		{
			check(Primary->GetNetMode() == ENetMode::NM_Client);
			Primary->HandleReturnToMainMenu();
		}
	}
}

void ASnakeGameState::GetPlayerRank(int32 TeamIndex, PlayerRankMap& OutMap)
{
	//Get the players score and sort them
	TMultiMap<int32, ASnakePlayerState*> SortedMaps;
	GetSortedPlayerList(SortedMaps, TeamIndex);
	
	//Add them to the OutMap
	OutMap.Empty();

	int32 Rank = 0;
	for (TMultiMap<int32, ASnakePlayerState*>::TIterator Map(SortedMaps); Map; ++Map)
	{
		OutMap.Add(Rank++, Map.Value());
	}
}

void ASnakeGameState::GetPlayerRankStruct(int32 TeamIndex, TArray<FRankMapStruct>& OutMap)
{
	//Get the players score and sort them
	TMultiMap<int32, ASnakePlayerState*> SortedMaps;
	GetSortedPlayerList(SortedMaps, TeamIndex);

	OutMap.Empty();

	int32 Rank = 0;
	for (TMultiMap<int32, ASnakePlayerState*>::TIterator Map(SortedMaps); Map; ++Map)
	{
		OutMap.Add(FRankMapStruct());
		OutMap.Last().SetData(Rank++, Map.Value());
	}
}

void ASnakeGameState::GetSortedPlayerList(TMultiMap<int32, ASnakePlayerState*>& OutMap, int32 TeamIndex)
{
	//Get the players score and sort them
	for (int32 c = 0; c < PlayerArray.Num(); c++)
	{
		ASnakePlayerState* MyState = Cast<ASnakePlayerState>(PlayerArray[c]);
		if (MyState && (TeamIndex != -1 ? (MyState->GetTeamNumber() == TeamIndex) : true))
		{
			OutMap.Add(FMath::TruncToInt(MyState->GetScore()), MyState);
		}
	}

	//Sort by the Keys
	OutMap.KeySort(TGreater<int32>());
}

int32 ASnakeGameState::GetPlayersOnTeamCount(int32 TeamNumber)
{
	int32 Count = 0;
	for (int c = 0; c < PlayerArray.Num(); c++)
	{
		ASnakePlayerState* State = Cast<ASnakePlayerState>(PlayerArray[c]);
		if (State && State->GetTeamNumber() == TeamNumber)
		{
			Count++;
		}
	}

	return Count;
}