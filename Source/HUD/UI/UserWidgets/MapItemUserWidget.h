// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"
#include "Blueprint/UserWidget.h"
#include "MapItemUserWidget.generated.h"

UCLASS()
class SNAKE_PROJECT_API UMapItemUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UMapItemUserWidget(const class FObjectInitializer& PCIP);

	UPROPERTY(BlueprintReadOnly, Category = "Map Item")
	FMapItemData MapData;

	UFUNCTION(BlueprintPure, Category = "Map Item Helper")
	FMapItemData& GetData();

	UFUNCTION(BlueprintCallable, Category = "Map Item Helper")
	void SetMapData(FMapItemData Data);

	UFUNCTION(BlueprintPure, Category = "Map Item Helper")
	FString GetSafeName();

	UFUNCTION(BlueprintPure, Category = "Map Item Helper")
	FString GetTrueName();

	UFUNCTION(BlueprintCallable, Category = "Map Item Helper")
	int32 GetMinPlayers();

	UFUNCTION(BlueprintCallable, Category = "Map Item Helper")
	int32 GetMaxPlayers();

	UFUNCTION(BlueprintPure, Category = "Map Item Helper")
	FSlateBrush GetMapImage();

};
