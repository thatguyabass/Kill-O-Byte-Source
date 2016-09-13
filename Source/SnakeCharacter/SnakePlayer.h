// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HUD/SnakeHUD.h"
#include "MasterController.h"
#include "SnakePlayer.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ASnakePlayer : public AMasterController
{
	GENERATED_BODY()
	
public:
	ASnakePlayer(const class FObjectInitializer& PCIP);

	virtual void SetPlayer(UPlayer* InPlayer);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Possess(class APawn* InPawn) override;
	virtual void UnPossess();

	/** Set up the Menu Input Bindings */
	virtual void SetupInputComponent() override;

	/** update camera when pawn dies */
	virtual void PawnPendingDestroy(APawn* P) override;

	/** Call Server Respawn to Spawn a new Player Character */
	void Respawn();

	/** How long will the player have to wait before respawning */
	UPROPERTY(EditAnywhere, Category = "Player|Respawn")
	float RespawnTime;

	/** How long is the player immune to damage after respawning? */
	UPROPERTY(EditAnywhere, Category = "Player|Respawn")
	float InvulnerableDuration;

	UFUNCTION(BlueprintCallable, Category = "Player|Respawn")
	bool IsPawnInvulnerable() const;

	/** Toggle God Mode */
	UFUNCTION(Exec)
	void SetGodMode();

	/** Activate the Spawn Particle effect and initialize the variables
	*	False = Spawn In
	*	True = Spawn Out */
	UFUNCTION(BlueprintCallable, Category = "Player|Respawn")
	void StartSpawnEffect(bool InDirection);

	UPROPERTY()
	bool bPlaySpawnEffect;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Player|Respawn")
	bool bPawnInvulnerable;

	UPROPERTY()
	float InvulnerableProgress;

	/** Initialize the Invulnerable Phase */
	void StartInvulnerablePhase();

	/** Clean up before stopping the Invulnerable Phase */
	void EndInvulnerablePhase();

public:
	/** Start the game on the client */
	virtual void ClientStartGame_Implementation() override;

	/** End the game on the client */
	virtual void ClientGameEnded_Implementation(AActor* EndGameFocus, bool bIsWinner) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Events")
	void AddHUD();

	UFUNCTION(BlueprintCallable, Category = "UI Events")
	void RemoveHUD();

	/** Add the HUD to the View port */
	UFUNCTION(BlueprintCallable, Category = "UI Events")
	void ShowHUD();

	UFUNCTION(BlueprintCallable, Category = "UI Events")
	void HideHUD();

	UPROPERTY(BlueprintReadWrite, Category = "UI Menus")
	UUserWidget* HUDWidget;

	/** Second HUD Widget. Used for displaying different options. */
	UPROPERTY(BlueprintReadWrite, Category = "UI Menus")
	UUserWidget* AdditionalHUDWidget;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Events")
	void AddInGameMenu();

	UFUNCTION(BlueprintCallable, Category = "UI Events")
	void RemoveInGameMenu();

	UPROPERTY(BlueprintReadWrite, Category = "UI Menus")
	UUserWidget* InGameMenu;

	/** Toggle The In Game Menu */
	void ToggleInGameMenu();

	/** Hide the Ingame menu if visible */
	UFUNCTION(BlueprintCallable, Category = "UI Events")
	void HideInGameMenu();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Events|Endgame")
	void AddEndGameScoreboard(int32 Score);

	UFUNCTION(BlueprintCallable, Category = "UI Events|Endgame")
	void RemoveEndGameScoreboard();

	UPROPERTY(BlueprintReadWrite, Category = "UI Menus|Endgame")
	UUserWidget* EndgameScoreboard;

	UFUNCTION(BlueprintCallable, Category = "UI Events")
	void ReturnToMainMenu(bool ShowCredits = false);

	/** Scoreboard to use for this gamemode */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TArray<TSubclassOf<UUserWidget>> ScoreboardClass;

	UPROPERTY(BlueprintReadWrite, Category = "UI Menus")
	UUserWidget* Scoreboard;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Events")
	void AddScoreboard();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Events")
	void RemoveScoreboard();

	/** Toggle Scoreboard */
	void ToggleScoreboard();

	/** Hide the Scoreboard */
	void HideScoreboard();

	/** Show Scoreboard */
	void ShowScoreboard();

	/** Null State Widget */
	UPROPERTY(BlueprintReadWrite, Category = "Player Null State")
	UUserWidget* NullStateWidget;

	/** Add the NullState Widget to the view port via blueprint */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI Events")
	void AddNullStateWidget();

	/** Remove and null the UUserWidget for the NullState */
	UFUNCTION(BlueprintCallable, Category = "UI Events")
	void RemoveNullStateWidget();

	/** Clean up the player controller before returning to the main menu */
	virtual void HandleReturnToMainMenu() override; 

	/** Clean up the player controller before transitioning to the next level */
	void HandleNextLevelTransition();

	virtual void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;
	virtual void PostInitializeComponents() override;

protected:
	void GetSensitivitySettings();
	
	/** Look Sensitivity. Modifies the Delta Movement before moving the camera of the Snake */
	float XSensitivity;
	float YSensitivity;

	FTimerHandle Respawn_TimerHandle;

	ASnakeHUD* GetSnakeHUD() const;

public:
	/** Event Triggered when the Input Settings have changed. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Player|Control")
	void InputSettingsChanged();

	/** Set the Players Controller Input Settings. If true, the primary player is using a gamepad controller. Note: Players 2-4 can ONLY use a controller  */
	UFUNCTION(BlueprintCallable, Category = "Player|Control")
	void SetInputSettings(bool Value);

	UFUNCTION(BlueprintCallable, Category = "Player|Control")
	void SetTwinStickMode(bool Value);

	UFUNCTION(BlueprintPure, Category = "Player|Control")
	bool GetTwinStickMode() const;

public:
	/** Signal that the Secondary Key has been pressed. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Player|Input")
	void SecondaryPressed();

	/** Signal that the Secondary Key has been Released. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Player|Input")
	void SecondaryReleased();

public:
	UPROPERTY(BlueprintReadWrite, Category = "UI Menus|Boss Health Bar")
	UUserWidget* BossBarWidget;

	UFUNCTION(BlueprintCallable, Category = "UI Menus|Boss Health Bar")
	void RemoveBossBarWidget();

	/** Set the Tutorial Repeat Variables 
	*	@First - Has the player completed this level? 
	*	@Repeat - Has the player choosen to repeat the tutorials? */
	UFUNCTION()
	void SetTutorialRepeat(bool First, bool Repeat);

	/** bLevelCompleted Getter */
	UFUNCTION(BlueprintPure, Category = "Player|Progress")
	bool GetCurrentLevelCompleted() const;

protected:
	/** If true, the player has compelted this level before */
	UPROPERTY()
	bool bLevelCompleted;

public:
	UFUNCTION(BlueprintPure, Category = "Player|Score")
	int32 GetDeathCount();

	/** Trigger an event after the score has been assigned and saved to the players slot. This can also act as a base to ensure the data is valid before being displayed to the player. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Player|Score")
	void OnScoreAssigned(int32 OutScore);

	/** Display the Current Score to the player. This is called from GameMode_Story after the player has died. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Player|Score")
	void DisplayCurrentScore(int32 OutScore);

	/** Score Widget Reference */
	UPROPERTY(BlueprintReadOnly, Category = "Player|Score")
	UUserWidget* ScoreProgressWidget;

	/** Assing the Property InScoreProgress to Progress and start the removal timer */
	UFUNCTION(BlueprintCallable, Category = "Player|Score")
	void AddScoreProgressWidget(UUserWidget* InScoreProgress);

	UFUNCTION(BlueprintCallable, Category = "Player|Score")
	void RemoveScoreProgressWidget();

	UPROPERTY(EditAnywhere, Category = "Player|Score")
	float ScoreProgressDuration;
	
private:
	FTimerHandle ScoreProgress_TimerHandle;

public:
	/** Widget that displays misc info to the player */
	UPROPERTY(BlueprintReadWrite, Category = "Player|Difficulty")
	UUserWidget* MiscInfoWidget;

	UFUNCTION(BlueprintImplementableEvent, Category = "Player|Difficulty")
	void AddMiscInfoWidget();

	UFUNCTION(BlueprintCallable, Category = "Player|Difficulty")
	void RemoveMiscInfoWidget();

public:
	UFUNCTION(BlueprintCallable, Category = "Player|Cine")
	void SuppressWidgets();

	UFUNCTION(BlueprintCallable, Category = "Player|Cine")
	void RestoreWidgets();

	/** Set the players input suppression 
	*	If InInput == true, the input will be blocked. Else, input can be processed */
	UFUNCTION(BlueprintCallable, Category = "Player|Cine")
	void BlockPlayerInput(bool InInput);

	UFUNCTION(BlueprintCallable, Category = "Player|Cine")
	void RemoveIntroWidget();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameRestart);

	UPROPERTY(BlueprintAssignable, Category = "Player|Map Reset")
	FOnGameRestart OnGameRestart;

	/** Remove all the widgets from the viewport and null them out */
	void RemoveAllWidgets();

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Player|Cine")
	UUserWidget* IntroWidget;

public:
	UFUNCTION(BlueprintPure, Category = "Player|Unlocked Abilities")
	bool GetAttackType02Unlocked() const;

	UFUNCTION(BlueprintCallable, Category = "Player|Unlocked ABilities")
	void SetAttackType02Unlocked(bool Value, bool Save = true);

	UFUNCTION(BlueprintPure, Category = "Player|Unlocked Abilities")
	bool GetAttackType03Unlocked() const;

	UFUNCTION(BlueprintCallable, Category = "Player|Unlocked Abilities")
	void SetAttackType03Unlocked(bool Value, bool Save = true);

	UFUNCTION(BlueprintPure, Category = "Player|Unlocked Abilities")
	bool GetAttackType04Unlocked() const;

	UFUNCTION(BlueprintCallable, Category = "Player|Unlocked Abilities")
	void SetAttackType04Unlocked(bool Value, bool Save = true);

	UFUNCTION(BlueprintPure, Category = "Player|Unlocked Abilities")
	bool GetFocusShapeUnlocked() const;

	UFUNCTION(BlueprintCallable, Category = "Player|Unlocked ABilities")
	void SetFocusShapeUnlocked(bool Value, bool Save = true);

	UFUNCTION(BlueprintPure, Category = "Player|Unlocked Abilities")
	bool GetLazerShapeUnlocked() const;

	UFUNCTION(BlueprintCallable, Category = "Player|Unlocked Abilities")
	void SetLazerShapeUnlocked(bool Value, bool Save = true);

	UFUNCTION(BlueprintPure, Category = "Player|Unlocked Abilities")
	bool GetSpeedShapeUnlocked() const;

	UFUNCTION(BlueprintCallable, Category = "Player|Unlocked Abilities")
	void SetSpeedShapeUnlocked(bool Value, bool Save = true);

	UFUNCTION(BlueprintPure, Category = "Player|Unlocked Abilities")
	bool GetShieldShapeUnlocked() const;

	UFUNCTION(BlueprintCallable, Category = "Player|Unlocked Abilities")
	void SetShieldShapeUnlocked(bool Value, bool Save = true);

	UFUNCTION(Exec, BlueprintCallable, Category = "Player|Unlocked Abilities")
	void HardResetAbilities();

	/** Unlock all the abilities without saving. This will be used when loading into the Arcade Mode without completing the Story mode */
	UFUNCTION(Exec, BlueprintCallable, Category = "Player|Unlocked Abilities")
	void UnlockAllAbilitiesWithoutSave();

	/** Unlock all the abilities and Save */
	UFUNCTION(Exec, BlueprintCallable, Category = "Player|Unlocked Abilities")
	void UnlockAllAbilities();

private:
	/** Rotation of the Camera */
	FRotator CameraRotation;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Arcade Scoreboard")
	FVector DeathCamOffset;

	UPROPERTY(EditAnywhere, Category = "Player|Arcade Scoreboard")
	float CameraBoomHeight;

};
