// Fill out your copyright notice in the Description page of Project Settings.

/** 
*	This class holds all the data that is shared between players that are connected to teh same lobby.
*	This is to ensure the players see the same data when the map, game mode or match starting has changed. 
*	The actor is to be placed inside the map itself and found by each player controller upon entering the lobby.
*/

#pragma once

#include "Utility/Utility.h"
#include "GameFramework/Actor.h"
#include "LobbyInfo.generated.h"

UCLASS()
class SNAKE_PROJECT_API ALobbyInfo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALobbyInfo();

	/** Launch Started Flag */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Lobby Info")
	bool bLaunchStarted;

	/** Level Information */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lobby Info")
	TArray<FMapItemData> MapData;

	/** Game Mode name and Short hand. Used for level travel */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lobby Info")
	TArray<FGameModeData> GameModeData;

	/** Colors that distinguish players on the same team */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lobby Info")
	TArray<FTeamColorData> TeamColorData;

	/** Get the Map data at the supplied index */
	UFUNCTION(BlueprintCallable, Category = "Lobby Info Helper")
	FMapItemData GetMapData(int32 Index);

	/** Get the Map at the Current Index */
	UFUNCTION(BlueprintCallable, Category = "Lobby Info Helper")
	FMapItemData GetCurrentMapData();

	/** Set the Level Index */
	UFUNCTION(BlueprintCallable, Category = "Lobby Info Helper")
	void SetLevelIndex(int32 Index);

	/** Set the Game Mode Text */
	UFUNCTION(BlueprintCallable, Category = "Lobby Info Helper")
	void SetGameMode(int32 Index);

	/** Get the Game Mode Data at this index*/
	UFUNCTION(BlueprintCallable, Category = "Lobby Info Helper")
	FGameModeData GetGameModeData(int32 Index);
	
	/** Get the Name of Game Mode Data at the replicated Index */
	UFUNCTION(BlueprintCallable, Category = "Lobby Info Helper")
	FString GetGameModeName() const;

	/** Get the Short Handed Name of the Game Mode Data at the Replicated Index */
	FString GetGameModeShortHand() const;

	FTeamColorData GetColorData(int32 Index);

protected:
	UPROPERTY(Replicated)
	int32 LevelIndex;
	
	UPROPERTY(Replicated)
	int32 GameModeIndex;

};
