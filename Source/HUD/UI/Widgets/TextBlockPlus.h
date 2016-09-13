// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/TextBlock.h"
#include "TextBlockPlus.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API UTextBlockPlus : public UTextBlock
{
	GENERATED_BODY()
	
public:
	UTextBlockPlus(const class FObjectInitializer& PCIP);

	UFUNCTION(BlueprintCallable, Category = "Custom")
	void SetFont(FSlateFontInfo InFont);

	UFUNCTION(BlueprintCallable, Category = "Custom")
	void SetFontSize(float InSize);

	UFUNCTION(BlueprintCallable, Category = "Custom")
	void ScaleFontSize(float InScale);

	/** Text to display to the player */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Custom")
	FString TextToScroll;

	/** Enable Text Scrolling. If true, make sure to call TextTick or it wont work */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
	bool bScrollText;

	/** Time between letters appearing on screen */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
	float ScrollSpeed;

	/** Initialize the Text Scroll. Call before running TextScrollTick */
	UFUNCTION(BlueprintCallable, Category = "Custom")
	void InitializeTextScroll();

	/** Call before Tick to store the text to scroll */
	UFUNCTION(BlueprintCallable, Category = "Custom")
	void StoreTextToScroll();

	/** Text Tick if bScrollText is true */
	UFUNCTION(BlueprintCallable, Category = "Custom")
	void TextScrollTick(float DeltaTime);

	UFUNCTION(BlueprintPure, Category = "Custom")
	FString GetScrollText();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScrollFinished);

	UPROPERTY(BlueprintAssignable, Category = "Custom Event")
	FOnScrollFinished OnScrollFinished;

private:
	UPROPERTY()
	float ScrollUpdateProgress;

	UPROPERTY()
	bool bScrollInitialized;

	/** Current number of characters being displayed to the player */
	UPROPERTY()
	int32 CharacterCount;

};
