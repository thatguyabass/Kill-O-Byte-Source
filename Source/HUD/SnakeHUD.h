// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"
#include "GameFramework/HUD.h"
#include "SnakeHUD.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ASnakeHUD : public AHUD
{
	GENERATED_BODY()

public:
	ASnakeHUD(const class FObjectInitializer& PCIP);

	virtual void BeginPlay() override;
	
	/** Main Loop for drawing the HUD*/
	virtual void DrawHUD() override;

	/** Show or hide the scoreboard, if true the scoreboard is visible */
	//void ShowScoreboard(bool bEnabled);

	/** Toggle between visible and hidden */
	//void ToggleScoreboard();

	/** Forces the Scoreboard Closed */
	//void ForceCloseScoreboard();

	/** Is the match over? If true, the match is in the post game state*/
	bool IsMatchOver() const;
	
	/** Set the Match State */
	void SetMatchState(ESnakeMatchState::Type State);

	/** Get the Current Match State */
	ESnakeMatchState::Type GetMatchState() const;

	/** Is the Scoreboard being displayed? */
	//bool IsScoreboardVisible();

protected:

	/** Shared Pointer to the Scoreboard */
	//TSharedPtr<class SScoreboardWidget> ScoreboardWidget;
	
	/** Scoreboard Widget Overlay */
	//TSharedPtr<class SOverlay> ScoreboardWidgetOverlay;

	/** Scoreboard Widget Contrainer */
	//TSharedPtr<class SWidget> ScoreboardWidgetContainer;

	/** Current State of the Scoreboard. if True, the scoreboard is currently being displayed */
	//bool bScoreboardVisible;

	/** Scale of the UI */
	float UIScale;

	/** Min Scale */
	static const float MinUIScale;

	/** Dark Font Color */
	FColor DarkFontColor;

	/** Light Font Color */
	FColor LightFontColor;

	UPROPERTY()
	UTexture2D* HealthFill;

	UPROPERTY()
	UTexture2D* HealthColor;

	//UPROPERTY()
	//TArray<UTexture2D*> PowerIcons;

	/** Large Font */
	UPROPERTY()
	UFont* BigFont;

	/** Small Font */
	UPROPERTY()
	UFont* SmallFont;

	/** FontRenderInfo enabled casting Shadows */
	FFontRenderInfo ShadowedText;

	/** Draw the Remaining Match Time */
	void DrawMatchTime();

	/** Draw Player Health */
	void DrawPlayerHealth();

	/** Draw Current Power */
	//void DrawPowerIcons();

	/** Convert the int into minutes and seconds */
	FString GetMatchTime(float MatchTime);
	
	/** Current State of the Match */
	ESnakeMatchState::Type MatchState;

};