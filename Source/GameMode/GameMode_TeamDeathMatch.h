// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameMode/BaseGameMode.h"
#include "GameMode_TeamDeathMatch.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AGameMode_TeamDeathMatch : public ABaseGameMode
{
	GENERATED_BODY()
	
public:
	AGameMode_TeamDeathMatch(const class FObjectInitializer& PCIP);
	
	void PostLogin(APlayerController* NewPlayer) override;

	/** Initialize Replicated game Data */
	virtual void InitGameState() override;
	
protected:
	//virtual bool IsTeamSpawnpoint(APlayerStart* StartPoint, AController* Player) const override;

private:

	/** Total number of teams */
	int32 TeamCount;

	/** Best Team */
	int32 WinningTeam;

	/** Check who won */
	virtual void DetermineWinner() override;

	/** Check if this player state was on the winning team */
	virtual bool IsWinner(class ASnakePlayerState* PlayerState) const override;


};
