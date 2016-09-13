// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "AudioMenuWidget.h"
#include "UserData/UserData.h"
#include "UserData/SnakeLocalPlayer.h"
#include "Sound/SoundClass.h"
#include "AudioDevice.h"

const float UAudioMenuWidget::MaxOptionCount = 10;
const float UAudioMenuWidget::MaxVolumePercent = 0.8f;

const int32 UAudioMenuWidget::DefaultVolume = 10;

const FString UAudioMenuWidget::MasterName = "Master";
const FString UAudioMenuWidget::SFXName = "SFX";
const FString UAudioMenuWidget::MusicName = "Music";

UAudioMenuWidget::UAudioMenuWidget(const FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

void UAudioMenuWidget::ChangeMasterVolume(int32 OptionIndex, bool bSet)
{
	ChangeVolume(MasterName, GetVolume(OptionIndex));
	if (bSet)
	{
		SetMasterVolumeSetting(OptionIndex);
	}
}

void UAudioMenuWidget::ChangeSFXVolume(int32 OptionIndex, bool bSet)
{
	ChangeVolume(SFXName, GetVolume(OptionIndex));
	if (bSet)
	{
		SetSFXVolumeSetting(OptionIndex);
	}
}

void UAudioMenuWidget::ChangeMusicVolume(int32 OptionIndex, bool bSet)
{
	ChangeVolume(MusicName, GetVolume(OptionIndex));
	if (bSet)
	{
		SetMusicVolumeSetting(OptionIndex);
	}
}

void UAudioMenuWidget::ChangeVolume(FString ClassName, float Volume)
{
	FAudioDevice* Device = GEngine->GetMainAudioDevice();
	if (!Device)
	{
		return;
	}

	for (TMap<USoundClass*, FSoundClassProperties>::TIterator Sound(Device->SoundClasses); Sound; ++Sound)
	{
		USoundClass* SoundClass = Sound.Key();
		if (SoundClass && SoundClass->GetFullName().Find(ClassName) != INDEX_NONE)
		{
			SoundClass->Properties.Volume = Volume;
		}
	}
}

UUserData* UAudioMenuWidget::GetSavedData()
{
	USnakeLocalPlayer* const LocalPlayer = Cast<USnakeLocalPlayer>(GetOwningLocalPlayer());
	if (LocalPlayer)
	{
		return LocalPlayer->GetUserData();
	}

	return nullptr;
}

float UAudioMenuWidget::GetVolume(int32 OptionIndex)
{
	float Blend = OptionIndex / MaxOptionCount;
	return FMath::Lerp(0.0f, MaxVolumePercent, Blend);
}

int32 UAudioMenuWidget::GetMasterVolumeSetting()
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		return Data->GetMasterVolumeSetting();
	}

	return -1;
}

int32 UAudioMenuWidget::GetSFXVolumeSetting()
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		return Data->GetSFXVolumeSetting();
	}

	return -1;
}

int32 UAudioMenuWidget::GetMusicVolumeSetting()
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		return Data->GetMusicVolumeSetting();
	}

	return -1;
}

void UAudioMenuWidget::SetMasterVolumeSetting(int32 InMasterVolume)
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		Data->SetMasterVolumeSetting(InMasterVolume);
		Data->SaveIfDirty();
	}
}

void UAudioMenuWidget::SetSFXVolumeSetting(int32 InSFXVolume)
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		Data->SetSFXVolumeSetting(InSFXVolume);
		Data->SaveIfDirty();
	}
}

void UAudioMenuWidget::SetMusicVolumeSetting(int32 InMusicVolume)
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		Data->SetMusicVolumeSetting(InMusicVolume);
		Data->SaveIfDirty();
	}
}

void UAudioMenuWidget::ResetAudioSettings()
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		ChangeMasterVolume(DefaultVolume, true);
		ChangeSFXVolume(DefaultVolume, true);
		ChangeMusicVolume(DefaultVolume, true);
	}
}