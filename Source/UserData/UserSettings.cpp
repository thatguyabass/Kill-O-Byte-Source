// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "UserSettings.h"

UUserSettings::UUserSettings(const FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

void UUserSettings::SetToDefaults()
{
	Super::SetToDefaults();

	GraphicsQuality = 1;
}

void UUserSettings::ApplySettings(bool bCheckForCommandLineOverrides)
{
	ScalabilityQuality.SetFromSingleQualityLevel(GraphicsQuality);

	Super::ApplySettings(bCheckForCommandLineOverrides);

	if (!GEngine)
	{
		return;
	}
}