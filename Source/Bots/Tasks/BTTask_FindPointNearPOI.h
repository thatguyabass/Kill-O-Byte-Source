// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FindPointNearPOI.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API UBTTask_FindPointNearPOI : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_FindPointNearPOI(const class FObjectInitializer& PCIP);

	UPROPERTY(EditAnywhere, Category = "Search")
	float SearchRadius;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
