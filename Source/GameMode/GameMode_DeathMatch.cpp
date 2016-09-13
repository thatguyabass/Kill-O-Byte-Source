// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "GameMode_DeathMatch.h"
#include "SnakeCharacter/SnakePlayer.h"
#include "GameMode/GameState/SnakeGameState.h"

AGameMode_DeathMatch::AGameMode_DeathMatch(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	//GameModeInfo.Index = 1;
	//GameModeInfo.bTeam = false;

	bDelayedStart = true;
}

bool AGameMode_DeathMatch::IsWinner(ASnakePlayerState* PlayerState) const
{
	return PlayerState && !PlayerState->IsQuitter() && PlayerState == WinningPlayer;
}

void AGameMode_DeathMatch::DetermineWinner()
{
	float Score = -1;
	float HighestScore = -1;

	ASnakePlayerState* Highest = nullptr;

	for (FConstControllerIterator Player = GetWorld()->GetControllerIterator(); Player; Player++)
	{
		ASnakePlayerState* State = Cast<ASnakePlayerState>((*Player)->PlayerState);
		if (State)
		{
			Score = State->GetScore();
			if (Score > HighestScore)
			{
				HighestScore = Score;
				Highest = State;
			}
		}
	}

	WinningPlayer = Highest;
}