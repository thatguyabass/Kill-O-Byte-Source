// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HUD/Controllers/MenuInfo.h"
#include "Blueprint/UserWidget.h"
#include "SnakeCharacter/MasterController.h"
#include "MenuController.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AMenuController : public AMasterController
{
	GENERATED_BODY()
	
public:
	AMenuController(const class FObjectInitializer& PCIP);

	UPROPERTY(BlueprintReadWrite, Category = "Menu Item")
	UUserWidget* MenuWidget;

	/** The Lobby Map Name */
	UPROPERTY(EditAnywhere, Category = "Menu Navigation")
	FString LobbyName;

	/** The Lobby Game Mode Type. */
	UPROPERTY(EditAnywhere, Category = "Menu Navigation")
	FString LobbyGameType;

	/** Add the Menu to the Viewport, if it failed, repeat until created */
	void InitializeMenu();

	/** Remove the Menu from the viewport, if failed, repeat until destroyed */
	void DestroyMenu();

	/** Blueprint Event which adds the Menu UMG to the viewport */
	UFUNCTION(BlueprintImplementableEvent, Category = "Menu Control")
	void AddMenu();

	/** Blueprint Event that removes the Menu UMG from the Viewport */
	UFUNCTION(BlueprintImplementableEvent, Category = "Menu Control")
	void RemoveMenu();

	/** Start a single player game 
	*	@Param name of the map that is being loaded. 
	*/
	UFUNCTION(BlueprintCallable, Category = "Single Player")
	void StartGame(int32 LevelIndex, bool bArcade = false);

	UFUNCTION(BlueprintCallable, Category = "Host Game")
	void HostGame();

	UFUNCTION(BlueprintCallable, Category = "Host Game")
	void SetLAN(FString InString);

	/** Is this a LAN game? */
	UPROPERTY(BlueprintReadOnly, Category = "Match Settings")
	bool bIsLanMatch;

	UPROPERTY(BlueprintReadOnly, Category = "Menu Info")
	AMenuInfo* Info;

	UPROPERTY(EditAnywhere, Category = "Menu Info")
	class ULevelListDataAsset* StoryLevelListDataAsset;

	UPROPERTY(EditAnywhere, Category = "Menu Info")
	class ULevelListDataAsset* ArcadeLevelListDataAsset;

	/** Create the SharedThis Object which checks for Online Privileges */
	TSharedPtr<class FOnlineHelper> OnlineHelper;
	void CreateOnlineHelper();
	void DestroyOnlineHelper();

	/** Join Server */
	UFUNCTION(BlueprintCallable, Category = "Join Game")
	void OnJoinServer();

private:
	FTimerHandle Add_TimerHandle;
	FTimerHandle Remove_TimerHandle;

	/** Sort through all the Actors in the Scene to find the Menu Info Actor */
	void FindMenuInfo();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	virtual bool InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamePad) override;

	UFUNCTION()
	void StartGame_Internal(int32 LevelIndex, bool bArcade);

	UPROPERTY()
	int32 InternalLevelIndex;

	UPROPERTY()
	bool bInternalArcadeFlag;

	/** Fade the Audio before traveling to the next level to prevent a hard cut off. */
	UPROPERTY()
	bool bAudioFadeInprogress;

	UPROPERTY()
	float AudioFadeDuration;

	UPROPERTY()
	float AudioFadeProgress;

	class ABackgroundMusic* GetBackgroundMusicActor();

public:
	UFUNCTION(BlueprintCallable, Category = "Demo")
	void PlayDemoMovie();

	UFUNCTION(BlueprintCallable, Category = "Demo")
	void SetPlayDemo(bool bInFlag);

	UFUNCTION(BlueprintPure, Category = "Demo")
	bool GetPlayDemo();

protected:
	static bool bPlayDemo;

};
