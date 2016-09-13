// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "SlateCore.h"
#include "SnakeViewportClient.h"

void USnakeViewportClient::ShowLoadingScreen()
{
	//Do not place a new widget one already exists 
	if (LoadingScreenWidget.IsValid())
	{
		return;
	}

	LoadingScreenWidget = SNew(SLoadingScreen);

	AddViewportWidgetContent(LoadingScreenWidget.ToSharedRef(), 0);
}

void USnakeViewportClient::HideLoadingScreen()
{
	//If not valid, return
	if (!LoadingScreenWidget.IsValid())
	{
		return;
	}

	RemoveViewportWidgetContent(LoadingScreenWidget.ToSharedRef());

	LoadingScreenWidget = nullptr;
}

void SLoadingScreen::Construct(const FArguments& InArgs)
{
	static const FName LoadingScreenName(TEXT("/Game/UI/LoadingScreen/LoadingScreen.LoadingScreen"));

	//Load a game style 
	LoadingScreenBrush = MakeShareable(new FSnakeLoadingScreenBrush(LoadingScreenName, FVector2D(1920, 1080)));

	ChildSlot
		[
			SNew(SOverlay)
			+SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SImage)
				.Image(LoadingScreenBrush.Get())
			]
			+SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SSafeZone)
				.VAlign(VAlign_Bottom)
				.HAlign(HAlign_Right)
				.Padding(10.0f)
				.IsTitleSafe(true)
				[
					SNew(SThrobber)
					.Visibility(this, &SLoadingScreen::GetLoadingIndicatorVisibility)
				]
			]
		];
}

void USnakeViewportClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Viewport->SetUserFocus(true);
}