// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BaseSpawner.h"

ABaseSpawner::ABaseSpawner(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	RootBoxComp = PCIP.CreateDefaultSubobject<UBoxComponent>(this, TEXT("RootBoxComp"));
	RootBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootBoxComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootBoxComp->bGenerateOverlapEvents = false;
	RootComponent = RootBoxComp;

	bPerformLineTrace = true;

	TraceDepth = 500.0f;

	PrimaryActorTick.bCanEverTick = true;
}

void ABaseSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (Content.Num() <= 0)
	{
		PrimaryActorTick.bCanEverTick = false;
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, "No Content was set in Spawner:" + GetName());
	}
	VarifySpawnChances();
}

void ABaseSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseSpawner::Spawn()
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (Content.Num() > 0)
		{
			int32 Index = GetSpawnIndex();
			FHitResult Hit(ForceInit);

			FRotator Rot = Content[Index].Rotation;
			FVector Loc = FVector::ZeroVector;

			FName Name = "PickupTrace";
			FCollisionQueryParams Params(Name, false, this);
			
			if (bPerformLineTrace)
			{
				// Counter measure to an infinate loop. 
				int32 LoopCounter = 0;
				do
				{
					LoopCounter++;

					Loc = GetSpawnLocation();

					const FVector Up = FVector::UpVector * TraceDepth;
					const FVector Start = Loc + Up;
					const FVector End = Loc + (Up * -1);

					GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params);
					GetWorld()->DebugDrawTraceTag = Name;
				} while (!Hit.IsValidBlockingHit() && LoopCounter < 10);

				if (LoopCounter >= 10)
				{
					return;
				}

				if (Hit.IsValidBlockingHit())
				{
					Loc.Z = Hit.Location.Z + 110.0f;
				}
			}
			else
			{
				Loc = GetSpawnLocation();
			}

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			SpawnedActor = GetWorld()->SpawnActor<AActor>(Content[Index].Actor, Loc, Rot, SpawnParams);
		}
	}
}

FVector ABaseSpawner::GetSpawnLocation()
{
	FVector BoxLoc = RootBoxComp->Bounds.Origin;
	FVector Bounds = RootBoxComp->Bounds.BoxExtent;

	float MinX = BoxLoc.X - Bounds.X;
	float MaxX = BoxLoc.X + Bounds.X;

	float MinY = BoxLoc.Y - Bounds.Y;
	float MaxY = BoxLoc.Y + Bounds.Y;

	float X = FMath::FRandRange(MinX, MaxX);
	float Y = FMath::FRandRange(MinY, MaxY);
	float Z = BoxLoc.Z;

	return FVector(X, Y, Z);
}

void ABaseSpawner::VarifySpawnChances()
{
	if (Content.Num() <= 0)
	{
		Content[0].SpawnChance = 100;
		return;//No need to do the additional checks
	}

	int32 Min = 0;
	int32 Max = 100;

	int32 Total = 0;

	for (int32 c = 0; c < Content.Num(); c++)
	{
		Total += Content[c].SpawnChance;
	}

	if (Total != Max)
	{
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, "Total didn't reach 100 :" + this->GetName() + ". Spawn chance has been modified");
		for (int32 c = 0; c < Content.Num(); c++)
		{
			int32 v = Content[c].SpawnChance;
			int32 p = v / Total;
			Content[c].SpawnChance = p * Max;
		}
	}
}

int32 ABaseSpawner::GetSpawnIndex()
{
	int32 Min = 0;
	int32 Max = 100;
	int32 Chance = FMath::RandRange(Min, Max);

	int32 Index = -1;

	for (int32 c = 0; c < Content.Num(); c++)
	{
		Max = Min + Content[c].SpawnChance;

		if (Chance >= Min && Chance <= Max)
		{
			Index = c;
			break;
		}
		Min = Max;
	}

	if (Index == -1)
	{
		Index = 0;
	}

	return Index;
}

//void ABaseSpawner::ServerSpawn_Implementation(int32 ContentIndex, FVector Location, FRotator Rotation)
//{
//	FTransform SpawnTrans(Rotation, Location);
//	SpawnedPickup = Cast<APickup>(UGameplayStatics::BeginSpawningActorFromClass(this, Content[ContentIndex].Actor, SpawnTrans));
//	if (SpawnedPickup)
//	{
//		SpawnedPickup->SetOwner(this);
//		SpawnedPickup->Instigator = Instigator;
//
//		UGameplayStatics::FinishSpawningActor(SpawnedPickup, SpawnTrans);
//	}
//}