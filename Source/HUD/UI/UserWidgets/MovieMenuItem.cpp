// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "MovieMenuItem.h"
#include "Runtime/MoviePlayer/Public/MoviePlayer.h"
#include "GenericApplicationMessagehandler.h"
#include "SlateBasics.h"
#include "SlateExtras.h"

class SMovieScreen : public SBorder
{
public:
	SLATE_BEGIN_ARGS(SMovieScreen) :
		_OnKeyDown()
	{}

	SLATE_EVENT(FOnKeyDown, OnKeyDown)
	SLATE_DEFAULT_SLOT(FArguments, Content)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		OnKeyDown = InArgs._OnKeyDown;

		SBorder::Construct(SBorder::FArguments()
			.BorderImage(FCoreStyle::Get().GetBrush(TEXT("BlackBrush")))
			.Padding(0)[InArgs._Content.Widget]);
	}

	void SetOnOnKeyDown(const FOnKeyDown& InHandler)
	{
		OnKeyDown = InHandler;
	}

protected:
	FOnKeyDown OnKeyDown;
};

void UMovieMenuItem::StartIngameMovie(UUserWidget* Widget)
{
	FLoadingScreenAttributes LoadingScreen;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
	LoadingScreen.bMoviesAreSkippable = true;
	LoadingScreen.bWaitForManualStop = false;
	LoadingScreen.MoviePaths.Add(MoviePath);

	//TSharedPtr<SWidget> Movie = SNew(SMovieScreen);
	//LoadingScreen.WidgetLoadingScreen = Movie;

	//FSlateApplication::Get().SetKeyboardFocus(Movie, EFocusCause::WindowActivate);
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	GetMoviePlayer()->PlayMovie();
}

FReply UMovieMenuItem::OnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	StopIngameMovie();

	return FReply::Handled();
}

void UMovieMenuItem::StopIngameMovie()
{
	GetMoviePlayer()->StopMovie();
}