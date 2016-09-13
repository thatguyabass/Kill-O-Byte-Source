// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "ScoreboardItem.h"

UScoreboardItem::UScoreboardItem(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	
}

void UScoreboardItem::SetPlayerRank(FRankMapStruct InRank)
{
	PlayerRank = InRank;
}

void UScoreboardItem::SetText(FString NewMessage)
{
	TextMessage = NewMessage;
}

bool UScoreboardItem::IsLocalPlayer()
{
	ASnakeGameState* GameState = Cast<ASnakeGameState>(GetWorld()->GameState);
	if (GameState)
	{
		ASnakePlayerState* PlayerState = Cast<ASnakePlayerState>(GetOwningPlayer()->PlayerState);
		if (PlayerState)
		{
			return PlayerState == PlayerRank.PlayerState;
		}
	}

	return false;
}