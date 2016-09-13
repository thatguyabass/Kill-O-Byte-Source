// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UserData/UserData.h"
#include "Blueprint/UserWidget.h"
#include "UserData/SnakeLocalPlayer.h"
#include "HUD/Controllers/ScoreDataAsset.h"
#include "HUD/Controllers/ArcadeScore.h"
#include "GameFramework/PlayerController.h"
#include "MasterController.generated.h"

USTRUCT()
struct FOnScreenEffectContainer
{
	GENERATED_BODY()

	UPROPERTY()
	UUserWidget* Widget;

	UPROPERTY()
	int32 ContextIndex;

	bool IsValid()
	{
		return Widget != nullptr;
	}

	bool Compare(int32 InIndex)
	{
		return ContextIndex == InIndex;
	}

	FOnScreenEffectContainer()
	{
		ContextIndex = -1;
		Widget = nullptr;
	}

	FOnScreenEffectContainer(UUserWidget* InWidget, int32 InContextIndex)
	{
		Widget = InWidget;
		ContextIndex = InContextIndex;
	}

	FORCEINLINE bool operator==(const FOnScreenEffectContainer& InCont) const
	{
		return (ContextIndex == InCont.ContextIndex);
	}

};

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AMasterController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMasterController(const class FObjectInitializer& PCIP);
	
	virtual void SetPlayer(UPlayer* InPlayer) override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Reliable, Client)
	virtual void ClientSetSpectatorCamera(FVector Location, FRotator Rotation);
	virtual void ClientSetSpectatorCamera_Implementation(FVector Location, FRotator Rotation);
	
	UFUNCTION(Reliable, Client)
	virtual void ClientStartGame();
	virtual void ClientStartGame_Implementation();

	/** End the game on the client */
	virtual void ClientGameEnded_Implementation(AActor* EndGameFocus, bool bIsWinner);

	UFUNCTION(Reliable, Client)
	void ClientStartOnlineGame();
	virtual void ClientStartOnlineGame_Implementation();

	UFUNCTION(Reliable, Client)
	void ClientEndOnlineGame();
	virtual void ClientEndOnlineGame_Implementation();

	/** Clean up the player controller before returning to the main menu */
	virtual void HandleReturnToMainMenu();

	/** Ends and/or destroys the game session */
	void CleanupSessionOnReturnToMenu();

	/** Return the Client to the Main Menu Gracefully */
	void ClientReturnToMainMenu_Implementation(const FString& ReturnReason) override;

private:
	FTimerHandle StartOnline_TimerHandle;

public:
	UFUNCTION()
	UUserData* GetSavedData() const;

	/** Get the Normal Scores */
	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetNormalScore(int32 LevelIndex, bool bArcade = false);

	/** Set the Normal Scores */
	UFUNCTION(BlueprintCallable, Category = "Score")
	void SetNormalScore(int32 LevelIndex, int32 NormalScore, bool bArcade = false);

	/** Get the Hard Scores */
	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetHardScore(int32 LevelIndex, bool bArcade = false);

	/** Set the Hard Scores */
	UFUNCTION(BlueprintCallable, Category = "Score")
	void SetHardScore(int32 LevelIndex, int32 HardScore, bool bArcade = false);

	/** Set the highest Normal Wave completed */
	UFUNCTION(BlueprintCallable, Category = "Score")
	void SetNormalWave(int32 LevelIndex, int32 InWaveIndex);

	/** Get the Highest wave completed on Normal */
	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetNormalWave(int32 LevelIndex);
	
	/** Set the highest wave completed on Hard  */
	UFUNCTION(BlueprintCallable, Category = "Score")
	void SetHardWave(int32 LevelIndex, int32 InWaveIndex);

	/** Get the highest wave completed on Hard */
	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetHardWave(int32 LevelIndex);

	/** Reset all the Normal Scores to the Default Value */
	UFUNCTION(Exec)
	void ResetNormalScores();

	/** Reset all the Hard Scores to the Default Value */
	UFUNCTION(Exec)
	void ResetHardScores();

	/** Reset all the Normal Arcade Scores */
	UFUNCTION(Exec)
	void ResetArcadeNormalScores();

	/** Reset all the Hard Arcade Scores */
	UFUNCTION(Exec)
	void ResetArcadeHardScores();

	/** Reset all the normal Waves */
	UFUNCTION(Exec)
	void ResetNormalWaveScores();

	/** Reset all the hard waves */
	UFUNCTION(Exec)
	void ResetHardWaveScores();

	/** Reset all Scores */
	UFUNCTION(Exec)
	void ResetAllScores();

	/** Get the Current Saved Difficulty */
	UFUNCTION(BlueprintPure, Category = "Difficulty")
	bool GetDifficulty();

	/** Set the Difficulty
	*	True = Hard
	*	False = Normal */
	UFUNCTION(BlueprintCallable, Category = "Difficulty")
	void SetDifficulty(bool InValue);

	UPROPERTY(EditAnywhere, Category = "ScoreData")
	UScoreDataAsset* ScoreDataAsset;

	UFUNCTION(BlueprintCallable, Category = "ScoreData")
	FScoreContentArray GetScoreContentArray(int32 LevelIndex);

	UFUNCTION(BlueprintPure, Category = "ScorData")
	FScoreContent GetScoreContent(int32 LevelIndex, int32 ContentIndex);

	UFUNCTION(BlueprintPure, Category = "ScoreData")
	FString GetScoreString(int32 levelIndex, int32 ContentIndex);

	UFUNCTION(BlueprintPure, Category = "ScoreData")
	TArray<FString> GetScoreStrings(int32 LevelIndex);

	UFUNCTION(BlueprintCallable, Category = "ScoreData")
	int32 GetScore(int32 LevelIndex, int32 PlayerDeaths);

	UPROPERTY(EditAnywhere, Category = "ScoreData|Arcade")
	UArcadeScore* ArcadeScoreDataAsset;

	/** Get the lowest wave needed for a single star */
	UFUNCTION(BlueprintPure, Category = "ScoreData|Arcade")
	int32 GetArcadeWaveLowest();

	/** Get the medium wave needed for two stars */
	UFUNCTION(BlueprintPure, Category = "ScoreData|Arcade")
	int32 GetArcadeWaveMedium();

	/** Get the Highest Wave Needed for all three stars */
	UFUNCTION(BlueprintPure, Category = "ScoreData|Arcade")
	int32 GetArcadeWaveHighest();

	UFUNCTION(BlueprintPure, Category = "LevelCompleted")
	bool GetLevelCompleted(int32 LevelIndex);

	UFUNCTION(BlueprintCallable, Category = "LevelCompleted")
	void SetLevelCompleted(int32 LevelIndex, bool InValue);

	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void SetRepeatTutorial(bool InValue);

	/** bRepeatTutorial Getter */
	UFUNCTION(BlueprintPure, Category = "Tutorial")
	bool GetRepeatTutorial() const;

	UFUNCTION(Exec)
	void UnlockAllLevels();

	UFUNCTION(Exec)
	void ResetAllLevels();

protected:
	UPROPERTY()
	bool bRepeatTutorial;

private:
	/** Is the game window currently in focus. True */
	UPROPERTY()
	bool bWindowInFocus;

	/** What was the Focus Status last tick */
	UPROPERTY()
	bool bWindowInFocusLastTick;

	/** Handle Ingame Focus */
	void HandleGameFocus();

protected:
	/** Track if the player is in the menu. If true, don't pause or unpause the game when focus lost or gained. */
	UPROPERTY()
	bool bPlayerInMenu;

public:
	UPROPERTY(BlueprintReadWrite, Category = "Player|Movie")
	bool bMoviePlaying;

	/** Widget container. Each widget has an id assigned to it on creation */
	UPROPERTY()
	TArray<FOnScreenEffectContainer> OnScreenEffectWidgets;

	UFUNCTION(BlueprintPure, Category = "Widget Effect")
	UUserWidget* GetWidgetAtContext(int32 ContextIndex);

	/** Find a OnScreenContainer At Context */
	UFUNCTION()
	FOnScreenEffectContainer FindOnScreenContainerWithContext(int32 ContextIndex);

	UFUNCTION(BlueprintCallable, Category = "Widget Effect")
	bool IsWidgetAtContextValid(int32 ContextIndex);

	/** Add a UserWidget to the OnScreenEffect Widget Container */
	UFUNCTION(BlueprintCallable, Category = "Widget Effect")
	int32 AddWidgetToOnScreenEffects(UUserWidget* InWidget);

	/** Remove the Widget At index From the view port and null the Widget */
	UFUNCTION(BlueprintCallable, Category = "Widget Effect")
	void RemoveOnScreenEffectWidget(int32 ContextIndex);

	/** Remove all OnScreenWidgets from the viewport and null the widget */
	UFUNCTION(BlueprintCallable, Category = "Widget Effect")
	void RemoveAllOnScreenEffectWidgets();

	static int32 OnScreenEffectContext;

public:
	UFUNCTION(BlueprintPure, Category = "Player|Control")
	bool GetInputSettings() const;

public:
	UFUNCTION(BlueprintPure, Category = "GameMode|GameOver")
	bool GetShowCredits();

	UFUNCTION(BlueprintCallable, Category = "GameMode|GameOver")
	void SetShowCredits(bool In);

protected:
	static bool bShowCredits;

};
