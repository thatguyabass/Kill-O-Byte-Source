// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "UserData.h"

UUserData::UUserData(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	SetToDefault();
}

void UUserData::SetToDefault()
{
	Gamma = 2.0f;
	bUseController = false;
	bTwinStickMode = false;

	bAttack02Unlocked = false;
	bAttack03Unlocked = false;
	bAttack04Unlocked = false;

	bFocusShapeUnlocked = false;
	bLazerShapeUnlocked = false;
	bSpeedShapeUnlocked = false;
	bShieldShapeUnlocked = false;

	bHardDifficulty = false;

	const int32 SPLevelCount = 12;
	const int32 ALevelCount = 3;

	for (int32 c = 0; c < SPLevelCount; c++)
	{
		NormalScores.Add(-1);
		HardScores.Add(-1);

		LevelsCompleted.Add(false);
	}

	for (int32 c = 0; c < ALevelCount; c++)
	{
		ArcadeNormalScores.Add(-1);
		ArcadeHardScores.Add(-1);

		NormalWaveScores.Add(-1);
		HardWaveScores.Add(-1);
	}

	MasterVolume = 10;
	SFXVolume = 10;
	MusicVolume = 10;
}

void UUserData::SaveUserData()
{
	UGameplayStatics::SaveGameToSlot(this, SlotName, UserIndex);
	bIsDirty = false;
}

UUserData* UUserData::LoadUserData(FString SlotName, int32 UserIndex)
{
	UUserData* UserData = nullptr;

	// Ensure the Slotname is valid. UWorld may not exist before the first player signs in
	if (SlotName.Len() > 0)
	{
		UserData = Cast<UUserData>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
		if (UserData == nullptr)
		{
			//If load failed, create a new one
			UserData = Cast<UUserData>(UGameplayStatics::CreateSaveGameObject(UUserData::StaticClass()));
		}
		check(UserData != nullptr);

		UserData->SlotName = SlotName;
		UserData->UserIndex = UserIndex;
	}

	return UserData;
}

void UUserData::SaveIfDirty()
{
	if (bIsDirty)
	{
		SaveUserData();
	}
}

int32 UUserData::GetUserIndex() const
{
	return UserIndex;
}

void UUserData::SetGamma(float InGamma)
{
	bIsDirty |= Gamma != InGamma;
	Gamma = InGamma;
}

void UUserData::SetGammaIndex(int32 Index)
{
	bIsDirty |= GammaIndex != Index;
	GammaIndex = Index;
}

void UUserData::SetInputSettings(bool Value)
{
	bIsDirty |= bUseController != Value;
	bUseController = Value;
}

void UUserData::SetTwinStickMode(bool Value)
{
	bIsDirty |= bTwinStickMode != Value;
	bTwinStickMode = Value;
}

void UUserData::SetAttack02Unlocked(bool Value)
{
	bIsDirty |= bAttack02Unlocked != Value;
	bAttack02Unlocked = Value;
}

void UUserData::SetAttack03Unlocked(bool Value)
{
	bIsDirty |= bAttack03Unlocked != Value;
	bAttack03Unlocked = Value;
}

void UUserData::SetAttack04Unlocked(bool Value)
{
	bIsDirty |= bAttack04Unlocked != Value;
	bAttack04Unlocked = Value;
}

void UUserData::SetFocusShapeUnlocked(bool Value)
{
	bIsDirty |= bFocusShapeUnlocked != Value;
	bFocusShapeUnlocked = Value;
}

void UUserData::SetLazerShapeUnlocked(bool Value)
{
	bIsDirty |= bLazerShapeUnlocked != Value;
	bLazerShapeUnlocked = Value;
}

void UUserData::SetSpeedShapeUnlocked(bool Value)
{
	bIsDirty |= bSpeedShapeUnlocked != Value;
	bSpeedShapeUnlocked = Value;
}

void UUserData::SetShieldShapeUnlocked(bool Value)
{
	bIsDirty |= bShieldShapeUnlocked != Value;
	bShieldShapeUnlocked = Value;
}

void UUserData::SetDifficulty(bool Value)
{
	bIsDirty |= bHardDifficulty != Value;
	bHardDifficulty = Value;
}

void UUserData::SetNormalScores(int32 LevelIndex, int32 NormalScore)
{
	ensure(NormalScores.IsValidIndex(LevelIndex));

	//Ensure the new score is Greater or even to the current one
	if (NormalScores[LevelIndex] <= NormalScore)
	{
		bIsDirty |= NormalScores[LevelIndex] != NormalScore;
		NormalScores[LevelIndex] = NormalScore;
	}
}

void UUserData::ResetNormalScores()
{
	for (int32 c = 0; c < NormalScores.Num(); c++)
	{
		NormalScores[c] = -1;
	}
	bIsDirty = true;

	SaveIfDirty();
}

void UUserData::SetHardScores(int32 LevelIndex, int32 HardScore)
{
	ensure(HardScores.IsValidIndex(LevelIndex));

	//Ensure the new score is Greater or even to the current one
	if (HardScores[LevelIndex] <= HardScore)
	{
		bIsDirty |= HardScores[LevelIndex] != HardScore;
		HardScores[LevelIndex] = HardScore;
	}
}

void UUserData::ResetHardScores()
{
	for (int32 c = 0; c < HardScores.Num(); c++)
	{
		HardScores[c] = -1;
	}
	bIsDirty = true;

	SaveIfDirty();
}

void UUserData::SetWaveNormal(int32 LevelIndex, int32 WaveIndex)
{
	ensure(NormalWaveScores.IsValidIndex(LevelIndex));

	/** Ensure the new score is greater or even to the current one */
	if (NormalWaveScores[LevelIndex] <= WaveIndex)
	{
		bIsDirty |= NormalWaveScores[LevelIndex] != WaveIndex;
		NormalWaveScores[LevelIndex] = WaveIndex;
	}
}

void UUserData::SetWaveHard(int32 LevelIndex, int32 WaveIndex)
{
	ensure(HardWaveScores.IsValidIndex(LevelIndex));

	/** Ensure the new score is greater or even to the current one */
	if (HardWaveScores[LevelIndex] <= WaveIndex)
	{
		bIsDirty |= HardWaveScores[LevelIndex] != WaveIndex;
		HardWaveScores[LevelIndex] = WaveIndex;
	}
}

void UUserData::SetArcadeNormalScores(int32 LevelIndex, int32 NormalScore)
{
	ensure(ArcadeNormalScores.IsValidIndex(LevelIndex));

	/** Ensure the new score is greater or even to the current one */
	if (ArcadeNormalScores[LevelIndex] <= NormalScore)
	{
		bIsDirty |= ArcadeNormalScores[LevelIndex] != NormalScore;
		ArcadeNormalScores[LevelIndex] = NormalScore;
	}
}

void UUserData::SetArcadeHardScores(int32 LevelIndex, int32 HardScore)
{
	ensure(ArcadeHardScores.IsValidIndex(LevelIndex));

	/** Ensure the new score is greater or even to the current one */
	if (ArcadeHardScores[LevelIndex] <= HardScore)
	{
		bIsDirty |= ArcadeHardScores[LevelIndex] != HardScore;
		ArcadeHardScores[LevelIndex] = HardScore;
	}
}

void UUserData::ResetArcadeNormalScores()
{
	for (int32 c = 0; c < ArcadeNormalScores.Num(); c++)
	{
		ArcadeNormalScores[c] = -1;
	}
	bIsDirty = true;

	SaveIfDirty();
}

void UUserData::ResetArcadeHardScores()
{
	for (int32 c = 0; c < ArcadeHardScores.Num(); c++)
	{
		ArcadeHardScores[c] = -1;
	}
	bIsDirty = true;

	SaveIfDirty();
}

void UUserData::ResetArcadeWaveNormal()
{
	for (int32 c = 0; c < NormalWaveScores.Num(); c++)
	{
		NormalWaveScores[c] = -1;
	}
	bIsDirty = true;

	SaveIfDirty();
}

void UUserData::ResetArcadeWaveHard()
{
	for (int32 c = 0; c < HardWaveScores.Num(); c++)
	{
		HardWaveScores[c] = -1;
	}
	bIsDirty = true;

	SaveIfDirty();
}

void UUserData::SetLevelCompleted(int32 LevelIndex, bool InValue)
{
	ensure(LevelsCompleted.IsValidIndex(LevelIndex));

	bIsDirty |= LevelsCompleted[LevelIndex] != InValue;
	LevelsCompleted[LevelIndex] = InValue;
}

void UUserData::SetMasterVolumeSetting(int32 InMasterVolume)
{
	bIsDirty |= MasterVolume != InMasterVolume;
	MasterVolume = InMasterVolume;
}

void UUserData::SetSFXVolumeSetting(int32 InSFXVolume)
{
	bIsDirty |= SFXVolume != InSFXVolume;
	SFXVolume = InSFXVolume;
}

void UUserData::SetMusicVolumeSetting(int32 InMusicVolume)
{
	bIsDirty |= MusicVolume != InMusicVolume;
	MusicVolume = InMusicVolume;
}