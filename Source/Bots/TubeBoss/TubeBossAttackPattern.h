// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "TubeBossAttackPattern.generated.h"

/** Core of the move, Store the index of the turret and the location.
*	In the TubeBoss class, move the selected turret to the selected location 
*/
USTRUCT(BlueprintType)
struct FAttackMove
{
	GENERATED_BODY()

	/** Turret Index */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	int32 TurretIndex;

	/** Attack position index */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	int32 PositionIndex;

	FAttackMove()
	{
		TurretIndex = 0;
		PositionIndex = 0;
	}

};

/** Sequence of moves that will be executed in order */
USTRUCT(BlueprintType)
struct FAttackPattern
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	TArray<FAttackMove> Attacks;

	/** Duration for the Turret to move into the target location */
	UPROPERTY(EditAnywhere, Category = "Pattern")
	float MoveDuration;

	/** Delay after the Turret has reached its target location. Fire Delay */
	UPROPERTY(EditAnywhere, Category = "Pattern")
	float MoveDelay;

};

UCLASS()
class SNAKE_PROJECT_API UTubeBossAttackPattern : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UTubeBossAttackPattern(const class FObjectInitializer& PCIP);

	/** Array of Patterns that can be choosen to execute an attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Pattern")
	TArray<FAttackPattern> Patterns;

	UFUNCTION(BlueprintPure, Category = "Attack Pattern")
	FAttackPattern GetAttackPattern(int32 Index) const;

	UFUNCTION(BlueprintPure, Category = "Attack Pattern")
	FAttackPattern GetRandomAttackPattern() const;

	UFUNCTION(BlueprintPure, Category = "Attack Pattern")
	FAttackMove GetAttackMove(int32 PatternIndex, int32 AttackIndex) const;

	FAttackMove GetAttackMoveWithPoint(FIntPoint Point) const;

	void GetPatternAndAttackWithPoint(FIntPoint Point, FAttackPattern& PatternRef, FAttackMove& AttackRef);

	UFUNCTION(BlueprintPure, Category = "Attack Pattern")
	int32 GetPatternLength() const;

	UFUNCTION(BlueprintPure, Category = "Attack Pattern")
	int32 GetAttackLength(int32 PatternIndex) const;

};
