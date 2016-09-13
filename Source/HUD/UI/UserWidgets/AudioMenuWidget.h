// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "AudioMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API UAudioMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UAudioMenuWidget(const class FObjectInitializer& PCIP);
	
	/** Get Saved User Data */
	class UUserData* GetSavedData();

	/** Change and Set the Master Volume Settings 
	*	If bSet is true, the Index will be saved as the volume level */
	UFUNCTION(BlueprintCallable, Category = "AudioMenu")
	void ChangeMasterVolume(int32 OptionIndex, bool bSet = false);

	/** Change and Set the SFX Volume Settings 
	*	If bSet is true, the Index will be saved as the volume level */
	UFUNCTION(BlueprintCallable, Category = "AudioMenu")
	void ChangeSFXVolume(int32 OptionIndex, bool bSet = false);

	/** Change and Set the Music Volume Settings 
	*	If bSet is true, the Index will be saved as the volume level */
	UFUNCTION(BlueprintCallable, Category = "AudioMenu")
	void ChangeMusicVolume(int32 OptionIndex, bool bSet = false);

	/** Change the Volume of the Sound Class using the ClassName to locate the correct SoundClass */
	UFUNCTION()
	void ChangeVolume(FString ClassName, float Volume);
	
	/** Get Master Volume Settings */
	UFUNCTION(BlueprintPure, Category = "AudioMenu")
	int32 GetMasterVolumeSetting();

	/** Get SFX Volume Settings */
	UFUNCTION(BlueprintPure, Category = "AudioMenu")
	int32 GetSFXVolumeSetting();

	/** Get Music Volume Settings */
	UFUNCTION(BlueprintPure, Category = "AudioMenu")
	int32 GetMusicVolumeSetting();

	/** Set the master Volume Level */
	UFUNCTION(BlueprintCallable, Category = "AudioMenu")
	void SetMasterVolumeSetting(int32 InMasterVolume);

	/** Set the SFX Volume Level */
	UFUNCTION(BlueprintCallable, Category = "AudioMenu")
	void SetSFXVolumeSetting(int32 InSFXVolume);

	/** Set the Music Volume Level */
	UFUNCTION(BlueprintCallable, Category = "AudioMenu")
	void SetMusicVolumeSetting(int32 InMusicVolume);

	/** Reset all audio volumes to the default value */
	UFUNCTION(BlueprintCallable, Category = "AudioMenu")
	void ResetAudioSettings();

protected:
	static const float MaxOptionCount;
	static const float MaxVolumePercent;

	static const int32 DefaultVolume;

	/** Sound Class Names */
	static const FString MasterName;
	static const FString SFXName;
	static const FString MusicName;

	/** Get the Volume On a scale of 1-0. */
	float GetVolume(int32 OptionIndex);

};
