// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BTDecorator_LoopRandomRange.h"

void UBTDecorator_LoopRandomRange::OnNodeActivation(FBehaviorTreeSearchData& SearchData)
{
	NumLoops = FMath::RandRange(MinRange, MaxRange);

	Super::OnNodeActivation(SearchData);
}