// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameMode/PlayerState/SnakePlayerState.h"
#include "GameFramework/GameState.h"
#include "SnakeGameState.generated.h"


typedef TMap<int32, TWeakObjectPtr<ASnakePlayerState> > PlayerRankMap;

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ASnakeGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	ASnakeGameState(const class FObjectInitializer& PCIP);

	/** number of teams in the current game. This doesn't decrement if a team has no players left on it */
	UPROPERTY(Transient, Replicated)
	int32 TeamCount;

	/** Accumulated Score per team */
	UPROPERTY(Transient, Replicated)
	TArray<int32> TeamScores;

	/** Time left for match warmup AND Match Time*/
	UPROPERTY(Transient, Replicated)
	int32 RemainingTime;

	/** Timer Paused State */
	UPROPERTY(Transient, Replicated)
	bool bTimerPaused;

	/** Index to track which game mode the game is currently using. Used to get the correct scoreboard */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Game Mode Property")
	struct FGameModeInfo GameModeData;

	/** Request that the match is finished and the player/players returned to the main menu */
	void RequestFinishAndExitToMainMenu();

	/** Sort the Teams Players using the TMap */
	void GetPlayerRank(int32 TeamIndex, PlayerRankMap& OutMap);

	/** Sort the Teams Players using the Struct */
	UFUNCTION(BlueprintCallable, Category = "Player List")
	void GetPlayerRankStruct(int32 TeamIndex, TArray<struct FRankMapStruct>& OutMap);

	/** Sort the players based on thier current score */
	void GetSortedPlayerList(TMultiMap<int32, ASnakePlayerState*>& OutMap, int32 TeamIndex = -1);

	/** Get the Current number of players on the team */
	UFUNCTION(BlueprintCallable, Category = "UI Helper")
	int32 GetPlayersOnTeamCount(int32 TeamNumber);

	static const int32 AllPlayers;

};
