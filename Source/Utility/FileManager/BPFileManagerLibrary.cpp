// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BPFileManagerLibrary.h"

void UBPFileManagerLibrary::CreateTextFile(FString Dir, FString FileName)
{
	if (!DirectoryExists(Dir))
	{
		return;
	}

	Dir += "\\";
	Dir += FileName;

	FFileHelper::SaveStringToFile("", *Dir);
}

void UBPFileManagerLibrary::SaveStringToFile(FString SaveDirectory, FString FileName, FString Text, bool bAllowOverwrite)
{
	FString Dir = FPaths::GameDir();
	Dir += SaveDirectory + "/" + FileName + ".txt";

	if (!DirectoryExists(Dir))
	{
		return;
	}

	IFileHandle* Handle = FPlatformFileManager::Get().GetPlatformFile().OpenWrite(*Dir, true);
	if (Handle)
	{
		if (!bAllowOverwrite)
		{
			Handle->SeekFromEnd();
		}

		WriteToFile(Text, Handle);
		delete Handle;
	}
}

bool UBPFileManagerLibrary::DirectoryExists(FString Dir)
{
	if (!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*Dir))
	{
		FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*Dir);

		if (!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*Dir))
		{
			return false;
		}
	}

	return true;
}

FString UBPFileManagerLibrary::CombatTestingCreateFile()
{
	FString Dir = FPaths::GameDir();
	Dir += "CombatData/";

	if (!DirectoryExists(Dir))
	{
		return FString("None");
	}

	FDateTime Now = FDateTime::Now();

	int32 D = Now.GetDayOfYear();
	int32 H = Now.GetHour();
	int32 M = Now.GetMinute();
	int32 S = Now.GetSecond();

	FString TimeStamp = FString::FromInt(D) + "_" + FString::FromInt(H) + "_" + FString::FromInt(M) + "_" + FString::FromInt(S);

	Dir += "\\";
	Dir += "Session_" + TimeStamp;

	FFileHelper::SaveStringToFile("", *Dir);

	return Dir;
}

void UBPFileManagerLibrary::CombatTestingSaveToFile(FString FilePath, FCombatTestData CombatData)
{
	FString Dir = FilePath.LeftChop(FilePath.Len() - FilePath.Find("/\\"));
	if (!DirectoryExists(Dir))
	{
		return;
	}

	const FString LineEnd = "\n";
	const FString Filler = "****************************************************************" + LineEnd;
	TArray<FString> TextToWrite;

	TextToWrite.Add(Filler);
	TextToWrite.Add("Wave Number: " + FString::FromInt(CombatData.WaveNumber) + LineEnd);
	TextToWrite.Add("Number of Bots Spawned: " + FString::FromInt(CombatData.SpawnedBotCount) + LineEnd);
	TextToWrite.Add("Wave Duration: " + FormatTimeMinSec(CombatData.WaveDuration) + LineEnd);
	TextToWrite.Add("Player Damage Taken: " + FString::FromInt(CombatData.PlayerDamageTaken) + LineEnd);
	TextToWrite.Add("Player Death Count: " + FString::FromInt(CombatData.PlayerDeathCount) + LineEnd);
	TextToWrite.Add("Player Satisfaction: " + FString::FromInt(CombatData.PlayerSatisfaction) + LineEnd);
	TextToWrite.Add("Difficulty: " + FString::FromInt(CombatData.Difficulty) + LineEnd);

	TextToWrite.Add(LineEnd + Filler);
	TextToWrite.Add("Names of Spawned Bots" + LineEnd);
	for (int32 c = 0; c < CombatData.BotNameList.Num(); c++)
	{
		const FString BotName = CombatData.BotNameList[c];
		TextToWrite.Add(BotName + LineEnd);
	}

	TextToWrite.Add(Filler);
	TextToWrite.Add(Filler);
	
	for (int32 c = 0; c < 3; c++)
	{
		TextToWrite.Add(LineEnd);
	}

	IFileHandle* Handle = FPlatformFileManager::Get().GetPlatformFile().OpenWrite(*FilePath, true);
	if (Handle)
	{
		Handle->SeekFromEnd();
		for (int32 c = 0; c < TextToWrite.Num(); c++)
		{
			WriteToFile(TextToWrite[c], Handle);
		}
		delete Handle;
	}
}

void UBPFileManagerLibrary::WriteToFile(FString TextToWrite, IFileHandle* Handle)
{
	if (Handle)
	{
		Handle->Write((const uint8*)TCHAR_TO_ANSI(*TextToWrite), TextToWrite.Len());
	}
}

FString UBPFileManagerLibrary::FormatTimeMinSec(float RawTime)
{
	const int32 TotalSeconds = FMath::Max(0, FMath::TruncToInt(RawTime) % 3600);
	const int32 Minutes = TotalSeconds / 60;
	const int32 Seconds = TotalSeconds % 60;
	const FString Time = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

	return Time;
}