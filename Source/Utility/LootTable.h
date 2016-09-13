// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "LootTable.generated.h"

USTRUCT()
struct FLootObject
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
	TSubclassOf<AActor> LootItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
	int32 DropChance;

};

UCLASS()
class SNAKE_PROJECT_API ULootTable : public UDataAsset
{
	GENERATED_BODY()

public:
	ULootTable(const class FObjectInitializer& PCIP);
	
	UPROPERTY(EditAnywhere, Category = "Loot Items")
	TArray<FLootObject> LootItems;

	/** Calculate which item dropped. If an item dropped, ItemDropped with be true and DroppedItem will be valid.
	*	Drop chance is calculated by checking the objects drop chance by a range of a 100. If the value lands within, the item has been dropped. 
	*/
	bool DropItem(TSubclassOf<AActor>& DroppedItem);

};
