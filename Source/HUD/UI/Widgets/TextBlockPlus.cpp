// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "TextBlockPlus.h"

UTextBlockPlus::UTextBlockPlus(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	bScrollText = false;
	bScrollInitialized = false;
}

void UTextBlockPlus::SetFont(FSlateFontInfo InFont)
{
	Font = InFont;
	SynchronizeProperties();
}

void UTextBlockPlus::SetFontSize(float InSize)
{
	FSlateFontInfo NewFont = Font;
	NewFont.Size = InSize;

	SetFont(NewFont);
}

void UTextBlockPlus::ScaleFontSize(float InScale)
{
	SetFontSize(Font.Size * InScale);
}

void UTextBlockPlus::InitializeTextScroll()
{
	ScrollUpdateProgress = 0.0f;
	CharacterCount = 0;
	bScrollInitialized = true;

	//TextToScroll = Text.ToString();
}

void UTextBlockPlus::StoreTextToScroll()
{
	TextToScroll = Text.ToString();
}

void UTextBlockPlus::TextScrollTick(float DeltaTime)
{
	if (bScrollInitialized && bScrollText)
	{
		if(CharacterCount >= TextToScroll.Len())
		{
			//Scroll Finished
			bScrollText = false;

			OnScrollFinished.Broadcast();
		}

		ScrollUpdateProgress += DeltaTime;
		if (ScrollUpdateProgress >= (ScrollSpeed * DeltaTime))
		{
			CharacterCount++;
			ScrollUpdateProgress = 0.0f;
		}
	}
}

FString UTextBlockPlus::GetScrollText()
{
	return TextToScroll.Len() > 0 ? TextToScroll.LeftChop(TextToScroll.Len() - CharacterCount) : FString("No Text To Scroll");
}