// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BotManager.h"
#include "SingleBotSpawner.h"
#include "SnakeCharacter/SnakeLinkHead.h"

const int32 ABotManager::LazerIndex = 2;

// Sets default values
ABotManager::ABotManager()
{
	SearchRadius = 500.0f;
	AliveBotCheckTime = 0.25f;

	bFinished = false;

	SpawnDelay = 0.5f;

	NormalSpawnPoolCost = 100;
	SpecialSpawnPoolCost = 200;

	MaxSpawnPool = 800;
	SpawnPool = 0;
	MaxSpecial = 0;
	MaxAliveSpawned = 8;
	AliveBotCount = 0;

	SpecialBotCounts.Add(FSpecialCount("Shield", 1));
	SpecialBotCounts.Add(FSpecialCount("Mortar", 4));
	SpecialBotCounts.Add(FSpecialCount("Lazer", 2));
	SpecialBotCounts.Add(FSpecialCount("Med", 10));
	SpecialBotCounts.Add(FSpecialCount("Seeker", 4));

	WaveIndex = 1;
	MaxWaveIndex = 30;

	WaveHeaderDuration = 5.0f;
	WaveHeaderProgress = 0.0f;

	WaveGrowth.Add(FWaveGrowth("SpawnPool"));
	WaveGrowth.Add(FWaveGrowth("Special"));
	WaveGrowth.Add(FWaveGrowth("Alive"));

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABotManager::BeginPlay()
{
	Super::BeginPlay();
	
	FindSpawners();

	GetWorldTimerManager().SetTimer(AliveBotCheck_TimerHandle, this, &ABotManager::GetAliveBots, AliveBotCheckTime, true);

	SetWaveState(EWaveState::DoNotTick);
	MaxWaveIndex += 1;

	InitialLazerBotCount = SpecialBotCounts[LazerIndex].BotCount;
	SpecialBotCounts[LazerIndex].BotCount = 0;
}

void ABotManager::FindSpawners()
{
	for (TActorIterator<ASingleBotSpawner> Iter(GetWorld()); Iter; ++Iter)
	{
		if (*Iter)
		{
			Spawners.Add(*Iter);
		}
	}
}

// Called every frame
void ABotManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	switch (WaveState)
	{
	case EWaveState::PreWave: TickPreWave(DeltaTime); break;
	case EWaveState::WaveInprogress: TickWaveInprogress(DeltaTime); break;
	case EWaveState::PostWave: TickPostWave(DeltaTime); break;
	}
}

void ABotManager::TickPreWave(float DeltaTime)
{
	WaveHeaderProgress += DeltaTime;
	if (WaveHeaderProgress > WaveHeaderDuration)
	{
		WaveHeaderProgress = 0.0f;
		OnWaveStart();
	}
}

void ABotManager::TickWaveInprogress(float DeltaTime)
{
	if (CanSpawn())
	{
		SpawnProgress += DeltaTime;
		if (SpawnProgress > SpawnDelay)
		{
			SpawnProgress = 0.0f;
			SpawnBot();
		}
	}
	else if (AliveBotCount == 0)
	{
		OnWaveComplete();
	}
}

void ABotManager::TickPostWave(float DeltaTime)
{
	WaveHeaderProgress += DeltaTime;
	if (WaveHeaderProgress > WaveHeaderDuration)
	{
		WaveHeaderProgress = 0.0f;
		OnPostWaveComplete();
	}
}

void ABotManager::SpawnBot()
{
	if (Spawners.Num() <= 0)
	{
		return;
	}

	if (!RoomInSpawnPool())
	{
		//mark as finished if this is hit. 
		bFinished = true;
	}

	TSubclassOf<ABot> BotClass = nullptr;
	int32 Cost = NormalSpawnPoolCost;
	if (CanSpawnSpecial())
	{
		BotClass = GetSpecialBotClass();
		Cost = SpecialSpawnPoolCost;
	}
	else
	{
		BotClass = GetNormalBotClass();
	}
	
	int32 Best = GetBestSpawner();
	Spawners[Best]->SoftReset();
	Spawners[Best]->BotClasses.Reset();
	Spawners[Best]->BotClasses.Add(BotClass);
	Spawners[Best]->StartSpawning();

	SpawnPool += Cost;
	if (!RoomInSpawnPool())
	{
		bFinished = true;
	}
}

bool ABotManager::RoomInSpawnPool()
{
	return SpawnPool <= MaxSpawnPool;
}

int32 ABotManager::GetBestSpawner()
{
	int32 Smallest = INT32_MAX;
	TArray<FIntPoint> WeightedIndexes;
	TArray<FIntPoint> ValidIndexes;

	for (int32 c = 0; c < Spawners.Num(); c++)
	{
		FVector SpawnLocation = Spawners[c]->GetActorLocation();
		int32 Temp = CalculateSpawnWeight(SpawnLocation);
		WeightedIndexes.Add(FIntPoint(c, Temp));

		if (Temp <= Smallest)
		{
			Smallest = Temp;
		}
	}

	for (int32 c = 0; c < Spawners.Num(); c++)
	{
		int32 Temp = WeightedIndexes[c].Y;
		if (Temp == Smallest)
		{
			ValidIndexes.Add(WeightedIndexes[c]);
		}
	}

	int32 Index = FMath::RandRange(0, ValidIndexes.Num() - 1);

	return ValidIndexes[Index].X;
}

bool ABotManager::CanSpawn()
{
	return !bFinished && (AliveBotCount < MaxAliveSpawned);
}

bool ABotManager::CanSpawnSpecial()
{
	return (MaxSpecial >= 0) && ((SpawnPool + SpecialSpawnPoolCost) < MaxSpawnPool) && (AliveSpecialCount < MaxSpecial);
}

int32 ABotManager::CalculateSpawnWeight(FVector SpawnLocation)
{
	int32 Weight = 0;

	FName SpawnWeightCheck = FName(TEXT("SpawnWeightCheck"));
	FCollisionQueryParams Params(SpawnWeightCheck, false, this);
	TArray<FOverlapResult> OverlapOut;

	GetWorld()->OverlapMultiByChannel(OverlapOut, SpawnLocation, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(SearchRadius), Params);

	for (int32 c = 0; c < OverlapOut.Num(); c++)
	{
		APawn* Pawn = Cast<APawn>(OverlapOut[c].GetActor());
		if (Pawn)
		{
			Weight++;
		}
	}

	return Weight;
}

TSubclassOf<ABot> ABotManager::GetNormalBotClass()
{
	int32 Index = FMath::RandRange(0, NormalBots.Num() - 1);

	if (NormalBots.IsValidIndex(Index))
	{
		return NormalBots[Index];
	}

	return nullptr;
}

TSubclassOf<ABot> ABotManager::GetSpecialBotClass()
{
	// 5 total special bot types
	int32 SpecialCount = 5;
	//The Sepcial Bot Types start at Index 3 (BotStructs.h)
	const int32 SpecialBotIndex = 3;
	TArray<int32> Index;

	for (int32 c = 0; c < SpecialCount; c++)
	{
		uint8 Type = c + SpecialBotIndex;

		int32 Count = GetAliveSpecialBots(Type);
		if (Count < SpecialBotCounts[c].BotCount)
		{
			Index.Add(c);
		}
	}

	int32 BotIndex = Index[FMath::RandRange(0, Index.Num() - 1)];

	switch (BotIndex)
	{
	case 0: return ShieldBots[	FMath::RandRange(0, ShieldBots.Num()	- 1)]; break;
	case 1: return MortarBots[	FMath::RandRange(0, MortarBots.Num()	- 1)]; break;
	case 2: return LazerBots[	FMath::RandRange(0, LazerBots.Num()		- 1)]; break;
	case 3: return MedBots[		FMath::RandRange(0, MedBots.Num()		- 1)]; break;
	case 4: return SeekerBots[	FMath::RandRange(0, SeekerBots.Num()	- 1)]; break;
	}


	return nullptr;
}

void ABotManager::GetAliveBots()
{
	AliveBotCount = 0;
	AliveSpecialCount = 0;

	for (int32 c = 0; c < Spawners.Num(); c++)
	{
		AliveBotCount += Spawners[c]->GetAliveBotCount();
		AliveSpecialCount += Spawners[c]->GetSpecialBotCount();
	}
}

int32 ABotManager::GetAliveSpecialBots(uint8 InBotType)
{
	int32 Count = 0;

	for (int32 c = 0; c < Spawners.Num(); c++)
	{
		Count += Spawners[c]->GetTargetTypeBotCount(InBotType);
	}

	return Count;
}

bool ABotManager::SpawningFinished()
{
	return bFinished;
}

void ABotManager::OnPreWaveStart_Implementation()
{
	bFinished = false;
}

void ABotManager::OnWaveStart_Implementation()
{
	SetWaveState(EWaveState::WaveInprogress);
}

void ABotManager::OnWaveComplete_Implementation()
{
	SetWaveState(EWaveState::PostWave);
}

void ABotManager::OnPostWaveComplete_Implementation()
{
	SpawnPool = 0;
	IncreaseWave();
	SetWaveState(EWaveState::PreWave);
}

void ABotManager::SetWaveState(EWaveState InWaveState)
{
	WaveState = InWaveState;

	OnWaveStateChange(WaveState);

	HandleWaveStateChange();
}

void ABotManager::HandleWaveStateChange()
{
	switch (WaveState)
	{
	case EWaveState::PreWave: OnPreWaveStart(); break;
	case EWaveState::WaveInprogress: break;
	case EWaveState::PostWave: break;
	}
}

bool ABotManager::IsPreWave() const
{
	return WaveState == EWaveState::PreWave;
}

bool ABotManager::IsWaveInprogress() const
{
	return WaveState == EWaveState::WaveInprogress;
}

bool ABotManager::IsPostWave() const
{
	return WaveState == EWaveState::PostWave;
}

void ABotManager::IncreaseWave()
{
	ArcadeGrowth();
	WaveIndex++;

	if (WaveIndex >= 6)
	{
		SpecialBotCounts[LazerIndex].BotCount = InitialLazerBotCount;
	}
}

void ABotManager::ArcadeGrowth()
{
	MaxSpawnPool	= ScaleBotLimits(WaveGrowth[0]);
	MaxSpecial		= ScaleBotLimits(WaveGrowth[1]);
	MaxAliveSpawned	= ScaleBotLimits(WaveGrowth[2]);
}

int32 ABotManager::ScaleBotLimits(const FWaveGrowth& InWaveGrowth)
{
	return ScaleBotLimits(InWaveGrowth.Base, InWaveGrowth.Growth);
}

int32 ABotManager::ScaleBotLimits(const float& Base, const float& Growth)
{
	float Exp = pow(Growth, WaveIndex);

	return FMath::FloorToInt(Base * Exp);
}

void ABotManager::RemoveWavePopupWidget()
{
	if (WavePopupWidget)
	{
		WavePopupWidget->RemoveFromParent();
		WavePopupWidget = nullptr;
	}
}

void ABotManager::RemoveWaveCompleteWidget()
{
	if (WaveCompleteWidget)
	{
		WaveCompleteWidget->RemoveFromParent();
		WaveCompleteWidget = nullptr;
	}
}

void ABotManager::EndMatch()
{
	SetWaveState(EWaveState::DoNotTick);

	for (int32 c = 0; c < Spawners.Num(); c++)
	{
		Spawners[c]->Reset();
	}

	RemoveWavePopupWidget();
	RemoveWaveCompleteWidget();
}