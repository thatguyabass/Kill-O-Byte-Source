// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlayerState/SnakePlayerState.h"
#include "GameFramework/GameMode.h"
#include "BaseGameMode.generated.h"

USTRUCT(BlueprintType)
struct FGameModeInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Info")
	int32 Index;

	UPROPERTY(BlueprintReadOnly, Category = "Info")
	bool bTeam;

	bool IsValid()
	{
		return Index != -1;
	}

	FGameModeInfo()
	{
		Index = -1;
		bTeam = false;
	}
};

/**
 * 
 */
UCLASS(config=Game)
class SNAKE_PROJECT_API ABaseGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ABaseGameMode(const class FObjectInitializer& PCIP);

	virtual void PreInitializeComponents() override;
	
	/** Warm up time before the Match has started */
	virtual void HandleMatchIsWaitingToStart() override;

	/** Start the Current Match */
	virtual void HandleMatchHasStarted() override;

	/** After the New player has finished loging in */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/** End the Current Match */
	UFUNCTION(exec)
	virtual void FinishMatch();

	/** Finishes the Match and bumps everyone to the main menu */
	void RequestFinishAndExitToMainMenu();

	/** Restart the Current Level */
	UFUNCTION(BlueprintCallable, Category = "Event")
	void TryAgain();

	/** Message the Player controller to show the score screen */
	UFUNCTION()
	void TriggerEndgameScorescreen(int32 Score);

	/** Track Killed Links */
	UFUNCTION()
	void LinkKilled(ASnakePlayerState* Killer);

	/** Track Killed Snakes. Send a Message to all players */
	UFUNCTION()
	virtual void SnakeKilled(ASnakePlayerState* Killer, ASnakePlayerState* Victim);

	/** Track the Bots killed. 
	*	@Param Points	because there are a veriety of bots, they each hold their own points value
	*/
	UFUNCTION()
	void BotKilled(float Points, ASnakePlayerState* Killer);

	/** Did this player win? */
	UFUNCTION()
	virtual	bool IsWinner(ASnakePlayerState* PlayerState) const;

	/** Determine who won the match */
	UFUNCTION()
	virtual void DetermineWinner();

	/** Find the best start point for player */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;

	UFUNCTION()
	class ACheckpoint* GetActiveCheckpoint();

protected:
	/** Find all the Check points in the level */
	void FindAllCheckpoints();

	UFUNCTION()
	void SetActiveCheckpoint(class ACheckpoint* Checkpoint);

	UPROPERTY()
	TArray<class ACheckpoint*> Checkpoints;

	/** current active start point */
	UPROPERTY()
	class ACheckpoint* ActiveCheckpoint;

public:
	/** Score Getters */
	int32 GetSnakePoints() const;
	int32 GetLinkPoints() const;
	int32 GetDeathPoints() const;

	UFUNCTION(BlueprintPure, Category = "GameMode|Level")
	virtual int32 GetLevelIndex() const;

protected:
	virtual void InitGameState() override;

	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const TSharedPtr<const FUniqueNetId>& UniqueId, const FString& Options, const FString& Portal) override;

	/** Game Mode Data */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "GameMode")
	int32 GameModeID;
	//FGameModeInfo GameModeInfo;

	/** Return the Custom Game Session */
	virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;

	/** How long after the first person has joined until the match starts */
	UPROPERTY()
	int32 WarmupTime;

	/** the total duration of the match */
	UPROPERTY()
	int32 RoundTime;

	/** Duration after the match before anything happens */
	UPROPERTY()
	int32 PostGameTime;

	/** Points the player will receive after killing a snake */
	UPROPERTY()
	int32 SnakePoints;

	/** Points the players will recieve after killing a link*/
	UPROPERTY()
	int32 LinkPoints;

	/** Points the players will recieve after dieing */
	UPROPERTY()
	int32 DeathPoints;

	/** Points the player will recieve after killing a Bot */
	UPROPERTY()
	int32 BotKillPoints;

public:
	/** Travel to the Lobby Map */
	void TravelToLobby();

	/** Travel to the Singleplayer Scorescreen */
	UFUNCTION(BlueprintCallable, Category = "Event")
	void TravelToScorescreen();

	/** By pass the menu and travel straight to the next level. */
	UFUNCTION(BlueprintCallable, Category = "Event")
	virtual void TravelToNextLevel();

	/** If false, the next level can't be traveled too */
	UFUNCTION(BlueprintPure, Category = "Event")
	virtual bool CanEnterNextLevel() const;

protected:
	/** Level List Data Asset */
	class ULevelListDataAsset* LevelListDataAsset;

public:
	UFUNCTION()
	class UUserData* GetSavedData();

public:
	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

};
