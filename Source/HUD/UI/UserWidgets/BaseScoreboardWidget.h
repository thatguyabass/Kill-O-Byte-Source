// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"
#include "Blueprint/UserWidget.h"
#include "BaseScoreboardWidget.generated.h"

USTRUCT()
struct FScoreboardUpdateInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "Info")
	bool bRank;

	UPROPERTY(EditAnywhere, Category = "Info")
	bool bName;

	UPROPERTY(EditAnywhere, Category = "Info")
	bool bLink;

	UPROPERTY(EditAnywhere, Category = "Info")
	bool bSnakeKill;

	UPROPERTY(EditAnywhere, Category = "Info")
	bool bDeath;

	UPROPERTY(EditAnywhere, Category = "Info")
	bool bScore;

	static const int32 Rank = 0;
	static const int32 Name = 1;
	static const int32 Link = 2;
	static const int32 Kill = 3;
	static const int32 Death = 4;
	static const int32 Score = 5;

	TArray<int32> UpdateOrder;

	void FillOrder()
	{
		UpdateOrder.Empty();

		if (bRank)
		{
			UpdateOrder.Add(Rank);
		}
		if (bName)
		{
			UpdateOrder.Add(Name);
		}
		if (bLink)
		{
			UpdateOrder.Add(Link);
		}
		if (bSnakeKill)
		{
			UpdateOrder.Add(Kill);
		}
		if (bDeath)
		{
			UpdateOrder.Add(Death);
		}
		if (bScore)
		{
			UpdateOrder.Add(Score);
		}
	}
};

UCLASS()
class SNAKE_PROJECT_API UBaseScoreboardWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Scoreboard Information")
	FScoreboardUpdateInfo ScoreboardInfo;

	UPROPERTY(BlueprintReadOnly, Category = "Player Information")
	TArray<FRankMapStruct> PlayerList;

	UPROPERTY(BlueprintReadWrite, Category = "Scoreboard Information")
	TArray<class UVerticalBox*> Boxes;

	UPROPERTY(BlueprintReadWrite, Category = "Team Scoreboard Information")
	TArray<class UVerticalBox*> BoxesTeam0;

	UPROPERTY(BlueprintReadWrite, Category = "Team Scoreboard Information")
	TArray<class UVerticalBox*> BoxesTeam1;

	UPROPERTY(BlueprintReadWrite, Category = "Team Scoreboard Information")
	TArray<class UVerticalBox*> BoxesTeam2;

	UPROPERTY(BlueprintReadWrite, Category = "Team Scoreboard Information")
	TArray<class UVerticalBox*> BoxesTeam3;

	/** Generate a sorted list to update the players */
	UFUNCTION(BlueprintCallable, Category = "Player Information")
	void GeneratePlayerList(int32 Index = -1);
	
	/** Add the new items to the box */
	UFUNCTION(BlueprintCallable, Category = "Player Information")
	void AddChildren(class UVerticalBox* InBox, class UWidget* NewItem);

	/** Fill the Passed in boxes with the correct information */
	UFUNCTION(BlueprintCallable, Category = "Scoreboard Information")
	void UpdateScoreboard(int32 TeamNumber = -1);

	/** This function can be called from a blueprint and trigger teh FillBoxes Event. This feels like such a hack. */
	UFUNCTION(BlueprintCallable, Category = "Scoreboard Information")
	void CallFillBoxes();

	/** Trigger the Fill Boxes Event */
	UFUNCTION(BlueprintImplementableEvent, Category = "Scoreboard Information")
	void FillBoxes();

	UFUNCTION(BlueprintCallable, Category = "Player Information")
	int32 GetOwningTeamNumber();

private:

	/** Update the Item with the Correct Data */
	void UpdateItem(class UScoreboardItem* InItem, int32 UpdateIndex);

};
