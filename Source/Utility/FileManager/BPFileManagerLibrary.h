// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "BPFileManagerLibrary.generated.h"

USTRUCT(BlueprintType)
struct FCombatTestData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	int32 WaveNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	int32 SpawnedBotCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	float WaveDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	int32 PlayerDamageTaken;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	int32 PlayerDeathCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	int32 PlayerSatisfaction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	int32 Difficulty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	TArray<FString> BotNameList;

	FCombatTestData()
	{
		WaveNumber = 0;
		SpawnedBotCount = 0;
		WaveDuration = 0.0f;
		PlayerDamageTaken = 0;
		PlayerDeathCount = 0;
		PlayerSatisfaction = 5;
		Difficulty = 5;
	}
};

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API UBPFileManagerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "File Manager")
	static void CreateTextFile(FString Dir, FString FileName);
	
	UFUNCTION(BlueprintCallable, Category = "File Manager")
	static void SaveStringToFile(FString SaveDirectory, FString FileName, FString Text, bool bAllowOverwrite = false);
	
	/** Check if a directory exists or create it if it doesn't */
	UFUNCTION(BlueprintCallable, Category = "File Manager")
	static bool DirectoryExists(FString Dir);

	/** Create the text file for the testing session. Return the Directory + File Path */
	UFUNCTION(BlueprintCallable, Category = "File Manager|Combat Testing Helper")
	static FString CombatTestingCreateFile();

	UFUNCTION(BlueprintCallable, Category = "File Manager|Combat Testing Helper")
	static void CombatTestingSaveToFile(FString FilePath, FCombatTestData CombatData);

private:
	static void WriteToFile(FString TextToWrite, IFileHandle* Handle);

	static FString FormatTimeMinSec(float RawTime);

};
