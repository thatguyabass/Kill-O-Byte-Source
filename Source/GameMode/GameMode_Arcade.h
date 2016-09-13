// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameMode/BaseGameMode.h"
#include "GameMode_Arcade.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AGameMode_Arcade : public ABaseGameMode
{
	GENERATED_BODY()

public:
	AGameMode_Arcade(const class FObjectInitializer& PCIP);
	
	virtual void PreInitializeComponents() override;

	void WaveTick();

	virtual void FinishMatch() override;

	/** How many each player starts the game with */
	UPROPERTY(EditAnywhere, Category = "Arcade | Lives")
	int32 StartingLives;

	/** Highest Power level can get */
	UPROPERTY(EditAnywhere, Category = "Arcade | Scale")
	int32 MaxPowerLevel;

	/** How long before the power level is increased again */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arcade | Scale")
	float RoundDuration;

	/** Start the Current Match */
	virtual void HandleMatchHasStarted() override;

	virtual int32 GetLevelIndex() const override;

protected:
	/** Find the bot manager in the level. */
	void FindBotManager();

	/** Bot Manager class handles the spawning of the bots. This reference is used to increase the max spawn numbers */
	UPROPERTY()
	class ABotManager* BotManager;

	/** Current Power Level*/
	int32 PowerLevel;
	FTimerHandle Power_TimerHandle;

	/** Increase the Current Power Level */
	void IncreasePowerLevel();

	FTimerHandle WaveTick_TimerHandle;

protected:
	/** Arcade Score Data Asset. Use post match to determine the players score */
	UPROPERTY()
	class UArcadeScore* ArcadeScoreDataAsset;

	/** Data Asset of the Arcade Level Names. Use to get the Level Index for the current level */
	UPROPERTY()
	class ULevelListDataAsset* LevelListDataAsset;

public:
	/** Get the current wave index */
	UFUNCTION(BlueprintPure, Category = "Arcade|Wave")
	int32 GetWaveIndex() const;

	/** Max Arcade Wave index */
	UFUNCTION(BlueprintPure, Category = "Arcade|Wave")
	int32 GetMaxWaveIndex() const;

public:
	/** Get the Bot Kills */
	UFUNCTION(BlueprintPure, Category = "Arcade|Score")
	int32 GetGruntKills() const;

	UFUNCTION(BlueprintPure, Category = "Arcade|Score")
	int32 GetDroneKills() const;

	UFUNCTION(BlueprintPure, Category = "Arcade|Score")
	int32 GetTankKills() const;

	UFUNCTION(BlueprintPure, Category = "Arcade|Score")
	int32 GetSuperGruntKills() const;

	UFUNCTION(BlueprintPure, Category = "Arcade|Score")
	int32 GetSeekerKills() const;

	UFUNCTION(BlueprintPure, Category = "Arcade|Score")
	int32 GetShieldKills() const;

	UFUNCTION(BlueprintPure, Category = "Arcade|Score")
	int32 GetMortarKills() const;

	UFUNCTION(BlueprintPure, Category = "Arcade|Score")
	int32 GetBeamKills() const;

	UFUNCTION(BlueprintCallable, Category = "Arcade|Score")
	void AddBotKill(uint8 BotClassType);

private:
	UPROPERTY()
	int32 GruntKills;

	UPROPERTY()
	int32 DroneKills;

	UPROPERTY()
	int32 TankKills;

	UPROPERTY()
	int32 SuperGruntKills;

	UPROPERTY()
	int32 SeekerKills;

	UPROPERTY()
	int32 ShieldKills;

	UPROPERTY()
	int32 MortarKills;

	UPROPERTY()
	int32 BeamKills;

};
