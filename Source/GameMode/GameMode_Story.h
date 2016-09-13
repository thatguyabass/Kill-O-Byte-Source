// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameMode/BaseGameMode.h"
#include "GameMode_Story.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AGameMode_Story : public ABaseGameMode
{
	GENERATED_BODY()
	
public:
	AGameMode_Story(const class FObjectInitializer& PCIP);

	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const TSharedPtr<const FUniqueNetId>& UniqueId, const FString& Options, const FString& Portal) override;

	/** Trigger the game is over and the player has won! */
	UFUNCTION(Exec, BlueprintCallable, Category = "GameMode|Match")
	void TriggerPlayerWon();

	/** By pass the menu and travel straight to the next level. */
	virtual void TravelToNextLevel() override;

	/** If false, the next level can't be traveled too */
	virtual bool CanEnterNextLevel() const;
	
	//UFUNCTION(BlueprintPure, Category = "GameMode|Level")
	virtual int32 GetLevelIndex() const override;

	virtual void SnakeKilled(ASnakePlayerState* Killer, ASnakePlayerState* Victim) override;

	/** bLevelCompleted Getter. If true, the level has been completed before */
	UFUNCTION(BlueprintPure, Category = "GameMode")
	bool GetLevelCompleted() const;

	/** bRepeat Tutorial Getter */
	UFUNCTION(BlueprintPure, Category = "GameMode")
	bool GetRepeatTutorial() const;

protected:
	/** Score Data Asset */
	UPROPERTY()
	class UScoreDataAsset* ScoreDataAsset;

	/** Level Completed Flag */
	//UPROPERTY()
	static bool bLevelCompleted;

	/** Repeat Tutorial Flag */
	//UPROPERTY()
	static bool bRepeatTutorial;

};
