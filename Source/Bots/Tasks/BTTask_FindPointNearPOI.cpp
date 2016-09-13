// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BTTask_FindPointNearPOI.h"
#include "Bots/Controller/GruntAI.h"
#include "Bots/Bot/Bot.h"
#include "Bots/PointOfInterest.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UBTTask_FindPointNearPOI::UBTTask_FindPointNearPOI(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	SearchRadius = 600.0f;
	BlackboardKey.AddVectorFilter(this, "FVector");
}

EBTNodeResult::Type UBTTask_FindPointNearPOI::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBehaviorTreeComponent& MyComp = OwnerComp;
	AGruntAI* Controller = Cast<AGruntAI>(MyComp.GetOwner());
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	ABot* Bot = Cast<ABot>(Controller->GetPawn());
	APointOfInterest* POI = Controller->GetPointOfInterest();
	if (Bot && POI)
	{
		const FVector SearchOrigin = POI->GetActorLocation();
		FVector Location = UNavigationSystem::GetRandomReachablePointInRadius(Controller, SearchOrigin, SearchRadius);
		if (Location != FVector::ZeroVector)
		{
			Controller->GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), Location);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
