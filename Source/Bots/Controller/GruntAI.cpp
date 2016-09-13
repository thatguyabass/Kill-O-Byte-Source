// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "GruntAI.h"
#include "Bots/Bot/Bot.h"
#include "Bots/PointOfInterest.h"
#include "BotUtility.h"

AGruntAI::AGruntAI(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	MinDistance = 400.0f;
	MaxDistance = 600.0f;

	OrbitRadius = 700.0f;
	BaseMovementSpeed = 0.0f;
	OrbitLineTraceDepth = 500.0f;

	OrbitDuration = 10;
}

void AGruntAI::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	ABot* MyBot = Cast<ABot>(GetPawn());
	if (MyBot)
	{
		BaseMovementSpeed = MyBot->GetCharacterMovement()->MaxWalkSpeed;
	}
}

void AGruntAI::InitializeBlackboardKeys()
{
	Super::InitializeBlackboardKeys();

	InterestKey = BlackboardComponent->GetKeyID("PointOfInterest");
	InRangeKey = BlackboardComponent->GetKeyID("InRange");
	StoredDirectionKey = BlackboardComponent->GetKeyID("StoredDirection");
}

void AGruntAI::FindTarget()
{
	ABot* MyBot = Cast<ABot>(GetPawn());
	if (!MyBot)
	{
		return;
	}

	const float SearchRadius = MyBot->SearchRadius;

	FName AISearch = FName(TEXT("AISearch"));
	FCollisionQueryParams Params(AISearch, false, this);

	FCollisionObjectQueryParams ObjParams = GetObjectQuery();

	TArray<AActor*> PossibleTargets;
	UBotUtility::GetTeamBasedTargets(MyBot, PossibleTargets, SearchRadius, Params, ObjParams);

	float Max = FLT_MAX;
	float Distance;
	int32 Index = 0;

	for (int32 c = 0; c < PossibleTargets.Num(); c++)
	{
		Distance = (PossibleTargets[c]->GetActorLocation() - MyBot->GetActorLocation()).Size();
		if (Distance < Max)
		{
			Max = Distance;
			Index = c;
		}
	}

	if (PossibleTargets.IsValidIndex(Index))
	{
		SetTarget(PossibleTargets[Index]);
	}
	else
	{
		SetTarget(nullptr);
	}
}

void AGruntAI::MaintainDistanceCustom(const float MinimumDistance, const float MaximumDistance)
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return;
	}

	AActor* Target = GetValue<AActor, UBlackboardKeyType_Object>(TargetKey);
	if (Target)
	{
		// Get the Direction between the pawn and target
		FVector Direction = MyPawn->GetActorLocation() - Target->GetActorLocation();
		float Distance = Direction.Size();

		float Min = MinimumDistance;
		bool bSight = CheckLineOfSightAdvanced(MyPawn, Target);
		//if (!bSight)
		//{
		//	//Reduce the Min to a really low value. 
		//	//This will ensure the Bot moves to the target dispite being too too close.
		//	Min = 100;
		//}

		// Check if we are outside the max distance from the target
		if (Distance > MaximumDistance)
		{
			//This is broken in 4.7.1 - Fixed in 4.7.4
			//Move to actor doesn't update if the goal is moving 
			MoveToActor(Target, 0.0f, true, true, true);
		}

		// Check if we are too close to the target
		else if (Distance < Min)
		{
			FVector DesiredLocation = FVector::ZeroVector;

			if (bSight)
			{
				const float AverageDistance = (Min + MaximumDistance) / 2;

				Direction.Normalize();
				DesiredLocation = Target->GetActorLocation() + Direction * AverageDistance;
				FVector Adjustment = FVector::ZeroVector;
				bool bHit = GetWorld()->EncroachingBlockingGeometry(MyPawn, DesiredLocation, MyPawn->GetActorRotation(), &Adjustment);
				if (bHit)
				{
					DesiredLocation += Adjustment;
				}
			}
			else
			{
				float TargetRadius = 750.0f;
				DesiredLocation = UNavigationSystem::GetRandomReachablePointInRadius(this, Target->GetActorLocation(), TargetRadius);
			}

			if (DesiredLocation != FVector::ZeroVector)
			{
				SetDestination(DesiredLocation);
				MoveToLocation(DesiredLocation);
			}
		}
		else
		{
			if (bSight)
			{
				StopMovement();
			}
			else
			{
				MoveToActor(Target);
			}
		}
	}
}

void AGruntAI::MaintainDistance()
{
	MaintainDistanceCustom(MinDistance, MaxDistance);
}

void AGruntAI::FindPointOfInterest()
{
	ABot* MyBot = Cast<ABot>(GetPawn());
	if (!MyBot)
	{
		return;
	}

	APointOfInterest* POI = GetValue<APointOfInterest, UBlackboardKeyType_Object>(InterestKey);
	if (POI)
	{
		//Reduce the Weight of Old POI
		POI->ReduceAtLocation();
	}
	
	TArray<APointOfInterest*> Out;
	MyBot->GetPointsOfInterest(Out);

	// Get the Lowest Weighted Point Of Interest
	int32 LowestWeight = INT32_MAX;
	for (int32 c = 0; c < Out.Num(); c++)
	{
		//Skip the old POI
		if (POI && POI == Out[c])
		{
			continue;
		}

		int32 Value = Out[c]->GetAtLocation();
		if (Value < LowestWeight)
		{
			LowestWeight = Value;
		}
	}

	//Sort through the Array getting all the Points that have the lowest weight
	TArray<APointOfInterest*> LowWeightPOI;
	for (int32 c = 0; c < Out.Num(); c++)
	{
		int32 Value = Out[c]->GetAtLocation();
		if (Value == LowestWeight)
		{
			LowWeightPOI.Add(Out[c]);
		}
	}

	// No POI in the Level
	if (LowWeightPOI.Num() <= 0)
	{
		return;
	}

	int32 Random = 0;
	do
	{
		Random = FMath::RandRange(0, LowWeightPOI.Num() - 1);
	} while ((Out.IsValidIndex(Random) == false) || (LowWeightPOI[Random] == POI));

	POI = LowWeightPOI[Random];

	if (POI)
	{
		//Increase the Weight and set the current POI
		POI->IncreaseAtLocation();
		SetPointOfInterest(POI);
	}
}

void AGruntAI::SetPointOfInterest(APointOfInterest* Interest)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValue<UBlackboardKeyType_Object>(InterestKey, Interest);
		SetFocus(Interest);
	}
}

void AGruntAI::SetInRange(bool InRange)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValue<UBlackboardKeyType_Bool>(InRangeKey, InRange);
	}
}

void AGruntAI::SetStoredDirection(FVector StoredDirection)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValue<UBlackboardKeyType_Vector>(StoredDirectionKey, StoredDirection);
	}
}

APointOfInterest* AGruntAI::GetPointOfInterest()
{
	if (BlackboardComponent)
	{
		return GetValue<APointOfInterest, UBlackboardKeyType_Object>(InterestKey);
	}

	return nullptr;
}

FCollisionObjectQueryParams AGruntAI::GetObjectQuery()
{
	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_Pawn);

	return ObjParams;
}

bool AGruntAI::InRangeOfTarget()
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return false;
	}

	AActor* Target = GetValue<AActor, UBlackboardKeyType_Object>(TargetKey);
	if (Target)
	{
		FVector D = Target->GetActorLocation() - MyPawn->GetActorLocation();
		float Distance = D.Size();

		if (Distance < MaxDistance)
		{
			return true;
		}
	}

	return false;
}

void AGruntAI::OrbitTarget()
{
	ABot* MyBot = Cast<ABot>(GetPawn());
	if (!MyBot)
	{
		return;
	}

	AActor* Target = GetValue<AActor, UBlackboardKeyType_Object>(TargetKey);
	if (Target)
	{
		FaceTarget(10.0f);

		float DeltaTime = GetWorld()->GetDeltaSeconds();
		if (OrbitProgress > OrbitDuration)
		{
			OrbitProgress = 0;
			OrbitAmount = FRotator::ZeroRotator;
		}
		OrbitProgress += DeltaTime;

		//Rotate around Yaw
		FRotator BotRotation = FRotator(0.0f, 1.0f, 0.0f) * OrbitDirection;
		OrbitAmount = BotRotation * (360.0f * (OrbitProgress / OrbitDuration));

		FVector Direction = BlackboardComponent ? BlackboardComponent->GetValue<UBlackboardKeyType_Vector>(StoredDirectionKey) : FVector::ZeroVector;
		FVector Offset = Direction * OrbitRadius;
		Offset = OrbitAmount.RotateVector(Offset);

		FVector TargetLocation = Target->GetActorLocation();
		FVector DesiredLocation = Offset + TargetLocation;

		//Get the Height of the Terrain the bot is on. 
		const FVector Up = MyBot->GetActorUpVector() * OrbitLineTraceDepth;
		const FVector Down = Up * -1;
		const FVector Start = MyBot->GetActorLocation() + Up;
		const FVector End = MyBot->GetActorLocation() + Down;
		const FName TraceName("OrbitHeightCheck");

		FCollisionQueryParams Params(TraceName, false, MyBot);
		FCollisionObjectQueryParams ObjParam;
		ObjParam.AddObjectTypesToQuery(ECC_WorldStatic);

		FHitResult Hit(ForceInit);
		bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjParam, Params);
		if (bHit)
		{
			//Modify the Height to match the terrain
			DesiredLocation.Z = Hit.Location.Z + 75.0f;
		}

		SetDestination(DesiredLocation);
		MoveToLocation(DesiredLocation, -1.0f, false, true, false, true);
	}
}

void AGruntAI::InitializeOrbit()
{
	ABot* MyBot = Cast<ABot>(GetPawn());
	if (!MyBot)
	{
		return;
	}

	AActor* Target = GetValue<AActor, UBlackboardKeyType_Object>(TargetKey);
	if (Target)
	{
		bool bLastFrame = BlackboardComponent ? BlackboardComponent->GetValue<UBlackboardKeyType_Bool>(InRangeKey) : false;
		bool bThisFrame = InRangeOfTarget();
		if (bThisFrame != bLastFrame)
		{
			//We are leaving the Orbit
			if (bLastFrame)
			{
				MyBot->GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
			}
			//We are entering the Orbit
			else
			{
				FVector TargetLocation = Target->GetActorLocation();
				FVector MyLocation = MyBot->GetActorLocation();

				FVector Direction = MyLocation - TargetLocation;
				Direction.Normalize();

				SetStoredDirection(Direction);

				OrbitDirection = FMath::RandRange(0, 1) == 0 ? -1.0f : 1.0f;

				OrbitDistance = 2 * 3.14 * OrbitRadius;
				float Speed = OrbitDistance / OrbitDuration;
				OrbitProgress = 0.0f;
			
				MyBot->GetCharacterMovement()->MaxWalkSpeed = Speed;
			}
		}

		SetInRange(bThisFrame);
	}
	else
	{
		SetInRange(false);
		MyBot->GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
}