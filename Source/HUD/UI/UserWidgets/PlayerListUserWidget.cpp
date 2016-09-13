// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "PlayerListUserWidget.h"
#include "GameMode/PlayerState/SnakePlayerState.h"
#include "GameMode/GameState/SnakeGameState.h"

UPlayerListUserWidget::UPlayerListUserWidget(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	PlayerIndex = 0;
}

FString UPlayerListUserWidget::GetPlayerNameString()
{
	FString Name = TEXT("");

	ASnakePlayerState* PlayerState = GetPlayerState();
	if (PlayerState)
	{
		Name = PlayerState->PlayerName;
	}

	return Name;
}

FString UPlayerListUserWidget::GetTeamID()
{
	FString TeamID = TEXT("");
	ASnakePlayerState* PlayerState = GetPlayerState();
	if (PlayerState)
	{
		// Remove the 0 based ID. 0 = Team 1
		TeamID = FString::FromInt(PlayerState->GetTeamNumber() + 1);
	}

	return TeamID;
}

FLinearColor UPlayerListUserWidget::GetTeamPrimaryColor()
{
	FLinearColor Color = FColor::White;

	ASnakePlayerState* PlayerState = GetPlayerState();
	if (PlayerState)
	{
		//Get the Primary Color from the player state
		Color = PlayerState->GetTeamColor().PrimaryColor;
	}

	return Color;
}

bool UPlayerListUserWidget::IsLocalPlayer()
{
	ASnakePlayerState* PlayerState = GetPlayerState();
	if (PlayerState)
	{
		APlayerController* Controller = GetOwningPlayer();
		if (Controller && PlayerState == Controller->PlayerState)
		{
			return true;
		}
	}

	return false;
}

ASnakePlayerState* UPlayerListUserWidget::GetPlayerState()
{
	ASnakeGameState* GameState = Cast<ASnakeGameState>(GetWorld()->GameState);
	TArray<FRankMapStruct> PlayerRanks;
	if (GameState)
	{
		GameState->GetPlayerRankStruct(GameState->AllPlayers, PlayerRanks);
		if (PlayerRanks.IsValidIndex(PlayerIndex))
		{
			return Cast<ASnakePlayerState>(PlayerRanks[PlayerIndex].PlayerState);
		}
	}

	return nullptr;
}