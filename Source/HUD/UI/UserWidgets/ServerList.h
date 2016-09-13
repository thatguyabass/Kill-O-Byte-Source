// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"
#include "Blueprint/UserWidget.h"
#include "ServerList.generated.h"

/**
 * User Widget that collects and displays the Servers that a player an connect to
 * 
 * Server Collection is done through the IOnlineSubsystem interface and stored into ServerListItems
 * which get added to a vertical box and displayed to the player 
 */

UCLASS()
class SNAKE_PROJECT_API UServerList : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UServerList(const class FObjectInitializer& PCIP);
	
	/** Start Server Search */
	UFUNCTION(BlueprintCallable, Category = "Server List")
	void BeginServerSearch(bool bLanMatch);

	/** Update Search Status when SearchingForServer is True */
	void NativeTick(FGeometry MyGeometry, float InDeltaTime);

	/** Updates Current Search Status */
	void UpdateSearchStatus();

	/** Fill/Update server List */
	void UpdateServerList();

	/** Called when Server Search has Finished */
	void OnServerSearchFinished();

	/** Connect to the Server at ServerIndex */
	UFUNCTION(BlueprintCallable, Category = "Server List")
	void ConnectToServer(int32 ServerIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Server List")
	void FillServerEntryData();

	UFUNCTION(BlueprintCallable, Category = "Server List")
	int32 GetServerListCount();

	UFUNCTION(BlueprintCallable, Category = "Server List")
	FServerEntry GetServerEntry(int32 Index);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Server List")
	bool IsSearchInProgress();

	UFUNCTION(BlueprintCallable, Category = "Server List")
	FString GetStatusMessage();

protected:
	/** Searching for Lan Match? */
	bool bSearchLanMatch;

	/** Are we searching for a server, True if Searching for Server */
	bool bSearchingForServer;

	/** Current Search Status Message */
	FString StatusText;

	/** Cast AGameSession to our Custom Session */
	class ASnakeSession* GetGameSession() const;

	/** Array of Server Entries, Each entry is a single server the player can join */
	TArray<TSharedPtr<FServerEntry>> ServerList;

};
