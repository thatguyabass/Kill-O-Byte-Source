// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "GameMode_TeamDeathMatch.h"
#include "GameState/SnakeGameState.h"

AGameMode_TeamDeathMatch::AGameMode_TeamDeathMatch(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	//GameModeInfo.Index = 1;
	//GameModeInfo.bTeam = true;

	TeamCount = 4;
	bDelayedStart = true;
}

void AGameMode_TeamDeathMatch::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AGameMode_TeamDeathMatch::InitGameState()
{
	Super::InitGameState();

	ASnakeGameState* SnakeGameState = Cast<ASnakeGameState>(GameState);
	if (SnakeGameState)
	{
		SnakeGameState->TeamCount = TeamCount;
		for (int32 c = 0; c < TeamCount; c++)
		{
			SnakeGameState->TeamScores.AddZeroed(c);
		}
	}
}

void AGameMode_TeamDeathMatch::DetermineWinner()
{
	float Score = -1;
	float HighestScore = -1;
	int32 BestTeam = 0;

	ASnakeGameState* MyGameState = Cast<ASnakeGameState>(GameState);
	if (MyGameState)
	{
		for (int32 c = 0; c < MyGameState->TeamCount; c++)
		{
			Score = MyGameState->TeamScores[c];
			if (Score > HighestScore)
			{
				HighestScore = Score;
				BestTeam = c;
			}
		}
	}
	WinningTeam = BestTeam;
}

bool AGameMode_TeamDeathMatch::IsWinner(ASnakePlayerState* PlayerState) const
{
	return PlayerState && !PlayerState->IsQuitter() && PlayerState->GetTeamNumber() == WinningTeam;
}

//bool AGameMode_TeamDeathMatch::IsTeamSpawnpoint(APlayerStart* StartPoint, AController* Player) const
//{
//	return true;
//}