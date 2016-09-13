// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Utility.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ECutType : uint8
{
	Cut,	//Remove the Dead Link and Freeze the Links behind the dead link
	Magnet	//Remove the Dead Link, but don't lose the following links
};

UENUM()
enum class EFireMode : uint8
{
	Simultaneous,
	Sequential,
	Random
};

USTRUCT(BlueprintType)
struct FSpeedPower
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power|Speed")
	float Duration;

	/* This is a Zero based Modifier. Any changes MUST be made around this */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power|Speed")
	float SpeedModifier;

	UPROPERTY(BlueprintReadOnly, Category = "Power|Speed")
	float Progress;

	UPROPERTY(BlueprintReadOnly, Category = "Power|Speed")
	bool bIsActive;

	void StopAndReset()
	{
		Progress = 0.0f;
		bIsActive = false;
	}

	FSpeedPower()
	{
		Duration = 5.0f;
		Progress = 0.0f;
		SpeedModifier = 0.25f;
		bIsActive = false;
	}
};

USTRUCT(BlueprintType)
struct FShieldPower
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power|Shield")
	float Duration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power|Shield")
	int32 Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power|Shield")
	TSubclassOf<class ARepulsion> RepulsionFieldClass;

	UPROPERTY(BlueprintReadOnly, Category = "Power|Shield")
	TArray<class ARepulsion*> RepulsionFields;

	UPROPERTY(BlueprintReadOnly, Category = "Power|Shield")
	float Progress;

	//Is the Shield currently Active for the Snake
	UPROPERTY(BlueprintReadOnly, Category = "Power|Shield")
	bool bIsActive;

	UPROPERTY(BlueprintReadOnly, Category = "Power|Shield")
	int32 PowerLevel;

	/** A Field can only be spawned when above level 1 power level. */
	bool CanSpawnField() { return PowerLevel > 0 && RepulsionFieldClass; }

	/** Trigger the Repulsion Fields Internal Timers */
	void TriggerTimers();

	/** Start the Destruction sequence for all the fields in the Array */
	void ClearFields();

	/** Find a field based on the Field Owner */
	class ARepulsion* GetField(AActor* RepulsionOwner);

	FShieldPower()
	{
		Duration = 5.0f;
		Progress = 0.0f;
		Health = 1;
		bIsActive = false;
		PowerLevel = -1;
	}
};

UENUM(BlueprintType)
enum class EPowerType : uint8
{
	None,
	Bomb,
	Build,
	Speed,
	Shield,
	Blast,
	BlastSecondary
};

namespace ESnakeMatchState
{
	enum Type
	{
		Warmup,
		Playing,
		Won,
		Lost
	};
}

namespace SpecialPlayerIndex
{
	const int32 All = -1;
}

USTRUCT(BlueprintType)
struct FPlayerData
{
	GENERATED_BODY()

	/** the Team the player belongs too */
	int32 TeamNumber;

	/** The Number within the team */
	int32 PlayerId;

	FPlayerData()
		: TeamNumber(0), PlayerId(SpecialPlayerIndex::All)
	{ }

	FPlayerData(int32 InTeamNumber, int32 InPlayerId)
		: TeamNumber(InTeamNumber), PlayerId(InPlayerId)
	{ }

	/** Compare */
	bool operator==(const FPlayerData& Other) const
	{
		return (TeamNumber == Other.TeamNumber && PlayerId == Other.PlayerId);
	}

	/** Ensure this is a valid Player Data*/
	bool IsValid() const
	{
		return !(*this == FPlayerData());
	}
};

USTRUCT(BlueprintType)
struct FMapItemData
{
	GENERATED_BODY()

	/** Map Name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	FString SafeName;

	/** Map Name the level uses to transition between levels */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	FString TrueName;

	/** Map Image */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	UTexture2D* MapImage;

	/** Level Index */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	int32 LevelIndex;

};

USTRUCT(BlueprintType)
struct FGameModeData
{
	GENERATED_BODY()

	/** Game Mode Name */
	UPROPERTY(EditAnywhere, Category = "Mode")
	FString GameModeName;

	/** Game Mode Short hand used during level transfer to choose the coorect gamemode */
	UPROPERTY(EditAnywhere, Category = "Mode")
	FString GameModeShortHand;

};

USTRUCT(BlueprintType)
struct FTeamColorData
{
	GENERATED_BODY()

	/** Primary Color */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Color")
	FLinearColor PrimaryColor;

	/** Secondary Color */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Color")
	FLinearColor SecondaryColor;

	FTeamColorData()
	{
		PrimaryColor = FColor::Blue;
		SecondaryColor = FColor::Red;
	}
};

UENUM(BlueprintType)
enum class EAttackMode : uint8
{
	Attack01_Red = 0,
	Attack02_Green = 1,
	Attack03_Purple = 2,
	Attack04_Blue = 3,
	Attack05_White = 4
};

USTRUCT(BlueprintType)
struct FAttackType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack Type")
	EAttackMode AttackMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack Type")
	FTeamColorData ColorData;

	//Check if the Attack Modes are the same. If either Mode is Attack05_White, return true 
	bool CompareAttackMode(EAttackMode InAttackMode)
	{
		if (AttackMode == InAttackMode)
		{
			return true;
		}
		
		return false;
	}

	FAttackType()
	{
		AttackMode = EAttackMode::Attack05_White;
	}

	FAttackType(EAttackMode Mode)
	{
		AttackMode = Mode;
	}
};

USTRUCT(BlueprintType)
struct FRankMapStruct
{
	GENERATED_BODY()

	FRankMapStruct()
	{

	}

	void SetData(const int32 NewIndex, class ASnakePlayerState* InState)
	{
		Index = NewIndex;
		PlayerState = InState;
	}

	UPROPERTY(BlueprintReadOnly, Category = "Rank Struct")
	int32 Index;

	UPROPERTY(BlueprintReadOnly, Category = "Rank Struct")
	class ASnakePlayerState* PlayerState;

};

USTRUCT(BlueprintType)
struct FServerEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Server Entry")
	FString ServerName;

	UPROPERTY(BlueprintReadOnly, Category = "Server Entry")
	FString CurrentPlayers;

	UPROPERTY(BlueprintReadOnly, Category = "Server Entry")
	FString MaxPlayers;

	UPROPERTY(BlueprintReadOnly, Category = "Server Entry")
	FString Ping;

	UPROPERTY(BlueprintReadOnly, Category = "Server Entry")
	int32 SearchResultsIndex;

	FString GameType;
	FString MapName;
};

USTRUCT(BlueprintType)
struct F2DInt32
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2D Int32")
	int32 X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2D int32")
	int32 Y;

	int32 GetX() const
	{
		return X;
	}

	int32 GetY() const
	{
		return Y;
	}

	F2DInt32(int32 _X, int32 _Y)
	{
		X = _X;
		Y = _Y;
	}

	F2DInt32()
	{
		X = 0;
		Y = 0;
	}
};

UCLASS()
class SNAKE_PROJECT_API UUtility : public UObject
{
	GENERATED_BODY()
};