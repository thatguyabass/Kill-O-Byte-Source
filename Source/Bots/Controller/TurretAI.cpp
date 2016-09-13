// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "TurretAI.h"
#include "Bots/Bot/Bot.h"
#include "Bots/Turret/SmartTurret.h"
#include "Bots/Controller/BotUtility.h"

ATurretAI::ATurretAI(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	bClampRotation = false;
	MinRotationClamp = 0.0f;
	MaxRotationClamp = 0.0f;

	PrimaryActorTick.bCanEverTick = true;
}

void ATurretAI::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	BaseRotation = InPawn ? InPawn->GetActorRotation() : FRotator::ZeroRotator;
}

void ATurretAI::InitializeBlackboardKeys()
{
	Super::InitializeBlackboardKeys();

	WaitKey = BlackboardComponent->GetKeyID("Wait");

	ASmartTurret* Turret = Cast<ASmartTurret>(GetPawn());

	if (Turret)
	{
		BlackboardComponent->SetValue<UBlackboardKeyType_Bool>(WaitKey, Turret->bWait);
	}
}

void ATurretAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FaceTarget(0.25f);
	ClampRotation();
}

void ATurretAI::FindTarget()
{
	ABot* MyBot = Cast<ABot>(GetPawn());
	if (!MyBot)
	{
		return;
	}

	const float SearchRadius = MyBot->SearchRadius;

	FName AISearch = FName(TEXT("AISearch"));
	FCollisionQueryParams Params(AISearch, false, this);

	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_Pawn);

	TArray<AActor*> PossibleTargets;
	UBotUtility::GetTeamBasedTargets(MyBot, PossibleTargets, SearchRadius, Params, ObjParams);

	AActor* Target = UBotUtility::GetClosestTarget(MyBot, PossibleTargets);

	if (Target)
	{
		SetTarget(Target);
	}
	else
	{
		SetTarget(nullptr);
	}
}

void ATurretAI::GetNextWaypoint()
{
	ASmartTurret* Turret = Cast<ASmartTurret>(GetPawn());
	if (Turret == nullptr)
	{
		return;
	}

	SetDestination(Turret->GetNextWaypoint());
}

void ATurretAI::ClampRotation()
{
	if (!bClampRotation)
	{
		return;
	}

	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return;
	}

	FRotator Rotation = MyPawn->GetActorRotation();
	Clamp(Rotation.Pitch, BaseRotation.Pitch);
	Clamp(Rotation.Yaw, BaseRotation.Yaw);
	Clamp(Rotation.Roll, BaseRotation.Roll);

	MyPawn->SetActorRotation(Rotation);
}

void ATurretAI::Clamp(float& Value, const float BaseValue)
{
	float Min = MinRotationClamp + BaseValue;
	float Max = MaxRotationClamp + BaseValue;
	if (Value < Min)
	{
		Value = Min;
	}
	else if (Value > Max)
	{
		Value = Max;
	}
}