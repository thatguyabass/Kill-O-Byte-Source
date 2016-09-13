// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "ArcadeScore.h"

UArcadeScore::UArcadeScore(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	LowestWave = 0;
	MedWave = 1;
	HighestWave = 2;
}

int32 UArcadeScore::GetLowestWave()
{
	return LowestWave;
}

int32 UArcadeScore::GetMediumWave()
{
	return MedWave;
}

int32 UArcadeScore::GetHighestWave()
{
	return HighestWave;
}

int32 UArcadeScore::GetScore(int32 WaveCount)
{
	int32 Score = -1;

	if (WaveCount >= LowestWave && WaveCount < MedWave)
	{
		Score = 0;
	}
	else if (WaveCount >= MedWave && WaveCount < HighestWave)
	{
		Score = 1;
	}
	else if(WaveCount >= HighestWave)
	{
		Score = 2;
	}

	return Score;
}