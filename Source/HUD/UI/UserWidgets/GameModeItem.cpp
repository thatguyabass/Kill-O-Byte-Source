// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "GameModeItem.h"

UGameModeItem::UGameModeItem(const FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

void UGameModeItem::SetData(FGameModeData InData)
{
	Data = InData;
}

FGameModeData& UGameModeItem::GetData()
{
	return Data;
}

FString UGameModeItem::GetGameModeName()
{
	return Data.GameModeName;
}

FString UGameModeItem::GetGameModeShortHand()
{
	return Data.GameModeShortHand;
}