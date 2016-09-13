// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "ScoreDataAsset.h"

UScoreDataAsset::UScoreDataAsset(const FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

FScoreContentArray UScoreDataAsset::GetScoreContentArray(int32 LevelIndex)
{
	return ScoreContent.IsValidIndex(LevelIndex) ? ScoreContent[LevelIndex] : FScoreContentArray();
}

FScoreContent UScoreDataAsset::GetScoreContent(int32 LevelIndex, int32 ContentIndex)
{
	FScoreContentArray Array = GetScoreContentArray(LevelIndex);

	return Array.ScoreContent[ContentIndex];
}

int32 UScoreDataAsset::GetScore(int32 LevelIndex, int32 PlayerDeaths)
{
	int32 Score = -1;

	FScoreContentArray Array = GetScoreContentArray(LevelIndex);

	for (int32 c = 0; c < Array.ScoreContent.Num(); c++)
	{
		FScoreContent Content = Array.ScoreContent[c];
		switch (Content.Condition)
		{
			case EScoreCondition::Greater:
			{
				if (PlayerDeaths > Content.MaxValue)
				{
					Score = c;
				}
				break;
			}
			case EScoreCondition::Compare:
			{
				if (PlayerDeaths <= Content.MaxValue && PlayerDeaths >= Content.MinValue)
				{
					Score = c;
				}
				break;
			}
			case EScoreCondition::Lesser:
			{
				if (PlayerDeaths < Content.MinValue)
				{
					Score = c;
				}
				break;
			}
		}
	}

	return Score;
}

FString UScoreDataAsset::BuildString(int32 LevelIndex, int32 ContentIndex)
{
	FScoreContent Content = GetScoreContent(LevelIndex, ContentIndex);

	FString OutString = "";

	switch (Content.Condition)
	{
		case EScoreCondition::Greater:
		{
			OutString = " >= " + FString::FromInt(Content.MaxValue + 1);// +" Deaths";
			break;
		}
		case EScoreCondition::Compare:
		{
			OutString = FString::FromInt(Content.MinValue) + " - " + FString::FromInt(Content.MaxValue);// +" Deaths";
			break;
		}
		case EScoreCondition::Lesser:
		{
			OutString = " <= " + FString::FromInt(Content.MinValue - 1);// +" Deaths";
			break;
		}
	}

	return OutString;
}

TArray<FString> UScoreDataAsset::BuildStrings(int32 LevelIndex)
{
	TArray<FString> OutStrings;

	for (int32 c = 0; c < 3; c++)
	{
		FString Temp = BuildString(LevelIndex, c);
		OutStrings.Add(Temp);
	}

	return OutStrings;
}