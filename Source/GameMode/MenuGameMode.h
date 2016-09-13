// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Snake_ProjectGameMode.h"
#include "MenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AMenuGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AMenuGameMode(const class FObjectInitializer& PCIP);

	/** After the player has finished loggin in */
	virtual void PostLogin(APlayerController* InPlayer) override;

	/** Spawning of the player pawn */
	virtual void RestartPlayer(class AController* NewPlayer) override;

	/** Get the Custom Game Session Class */
	virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;

};
