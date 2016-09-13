// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "MovieMenuItem.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API UMovieMenuItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Movie")
	FString MoviePath;

	UFUNCTION(BlueprintCallable, Category = "Movie")
	void StartIngameMovie(UUserWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = "Movie")
	void StopIngameMovie();

	FReply OnKeyDown(const FGeometry& Geometry, const FKeyEvent& KeyEvent);

private:
	APlayerController* Controller;

};
