// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"
#include "Blueprint/UserWidget.h"
#include "GameModeItem.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API UGameModeItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UGameModeItem(const class FObjectInitializer& PCIP);
	
	UFUNCTION(BlueprintCallable, Category = "Mode Helper")
	FGameModeData& GetData();

	UFUNCTION(BlueprintCallable, Category = "Mode Helper")
	void SetData(FGameModeData InData);

	UFUNCTION(BlueprintCallable, Category = "Mode Helper")
	FString GetGameModeName();

	UFUNCTION(BlueprintCallable, Category = "Mode Helper")
	FString GetGameModeShortHand();

protected:
	FGameModeData Data;

};
