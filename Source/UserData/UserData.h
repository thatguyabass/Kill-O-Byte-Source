// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "UserData.generated.h"

/**
 * Store Data that needs to be tracked between game sessions.
 */
UCLASS()
class SNAKE_PROJECT_API UUserData : public USaveGame
{
	GENERATED_BODY()
	
public:
	UUserData(const class FObjectInitializer& PCIP);
	
	/** Loads user data if it exists, creates and empty record otherwise */
	static UUserData* LoadUserData(FString SlotName, const int32 UserIndex);

	/** Save any data if it has changed since last save */
	void SaveIfDirty();
	
	int32 GetUserIndex() const;

	/** Index of the Slider in the menu */
	void SetGammaIndex(int32 Index);

	/** Get the Saved Gamma Index */
	FORCEINLINE int32 GetGammaIndex() const
	{
		return GammaIndex;
	}

	/** Set the Gamma Options */
	void SetGamma(float InGamma);

	/** Get the Gamma Option */
	FORCEINLINE float GetGamma() const
	{
		return Gamma;
	}

	/** Set the Input Method bool. If true, a gamepad is the exepected input device */
	UFUNCTION(exec)
	void SetInputSettings(bool Value);

	/** Get the Control Option */
	FORCEINLINE bool GetInputSetting() const
	{
		return bUseController;
	}

	/** Set Twin Stick Mode */
	void SetTwinStickMode(bool Value);

	/** Get Twin Stick Mode */
	FORCEINLINE bool GetTwinStickMode() const
	{
		return bTwinStickMode;
	}

	FORCEINLINE FString Getname() const
	{
		return SlotName;
	}

protected:
	/** Reset user data to the default state */
	void SetToDefault();

	/** Trigger a save of this data */
	void SaveUserData();

	/** Holds the Gamma correction settings */
	UPROPERTY()
	float Gamma;

	/** Holds the Gamma Index for the Settings Menu */
	UPROPERTY()
	int32 GammaIndex;

	/** Holds the Input Method Settings */
	UPROPERTY()
	bool bUseController;

	UPROPERTY()
	bool bTwinStickMode;

public:
	/** Get attack 02 unlocked state */
	FORCEINLINE bool GetAttack02Unlocked() const
	{
		return bAttack02Unlocked;
	}

	/** Set Attack 02 Unlocked State */
	void SetAttack02Unlocked(bool Value);

	/** Get Attack 03 Unlocked State */
	FORCEINLINE bool GetAttack03Unlocked() const
	{
		return bAttack03Unlocked;
	}

	/** Set Attack 03 Unlocked State */
	void SetAttack03Unlocked(bool Value);

	/** Get Attack 04 Unlocked State */
	FORCEINLINE bool GetAttack04Unlocked() const
	{
		return bAttack04Unlocked;
	}

	/** Set Attack 04 Unlocked State */
	void SetAttack04Unlocked(bool Value);

	/** Get Focus Shape Unlock State */
	FORCEINLINE bool GetFocusShapeUnlocked() const
	{
		return bFocusShapeUnlocked;
	}

	/** Set Focus Shape Unlock State */
	void SetFocusShapeUnlocked(bool Value);

	/** Get Lazer Shape Unlock State */
	FORCEINLINE bool GetLazerShapeUnlocked() const
	{
		return bLazerShapeUnlocked;
	}

	/** Set Lazer Shape Unlock State */
	void SetLazerShapeUnlocked(bool Value);

	/** Get Speed Shape Unlock State */
	FORCEINLINE bool GetSpeedShapeUnlocked() const
	{
		return bSpeedShapeUnlocked;
	}

	/** Set Speed Shape Unlock State */
	void SetSpeedShapeUnlocked(bool Value);

	/** Get Shield Shape Unlock State */
	FORCEINLINE bool GetShieldShapeUnlocked() const
	{
		return bShieldShapeUnlocked;
	}

	/** Set Shield Shape Unlock State */
	void SetShieldShapeUnlocked(bool Value);

protected:
	/*************** Progression */
	/** Attack Type 2 */
	UPROPERTY()
	bool bAttack02Unlocked;

	/** Attack Type 3 */
	UPROPERTY()
	bool bAttack03Unlocked;

	/** Attack Type 4 */
	UPROPERTY()
	bool bAttack04Unlocked;

	/** Focus Shape */
	UPROPERTY()
	bool bFocusShapeUnlocked;

	/** Lazer Shape */
	UPROPERTY()
	bool bLazerShapeUnlocked;

	/** Speed Shape */
	UPROPERTY()
	bool bSpeedShapeUnlocked;

	/** Shield Shape */
	UPROPERTY()
	bool bShieldShapeUnlocked;

public:
	/** Get the Players Difficulty Setting */
	FORCEINLINE bool GetDifficulty() const
	{
		return bHardDifficulty;
	}

	/** Set the Difficulty. 
	*	True = Hard
	*	False = Normal */
	void SetDifficulty(bool Value);

	/** Get the Players Normal Scores At Level Index */
	FORCEINLINE int32 GetNormalScores(int32 LevelIndex) const
	{
		return NormalScores.IsValidIndex(LevelIndex) ? NormalScores[LevelIndex] : -1;
	}

	/** Set the Players Normal Score At Level Index */
	void SetNormalScores(int32 LevelIndex, int32 NormalScore);

	/** Get the Normal Score Element Count */
	FORCEINLINE int32 GetNormalScoresLength() const
	{
		return NormalScores.Num();
	}

	/** Reset normal scores to the default value */
	void ResetNormalScores();

	/** Get the Players Hard Scores At Level Index */
	FORCEINLINE int32 GetHardScores(int32 LevelIndex) const
	{
		return HardScores.IsValidIndex(LevelIndex) ? HardScores[LevelIndex] : -1;
	}

	/** Set the Players Hard Score At Level Index */
	void SetHardScores(int32 LevelIndex, int32 HardScore);

	/** Get the Hard Score Element Count */
	FORCEINLINE int32 GetHardScoresLength() const
	{
		return HardScores.Num();
	}

	/** Reset hard scores to the default value */
	void ResetHardScores();

protected:
	/** Hard Flag. If true, the player is playing on Hard */
	UPROPERTY()
	bool bHardDifficulty;

	/** Array of Normal Player Scores */
	UPROPERTY()
	TArray<int32> NormalScores;

	/** Array of Hard Player Scores */
	UPROPERTY()
	TArray<int32> HardScores;

public:
	/** Get the highest wave completed on Normal Difficulty At Level Index */
	FORCEINLINE int32 GetWaveNormal(int32 LevelIndex) const
	{
		return NormalWaveScores.IsValidIndex(LevelIndex) ? NormalWaveScores[LevelIndex] : -1;
	}

	/** Set the Highest wave completed on normal difficulty at level index */
	void SetWaveNormal(int32 LevelIndex, int32 WaveIndex);

	/** Get the Highest Wave compelted on Hard Difficutly At Level Index */
	FORCEINLINE int32 GetWaveHard(int32 LevelIndex) const
	{
		return HardWaveScores.IsValidIndex(LevelIndex) ? HardWaveScores[LevelIndex] : -1;
	}

	/** Set the Highest Wave completed on hard difficulty at level index */
	void SetWaveHard(int32 LevelIndex, int32 WaveIndex);

	/** Get the Normal Difficulty Scores for the Arcade At Level Index */
	FORCEINLINE int32 GetArcadeNormalScores(int32 LevelIndex) const
	{
		return ArcadeNormalScores.IsValidIndex(LevelIndex) ? ArcadeNormalScores[LevelIndex] : -1;
	}

	/** Set the Normal Difficulty Score for the arcade at level index */
	void SetArcadeNormalScores(int32 LevelIndex, int32 NormalScore);

	/** Get the hard Difficulty Scores for the Arcade at level index */
	FORCEINLINE int32 GetArcadeHardScores(int32 LevelIndex) const
	{
		return ArcadeHardScores.IsValidIndex(LevelIndex) ? ArcadeHardScores[LevelIndex] : -1;
	}

	/** Set the Hard Difficulty Scores for the Arcade at level index */
	void SetArcadeHardScores(int32 LevelIndex, int32 HardScore);

	/** Reset all the normal arcade scores */
	UFUNCTION()
	void ResetArcadeNormalScores();

	/** Reset all the hard arcade scores */
	UFUNCTION()
	void ResetArcadeHardScores();

	/** Reset all the Normal arcade waves */
	UFUNCTION()
	void ResetArcadeWaveNormal();

	/** Reset all the Hard arcade waves */
	UFUNCTION()
	void ResetArcadeWaveHard();

protected:
	/** Highest Waves the player has reached for each map on Normal */
	UPROPERTY()
	TArray<int32> NormalWaveScores;

	/** Highest Waves the player has reached for each map on Hard */
	UPROPERTY()
	TArray<int32> HardWaveScores;

	/** Array of Arcade Normal Scores */
	UPROPERTY()
	TArray<int32> ArcadeNormalScores;

	/** Array of Arcade Hard Scores */
	UPROPERTY()
	TArray<int32> ArcadeHardScores;


public:
	/** Get the Levels Completed Status. If true, the level has been completed */
	FORCEINLINE bool GetLevelCompleted(int32 LevelIndex) const
	{
		return LevelsCompleted.IsValidIndex(LevelIndex) ? LevelsCompleted[LevelIndex] : false;
	}

	/** Set the level Completed at LevelIndex */
	void SetLevelCompleted(int32 LevelIndex, bool InValue);

	FORCEINLINE int32 GetLevelsCompletedLength() const
	{
		return LevelsCompleted.Num();
	}

protected:
	/** Levels Completed Status. If true, the level has been completed */
	UPROPERTY()
	TArray<bool> LevelsCompleted;

public:
	/** Get the Master Volume Level */
	FORCEINLINE int32 GetMasterVolumeSetting() const
	{
		return MasterVolume;
	}

	/** Set the Master Volume Level */
	void SetMasterVolumeSetting(int32 InMasterVolume);

	/** Get the SFX Volume Level */
	FORCEINLINE int32 GetSFXVolumeSetting() const
	{
		return SFXVolume;
	}

	/** Set the SFX Volume Level */
	void SetSFXVolumeSetting(int32 InSFXVolume);

	/** Get the Music Volume Level */
	FORCEINLINE int32 GetMusicVolumeSetting() const
	{
		return MusicVolume;
	}

	/** Set the Music Volume Level */
	void SetMusicVolumeSetting(int32 InMusicVolume);

protected:
	/** Master Volume Level */
	UPROPERTY()
	int32 MasterVolume;

	/** SFX Volume Level */
	UPROPERTY()
	int32 SFXVolume;

	/** Music Volume Level */
	UPROPERTY()
	int32 MusicVolume;

public:
	//FORCEINLINE int32 Get

private:
	/** Internal. if true, the data has changed */
	bool bIsDirty;

	/** string identifier to save/load this user data */
	FString SlotName;
	int32 UserIndex;

};