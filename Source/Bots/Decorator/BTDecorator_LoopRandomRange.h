// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/Decorators/BTDecorator_Loop.h"
#include "BTDecorator_LoopRandomRange.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API UBTDecorator_LoopRandomRange : public UBTDecorator_Loop
{
	GENERATED_BODY()
	
public:
	/** Min Range <= */
	UPROPERTY(EditAnywhere, Category = "Decorator")
	int32 MinRange;

	/** Max Range  >= */
	UPROPERTY(EditAnywhere, Category = "Decorator")
	int32 MaxRange;

protected:
	virtual void OnNodeActivation(FBehaviorTreeSearchData& SearchData) override;

};
