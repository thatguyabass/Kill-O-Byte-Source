// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SnakePlayerState.h"
#include "GameMode/GameState/SnakeGameState.h"

ASnakePlayerState::ASnakePlayerState(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	Lives = 0;
	TeamNumber = 0;
	SnakeKillsCount = 0;
	LinkKillsCount = 0;
	DeathCount = 0;
	BotKillsCount = 0;
	bQuitter = false;
}

void ASnakePlayerState::Reset()
{
	Super::Reset();

	Lives = 0;
	SnakeKillsCount = 0;
	LinkKillsCount = 0;
	DeathCount = 0;
	BotKillsCount = 0;
	bQuitter = false;
}

void ASnakePlayerState::UnregisterPlayerWithSession()
{
	if (!bFromPreviousLevel)
	{
		Super::UnregisterPlayerWithSession();
	}
}

void ASnakePlayerState::ClientInitialize(class AController* InController)
{
	Super::ClientInitialize(InController);
}

void ASnakePlayerState::SetTeamNumber(int32 NewTeamNumber)
{
	TeamNumber = NewTeamNumber;
}

void ASnakePlayerState::SetTeamColor(FTeamColorData InData)
{
	TeamColor = InData;
}

void ASnakePlayerState::SetQuitter(bool bIsQuitter)
{
	bQuitter = bIsQuitter;
}

void ASnakePlayerState::SetPlayerLives(int32 InLives)
{
	Lives = InLives;
}

void ASnakePlayerState::ReducePlayerLives(int32 Value)
{
	Lives -= Value;
}

void ASnakePlayerState::IncreasePlayerLives(int32 Value)
{
	Lives += Value;
}

void ASnakePlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	ASnakePlayerState* Player = Cast<ASnakePlayerState>(PlayerState);
	if (Player)
	{
		Player->TeamNumber = TeamNumber;
		Player->TeamColor = TeamColor;
	}
}

int32 ASnakePlayerState::GetPlayerLives() const
{
	return Lives;
}

int32 ASnakePlayerState::GetTeamNumber() const
{
	return TeamNumber;
}

FTeamColorData ASnakePlayerState::GetTeamColor() const
{
	return TeamColor;
}

int32 ASnakePlayerState::GetSnakeKills() const
{
	return SnakeKillsCount;
}

int32 ASnakePlayerState::GetLinkKills() const
{
	return LinkKillsCount;
}

int32 ASnakePlayerState::GetDeaths() const
{
	return DeathCount;
}

int32 ASnakePlayerState::GetBotKills() const
{
	return BotKillsCount;
}

int32 ASnakePlayerState::GetPickups() const
{
	return PickupCount;
}

int32 ASnakePlayerState::GetMiscScore() const
{
	return MiscCount;
}

float ASnakePlayerState::GetScore() const
{
	return Score;
}

bool ASnakePlayerState::IsQuitter() const
{
	return bQuitter;
}

void ASnakePlayerState::ScoreSnakeKill(int32 Points)
{
	SnakeKillsCount++;
	AddScore(Points);
}

void ASnakePlayerState::ScoreLinkKill(int32 Points)
{
	LinkKillsCount++;
	AddScore(Points);
}

void ASnakePlayerState::ScoreDeath(int32 Points)
{
	DeathCount++;
	AddScore(Points);
}

void ASnakePlayerState::ScoreBotKill(int32 Points)
{
	BotKillsCount++;
	AddScore(Points);
}

void ASnakePlayerState::ScoreMisc(int32 Points)
{
	MiscCount += Points;
	AddScore(Points);
}

void ASnakePlayerState::ScorePickup(int32 Points)
{
	PickupCount++;
	AddScore(Points);
}

void ASnakePlayerState::AddScore(int32 Points)
{
	ASnakeGameState* const MyGameState = Cast<ASnakeGameState>(GetWorld()->GetGameState());
	if (MyGameState && TeamNumber >= 0)
	{
		if (TeamNumber >= MyGameState->TeamScores.Num())
		{
			MyGameState->TeamScores.AddZeroed(TeamNumber - MyGameState->TeamScores.Num() + 1);
		}
		MyGameState->TeamScores[TeamNumber] += Points;
	}

	Score += Points;
}

void ASnakePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASnakePlayerState, TeamNumber);
	DOREPLIFETIME(ASnakePlayerState, TeamColor);
	DOREPLIFETIME(ASnakePlayerState, SnakeKillsCount);
	DOREPLIFETIME(ASnakePlayerState, LinkKillsCount);
	DOREPLIFETIME(ASnakePlayerState, DeathCount);
	DOREPLIFETIME(ASnakePlayerState, BotKillsCount);
	DOREPLIFETIME(ASnakePlayerState, PickupCount);
}