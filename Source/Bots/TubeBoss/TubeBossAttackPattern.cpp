// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "TubeBossAttackPattern.h"

UTubeBossAttackPattern::UTubeBossAttackPattern(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

FAttackPattern UTubeBossAttackPattern::GetAttackPattern(int32 Index) const
{
	return Patterns.IsValidIndex(Index) ? Patterns[Index] : FAttackPattern();
}

FAttackPattern UTubeBossAttackPattern::GetRandomAttackPattern() const
{
	return GetAttackPattern(FMath::RandRange(0, Patterns.Num() - 1));
}

FAttackMove UTubeBossAttackPattern::GetAttackMove(int32 PatternIndex, int32 AttackIndex) const
{
	return GetAttackPattern(PatternIndex).Attacks[AttackIndex];
}

FAttackMove UTubeBossAttackPattern::GetAttackMoveWithPoint(FIntPoint Point) const
{
	return GetAttackMove(Point.X, Point.Y);
}

int32 UTubeBossAttackPattern::GetPatternLength() const
{
	return Patterns.Num();
}

int32 UTubeBossAttackPattern::GetAttackLength(int32 PatternIndex) const
{
	return GetAttackPattern(PatternIndex).Attacks.Num();
}

void UTubeBossAttackPattern::GetPatternAndAttackWithPoint(FIntPoint Point, FAttackPattern& PatternRef, FAttackMove& AttackRef)
{
	PatternRef = GetAttackPattern(Point.X);
	AttackRef = GetAttackMoveWithPoint(Point);
}