// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "PlayerListUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API UPlayerListUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPlayerListUserWidget(const class FObjectInitializer& PCIP);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerListProperty")
	int32 PlayerIndex;

	UFUNCTION(BlueprintCallable, Category = "PlayerListHelper")
	FString GetPlayerNameString();

	UFUNCTION(BlueprintCallable, Category = "PlayerListHelper")
	FString GetTeamID();

	UFUNCTION(BlueprintCallable, Category = "PlayerListHelper")
	FLinearColor GetTeamPrimaryColor();

	UFUNCTION(BlueprintCallable, Category = "PlayerListHelper")
	bool IsLocalPlayer();

private:
	class ASnakePlayerState* GetPlayerState();

};
