// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameMode/BaseGameMode.h"
#include "GameMode_DeathMatch.generated.h"

UCLASS()
class SNAKE_PROJECT_API AGameMode_DeathMatch : public ABaseGameMode
{
	GENERATED_BODY()
	
public:
	AGameMode_DeathMatch(const class FObjectInitializer& PCIP);

	ASnakePlayerState* WinningPlayer;

	virtual bool IsWinner(ASnakePlayerState* PlayerState) const override;
	
	virtual void DetermineWinner() override;
};
