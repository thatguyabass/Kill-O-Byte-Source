// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"
#include "Blueprint/UserWidget.h"
#include "ServerListItem.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API UServerListItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UServerListItem(const class FObjectInitializer& PCIP);

	UFUNCTION(BlueprintCallable, Category = "Server Entry")
	void SetServerEntry(FServerEntry Entry);

	UFUNCTION(BlueprintCallable, Category = "Server Entry Helper")
	FString GetServerName() const;

	UFUNCTION(BlueprintCallable, Category = "Server Entry Helper")
	FString GetPlayerCount();

	UFUNCTION(BlueprintCallable, Category = "Server Entry Helper")
	FString GetPing() const;

	UFUNCTION(BlueprintCallable, Category = "Server Entry Helper")
	int32 GetServerEntryIndex() const;

private:
	/** Server Entry that this UI represents */
	FServerEntry ServerEntry;

};
