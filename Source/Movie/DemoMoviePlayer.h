// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/MoviePlayer/Public/MoviePlayer.h"

namespace DemoMovie
{
	static void PlayDemoMovie()
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
		LoadingScreen.bMoviesAreSkippable = true;

		LoadingScreen.MoviePaths.Add(TEXT("MenuGameplayTrailer01"));

		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
		GetMoviePlayer()->PlayMovie();
	}
};