// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SingleBotSpawner.h"
#include "Bots/Bot/Bot.h"
#include "Bots/POIGroup.h"

// Sets default values
ASingleBotSpawner::ASingleBotSpawner()
{
	bSpawnAtStart = true;
	bDestroyAfterSpawn = false;

	bFinished = false;

	SpawnParams = ESpawnParameters::Farthest;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASingleBotSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	if (bSpawnAtStart)
	{
		StartSpawning();
	}

	for (TActorIterator<ASnakePlayer> Iter(GetWorld()); Iter; ++Iter)
	{
		if (*Iter)
		{
			PlayerControllers.Add(*Iter);
		}
	}
}

void ASingleBotSpawner::Tick(float DeltaTime)
{
	if (SpawnedBots.Num() > 0)
	{
		for (int32 c = 0; c < SpawnedBots.Num(); c++)
		{
			if (SpawnedBots[c]->IsDead())
			{
				SpawnedBots.RemoveAt(c);
				c--;
			}
		}
	}
}

void ASingleBotSpawner::SpawnBot()
{
	if (!CanSpawn())
	{
		return;
	}

	if (BotClasses.Num() <= 0)
	{
		return;
	}

	int32 Ran = GetSpawnIndex();

	if (BotClasses.IsValidIndex(Ran))
	{
		const FVector Location = GetSpawnLocation();
		const FRotator Rotation = GetActorRotation();

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		ABot* Bot = GetWorld()->SpawnActor<ABot>(BotClasses[Ran], Location, Rotation, Params);

		LatestBot(Bot);

		MarkFinished();
	}

	if (bFinished && bDestroyAfterSpawn)
	{
		SetLifeSpan(2.0f);
	}
}

int32 ASingleBotSpawner::GetSpawnIndex()
{
	return FMath::RandRange(0, BotClasses.Num() - 1);
}

bool ASingleBotSpawner::CanSpawn()
{
	return !bFinished;
}

void ASingleBotSpawner::StartSpawning()
{
	SpawnBot();
}

void ASingleBotSpawner::LatestBot(ABot* Bot)
{
	if (Bot)
	{
		LastSpawnedBot = Bot;
		SpawnedBots.Add(Bot);

		if (POIGroup)
		{
			Bot->SetPointOfInterestGroup(POIGroup->Group);
		}
	}
}

ABot* ASingleBotSpawner::GetLatestBot()
{
	return LastSpawnedBot;
}

void ASingleBotSpawner::MarkFinished()
{
	bFinished = true;
}

bool ASingleBotSpawner::IsFinishedSpawning()
{
	return bFinished;
}

bool ASingleBotSpawner::IsFinishedWithNoAliveBots()
{
	return (bFinished && GetSpawnedBots().Num() == 0);
}

int32 ASingleBotSpawner::GetAliveBotCount()
{
	int32 Alive = 0;

	for (int32 c = 0; c < SpawnedBots.Num(); c++)
	{
		if (!SpawnedBots[c]->IsDead())
		{
			Alive++;
		}
	}

	return Alive;
}

int32 ASingleBotSpawner::GetSpecialBotCount()
{
	int32 Special = 0;

	for (int32 c = 0; c < SpawnedBots.Num(); c++)
	{
		if (SpawnedBots[c]->BotType == EBotType::Special)
		{
			Special++;
		}
	}

	return Special;
}

int32 ASingleBotSpawner::GetTargetTypeBotCount(uint8 BotTypeTarget)
{
	int32 Count = 0;

	for (int32 c = 0; c < SpawnedBots.Num(); c++)
	{
		if (SpawnedBots[c]->BotClass == TEnumAsByte<EBotClass>(BotTypeTarget))
		{
			Count++;
		}
	}

	return Count;
}

void ASingleBotSpawner::SoftReset()
{
	bFinished = false;
}

void ASingleBotSpawner::Reset()
{
	bFinished = false;

	if (SpawnedBots.Num() > 0)
	{
		for (int32 c = 0; c < SpawnedBots.Num(); c++)
		{
			SpawnedBots[c]->DestroyTime = 1.0f;
			SpawnedBots[c]->HideAndDestroy();
		}
	}
	SpawnedBots.Empty();

	if (GetLifeSpan() != 0.0f)
	{
		SetLifeSpan(0.0f);
	}
}

TArray<ABot*>& ASingleBotSpawner::GetSpawnedBots()
{
	return SpawnedBots;
}

void ASingleBotSpawner::SetAdditionalSpawnPoints(TArray<FVector> Locations)
{
	AdditionalSpawnLocations.Reset();
	if (Locations.Num() <= 0)
	{
		return;
	}

	AdditionalSpawnLocations.Add(GetActorLocation());
	for (int32 c = 0; c < Locations.Num(); c++)
	{
		FVector Location = Locations[c] + GetActorLocation();
		AdditionalSpawnLocations.Add(Location);
	}
}

FVector ASingleBotSpawner::GetSpawnLocation()
{
	//If no additional Spawn Locations were set, use this actor's location
	if (AdditionalSpawnLocations.Num() <= 0)
	{
		return GetActorLocation();
	}

	FVector Closest = FVector::ZeroVector;
	FVector Farthest = FVector::ZeroVector;
	FVector Inbetween = FVector::ZeroVector;
	FVector Random = FVector::ZeroVector;

	float Min = 0.0f;
	float Max = MAX_FLT;

	for (int32 c = 0; c < PlayerControllers.Num(); c++)
	{
		APawn* Pawn = PlayerControllers[c]->GetPawn();
		if (Pawn)
		{
			for (int32 LocationIndex = 0; LocationIndex < AdditionalSpawnLocations.Num(); LocationIndex++)
			{
				float Distance = (Pawn->GetActorLocation() - AdditionalSpawnLocations[LocationIndex]).Size();
				if (Distance > Min)
				{
					Min = Distance;
					Farthest = AdditionalSpawnLocations[LocationIndex];
				}

				if (Distance < Max)
				{
					Max = Distance;
					Closest = AdditionalSpawnLocations[LocationIndex];
				}
			}
		}
	}
	
	if (Closest == FVector::ZeroVector)
	{
		Closest = GetActorLocation();
	}
	if (Farthest == FVector::ZeroVector)
	{
		Farthest = GetActorLocation();
	}

	if (SpawnParams == ESpawnParameters::Inbetween)
	{
		TArray<FVector> InbetweenLocations;
		for (int32 c = 0; c < AdditionalSpawnLocations.Num(); c++)
		{
			if (AdditionalSpawnLocations[c] == Closest || AdditionalSpawnLocations[c] == Farthest)
			{
				continue;
			}

			InbetweenLocations.Add(AdditionalSpawnLocations[c]);
		}

		if (InbetweenLocations.Num() > 0)
		{
			int32 RandIndex = FMath::RandRange(0, InbetweenLocations.Num() - 1);
			Inbetween = InbetweenLocations[RandIndex];
		}
		else
		{
			FString Name = GetName();
			GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, Name + ": Not enough additional spawn points to use Inbetween spawn params. SingleBotSpawner.cpp (247)");
			Inbetween = GetActorLocation();
		}
	}
	else if (SpawnParams == ESpawnParameters::Random)
	{
		int32 RandIndex = FMath::RandRange(0, AdditionalSpawnLocations.Num() - 1);
		Random = AdditionalSpawnLocations[RandIndex];
	}

	FVector Location = FVector::ZeroVector;
	switch (SpawnParams)
	{
	case ESpawnParameters::Closest:		Location = Closest;		break;
	case ESpawnParameters::Inbetween:	Location = Inbetween;	break;
	case ESpawnParameters::Farthest:	Location = Farthest;	break;
	case ESpawnParameters::Random:		Location = Random;		break;
	default: Location = GetActorLocation(); break;
	}

	return Location;
}