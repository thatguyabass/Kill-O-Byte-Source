// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LobbyInfo.h"
#include "Blueprint/UserWidget.h"
#include "SnakeCharacter/MasterController.h"
#include "LobbyController.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ALobbyController : public AMasterController
{
	GENERATED_BODY()
	
public:
	ALobbyController(const class FObjectInitializer& PCIP);

	UPROPERTY(EditAnywhere, Category = "Lobby Controller")
	float LaunchTime;
	
	/** Update Method */
	virtual void Tick(float DeltaTime) override;

	/** Start the Lobby Session and Bring up the Lobby Widget */
	virtual void ClientStartGame_Implementation() override;
	
	/** End the game and terminate the Session Widget */
	virtual void ClientGameEnded_Implementation(AActor* EndGameFocus, bool bIsWinner);

	/** [Client] - Hide Lobby */
	UFUNCTION(Reliable, Client)
	void ClientHideLobby();
	virtual void ClientHideLobby_Implementation();

	/** Terminate the Lobby GUI before traveling to the new URL */
	virtual void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;

	virtual void SetupInputComponent() override;

	/** Lobby Widget */
	UPROPERTY(BlueprintReadWrite, Category = "Lobby Control")
	UUserWidget* LobbyMenu;

	/** Add the Blueprint Widget to the Viewport. Called via blueprints */
	UFUNCTION(BlueprintNativeEvent, Category = "Lobby Control")
	void AddLobby();
	virtual void AddLobby_Implementation();

	/** Remove the Lobby from the Viewport. Called Via Blueprints */
	UFUNCTION(BlueprintImplementableEvent, Category = "Lobby Control")
	void RemoveLobby();

	/** Start the count to game start */
	UFUNCTION(BlueprintCallable, Category = "Lobby Control")
	void StartLaunch(FString InMapName);

	/** Start the Game */
	void StartGame();

	/** Return to the main menu */
	UFUNCTION(BlueprintCallable, Category = "Lobby Control")
	void BackToMenu();

	/** Get the Launch Time */
	UFUNCTION(BlueprintCallable, Category = "Lobby Control")
	FString GetLaunchTime();

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Lobby Control")
	ALobbyInfo* Info;

	UFUNCTION(BlueprintCallable, Category = "Lobby Control")
	void SetTeamID(int32 TeamNumber);

	/** Check if the lobby is currently up and Attempt to add the lobby to the viewport. Repeat until the Lobby is in the viewport */
	void AddLobbySlowConnect();

protected:
	/** Timer Handle for the Timer Delegate */
	FTimerHandle Launch_TimeHandle;

	/** Timer Handle for hte Lobby Refresh */
	FTimerHandle Refresh_TimerHandle;

	/** Find the Lobby info in this Map */
	void FindLobbyInfo();

	/** Is the Lobby being displayed? */
	bool bLobbyUp;

	/** Game Map URL */
	FString TravelURL;

	/** Map Name */
	FString MapName;

	/** Has Launch Started? */
	bool bLaunchStarted;

	/** Get the Current Game Mode */
	FString GetGameModeType() const;

	UFUNCTION(REliable, Server, WithValidation)
	void ServerSetTeamID(int32 TeamNumber);
	virtual void ServerSetTeamID_Implementation(int32 TeamNumber);
	virtual bool ServerSetTeamID_Validate(int32 TeamNumber) { return true; }

};
