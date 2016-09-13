// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"
#include "Blueprint/UserWidget.h"
#include "ScoreboardItem.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API UScoreboardItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UScoreboardItem(const class FObjectInitializer& PCIP);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
	FLinearColor TextTint;

	UPROPERTY(BlueprintReadWrite, Category = "Scoreboard Item")
	FString TextMessage;

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard Item")
	FRankMapStruct PlayerRank;

	UFUNCTION(BlueprintCallable, Category = "Scoreboard Item")
	void SetPlayerRank(FRankMapStruct InRank);

	UFUNCTION(BlueprintCallable, Category = "Scoreboard Item")
	void SetText(FString NewMessage);

	UFUNCTION(BlueprintCallable, Category = "Scoreboard Item")
	bool IsLocalPlayer();

};
