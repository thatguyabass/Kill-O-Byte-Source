// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "LootTable.h"

ULootTable::ULootTable(const FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

bool ULootTable::DropItem(TSubclassOf<AActor>& DroppedItem)
{
	const int32 Min = 1;
	const int32 Max = 100;

	for (int32 c = 0; c < LootItems.Num(); c++)
	{
		int32 Rand = FMath::RandRange(Min, Max);
		if (Rand <= LootItems[c].DropChance)
		{
			DroppedItem = LootItems[c].LootItem;
			return true;
		}
	}

	return false;
}
