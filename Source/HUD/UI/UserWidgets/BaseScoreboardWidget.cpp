// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BaseScoreboardWidget.h"

#include "GameMode/GameState/SnakeGameState.h"
#include "GameMode/PlayerState/SnakePlayerState.h"
#include "ScoreboardItem.h"

void UBaseScoreboardWidget::GeneratePlayerList(int32 Index)
{
	ASnakeGameState* GameState = Cast<ASnakeGameState>(GetWorld()->GameState);
	if (GameState)
	{
		PlayerList.Empty();
		GameState->GetPlayerRankStruct(Index, PlayerList);
	}
}

void UBaseScoreboardWidget::AddChildren(UVerticalBox* InBox, UWidget* NewItem)
{
	UVerticalBoxSlot* Slot = InBox->AddChildToVerticalBox(NewItem);
	Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
}

void UBaseScoreboardWidget::UpdateScoreboard(int32 TeamNumber)
{
	GeneratePlayerList(TeamNumber);

	ScoreboardInfo.FillOrder();

	for (int32 c = 0; c < Boxes.Num(); c++)
	{
		UVerticalBox* Box = Boxes[c];
		int32 UpdateIndex = ScoreboardInfo.UpdateOrder[c];

		for (int32 v = 0; v < Box->GetChildrenCount() - 1; v++)
		{
			UScoreboardItem* Item = Cast<UScoreboardItem>(Box->GetChildAt(v + 1));
			Item->SetPlayerRank(PlayerList[v]);

			UpdateItem(Item, UpdateIndex);
		}
	}
}

void UBaseScoreboardWidget::UpdateItem(UScoreboardItem* InItem, int32 UpdateIndex)
{
	FRankMapStruct& Rank = InItem->PlayerRank;

	switch (UpdateIndex)
	{
	case FScoreboardUpdateInfo::Rank: InItem->SetText(FString::FromInt(Rank.Index)); break;
	case FScoreboardUpdateInfo::Name: InItem->SetText(Rank.PlayerState->PlayerName); break;
	case FScoreboardUpdateInfo::Link: InItem->SetText(FString::FromInt((int32)Rank.PlayerState->GetLinkKills())); break;
	case FScoreboardUpdateInfo::Kill: InItem->SetText(FString::FromInt((int32)Rank.PlayerState->GetSnakeKills())); break;
	case FScoreboardUpdateInfo::Death: InItem->SetText(FString::FromInt((int32)Rank.PlayerState->GetDeaths())); break;
	case FScoreboardUpdateInfo::Score: InItem->SetText(FString::FromInt((int32)Rank.PlayerState->GetScore())); break;
	}
}

void UBaseScoreboardWidget::CallFillBoxes()
{
	FillBoxes();
}

int32 UBaseScoreboardWidget::GetOwningTeamNumber()
{
	ASnakePlayerState* State = Cast<ASnakePlayerState>(GetOwningPlayer()->PlayerState);
	if (State)
	{
		return State->GetTeamNumber();
	}

	return -1;
}