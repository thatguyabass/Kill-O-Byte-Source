// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/BotStructs.h"
#include "GameFramework/Actor.h"
#include "BotManager.generated.h"

USTRUCT(BlueprintType)
struct FWaveGrowth
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = "Arcade Growth")
	FString Name;

	/** Base Value of Growth */
	UPROPERTY(EditAnywhere, Category = "Arcade Growth")
	float Base;

	/** Growth rate of the Arcade Count */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arcade Growth")
	float Growth;

	FWaveGrowth() 
	{
		Base = 1;
		Growth = 1;
	}

	FWaveGrowth(FString _Name)
	{
		Name = _Name;
		Base = 1;
		Growth = 1;
	}
};

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	PreWave,
	WaveInprogress,
	PostWave,
	DoNotTick
};

USTRUCT(BlueprintType)
struct FSpecialCount
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = "Name")
	FString Name;

	UPROPERTY(EditAnywhere, Category = "Count")
	int32 BotCount;

	FSpecialCount() { }

	FSpecialCount(FString _Name, int32 _BotCount)
	{
		Name = _Name;
		BotCount = _BotCount;
	}
};

UCLASS()
class SNAKE_PROJECT_API ABotManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABotManager();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** Values which the Bots spawn limits will be increased by */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manager|Wave Growth")
	TArray<FWaveGrowth> WaveGrowth;

	/** Range to determine how long till the next bot spawns */
	UPROPERTY(EditAnywhere, Category = "Manager|Range")
	float SpawnDelay;

	/** Spawn Pool */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manager|Pool")
	int32 MaxSpawnPool;

	UPROPERTY(EditAnywhere, Category = "Manager|Pool")
	int32 NormalSpawnPoolCost;

	UPROPERTY(EditAnywhere, Category = "Manager|Pool")
	int32 SpecialSpawnPoolCost;

	UPROPERTY(BlueprintReadOnly, Category = "Manager|Pool")
	int32 SpawnPool;

	/** Max number of special bots alive at a time */
	UPROPERTY(EditAnywhere, Category = "Manager|Pool")
	int32 MaxSpecial;

	/** Max number of bots alive at a time */
	UPROPERTY(EditAnywhere, Category = "Manager|Pool")
	int32 MaxAliveSpawned;

	/** All the Grunt Bots and Drones Classes */
	UPROPERTY(EditAnywhere, Category = "Manager|Bots")
	TArray<TSubclassOf<class ABot>> NormalBots;

	UPROPERTY(EditAnywhere, Category = "Manager|Bots")
	TArray<FSpecialCount> SpecialBotCounts;

	//Hack to keep the lazer from spawning too early 
	static const int32 LazerIndex;
	int32 InitialLazerBotCount;

	/** Shield Bot Classes */
	UPROPERTY(EditAnywhere, Category = "Manager|Bots")
	TArray<TSubclassOf<class ABot>> ShieldBots;

	/** MortarBot Classes */
	UPROPERTY(EditAnywhere, Category = "Manager|Bots")
	TArray<TSubclassOf<class ABot>> MortarBots;

	/** Lazer Bot Classes */
	UPROPERTY(EditAnywhere, Category = "Manager|Bots")
	TArray<TSubclassOf<class ABot>> LazerBots;

	/** Medium Bot Classes */
	UPROPERTY(EditAnywhere, Category = "Manager|Bots")
	TArray<TSubclassOf<class ABot>> MedBots;

	/** Seeker Bot Classes */
	UPROPERTY(EditAnywhere, Category = "Manager|Bots")
	TArray<TSubclassOf<class ABot>> SeekerBots;

	/** return the spawning status */
	UFUNCTION(BlueprintPure, Category = "Manager|Status")
	bool SpawningFinished();

protected:
	/** all the spawners in the level */
	TArray<class ASingleBotSpawner*> Spawners;

	/** Progress till the next bot is spawned IF the max bot limit hasn't been hit or the spawn pool still has room */
	float SpawnProgress;

	/** Find all the spawners in the level */
	void FindSpawners();

	/** Enter the Bot Spawning Process  */
	void SpawnBot();

	/** Search through the Spawners and find the ones that have the least number of pawns near */
	int32 GetBestSpawner();

	/** Get the number of pawns found within the search radius around the Spawn Location */
	int32 CalculateSpawnWeight(FVector SpawnLocation);

	/** Radius around a spawn point that is checked for other Pawns */
	UPROPERTY(EditAnywhere, Category = "Manager")
	float SearchRadius;

	/** Get a random Bot in the Normal Bot class */
	TSubclassOf<class ABot> GetNormalBotClass();

	/** Get a random Bot in the Speical Bot Class */
	TSubclassOf<class ABot> GetSpecialBotClass();

	/** Get the current number of alive bots */
	void GetAliveBots();

	int32 GetAliveSpecialBots(uint8 InBotType);

	FTimerHandle AliveBotCheck_TimerHandle;

	UPROPERTY(EditAnywhere, Category = "Manager")
	float AliveBotCheckTime;

	/** Current Alive Bot Count */
	UPROPERTY(BlueprintReadOnly, Category = "Manager|Bot Count")
	int32 AliveBotCount;

	/** Current Alive Special Bots Count */
	UPROPERTY(BlueprintReadOnly, Category = "Manager|Bot Count")
	int32 AliveSpecialCount;

	bool CanSpawn();

	bool RoomInSpawnPool();

	bool CanSpawnSpecial();

	/** Finished Spawning bots this wave */
	bool bFinished;

protected:
	/** Triggered at the Start of Pre Wave */
	UFUNCTION(BlueprintNativeEvent, Category = "Manager|Wave Status")
	void OnPreWaveStart();
	virtual void OnPreWaveStart_Implementation();

	/** Triggered at the Wave Start */
	UFUNCTION(BlueprintNativeEvent, Category = "Manager|Wave Status")
	void OnWaveStart();
	virtual void OnWaveStart_Implementation();

	/** Triggered on Wave Completed */
	UFUNCTION(BlueprintNativeEvent, Category = "Manager|Wave Status")
	void OnWaveComplete();
	virtual void OnWaveComplete_Implementation();

	/** Triggered at Post Wave Completed */
	UFUNCTION(BlueprintNativeEvent, Category = "Manager|Wave Status")
	void OnPostWaveComplete();
	virtual void OnPostWaveComplete_Implementation();

public:
	UPROPERTY(BlueprintReadOnly, Category = "Manager|Wave Status")
	EWaveState WaveState;

	UFUNCTION(BlueprintCallable, Category = "Manager|Wave Status")
	void SetWaveState(EWaveState InWaveState);

	/** Called when the Wave State has changed */
	UFUNCTION(BlueprintImplementableEvent, Category = "Manager|Wave Status")
	void OnWaveStateChange(EWaveState NewWaveState);

	UPROPERTY(EditAnywhere, Category = "Manager|Wave Status")
	float WaveHeaderDuration;

	UPROPERTY()
	float WaveHeaderProgress;
	
protected:
	UFUNCTION(BlueprintCallable, Category = "Manager|Wave Status")
	void HandleWaveStateChange();

	UFUNCTION(BlueprintPure, Category = "Manager|Wave Status")
	bool IsPreWave() const;

	UFUNCTION(BlueprintPure, Category = "Manager|Wave Status")
	bool IsWaveInprogress() const;

	UFUNCTION(BlueprintPure, Category = "Manager|Wave Status")
	bool IsPostWave() const;

	/** Function to Tick during the PreWave State */
	void TickPreWave(float DeltaTime);

	/** Function to Tick during the WaveInprogress State */
	void TickWaveInprogress(float DeltaTime);

	/** Function to Tick during the PostWave State */
	void TickPostWave(float DeltaTime);

public:
	/** Current Wave the Player is on */
	UPROPERTY(BlueprintReadOnly, Category = "Manager|Wave Properties")
	int32 WaveIndex;

	/** Wave Index is 1 count based. 1 will be automaticlly added to this value at runtime. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manager|Wave Properties")
	int32 MaxWaveIndex;

protected:
	/** Increase the Wave Count and increase the bot limits */
	void IncreaseWave();

	/** Arcade Spawn Scaleing */
	void ArcadeGrowth();

	int32 ScaleBotLimits(const FWaveGrowth& InWaveGrowth);
	int32 ScaleBotLimits(const float& Base, const float& Growth);

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Manager|Widget")
	UUserWidget* WavePopupWidget;

	UPROPERTY(BlueprintReadWrite, Category = "Manager|Widget")
	UUserWidget* WaveCompleteWidget;

	UFUNCTION(BlueprintCallable, Category = "Manager|Widget")
	void RemoveWavePopupWidget();

	UFUNCTION(BlueprintCallable, Category = "Manager|Widget")
	void RemoveWaveCompleteWidget();

public:
	/** End the Tick Process and Cleanup any rogue Widgets */
	void EndMatch();

};
