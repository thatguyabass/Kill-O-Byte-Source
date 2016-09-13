// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameUserSettings.h"
#include "UserSettings.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API UUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:
	UUserSettings(const class FObjectInitializer& PCIP);

	/** Applies all current user settings to the game and saves to permanent storage, optionally checking for command line overrides */
	virtual void ApplySettings(bool bCheckForCommandLinOverrides) override;
	
	int32 GetGraphicsQuality() const
	{
		return GraphicsQuality;
	}
	
	void SetGraphicsQuality(int32 InGraphicsQuality)
	{
		GraphicsQuality = InGraphicsQuality;
	}

	/** UGameUserSettings Override */
	virtual void SetToDefaults() override;

private:
	/**
	* 0 = Low
	* 1 = High
	*/
	UPROPERTY(config)
	int32 GraphicsQuality;

};
