// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameMode/BaseGameMode.h"
#include "GameMode_Lobby.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AGameMode_Lobby : public ABaseGameMode
{
	GENERATED_BODY()
	
public:
	AGameMode_Lobby(const class FObjectInitializer& PICP);

	/** After a new player has finished logging on */
	virtual void PostLogin(APlayerController* NewPlayer);

	/** A player has loged out */
	virtual void Logout(AController* Exiting) override;

	/** End the Current Match */
	virtual void FinishMatch() override;

	/** Override to not enter the */
	virtual void DefaultTimer();//This has been removed from Game Mode;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

};
