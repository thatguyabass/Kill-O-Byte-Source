// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "GameMode_Story.h"
#include "Online/SnakeInstance.h"
#include "HUD/Controllers/LevelListDataAsset.h"
#include "HUD/Controllers/ScoreDataAsset.h"
#include "SnakeCharacter/SnakePlayer.h"
#include "UserData/UserData.h"
#include "Viewport/SnakeViewportClient.h"

bool AGameMode_Story::bLevelCompleted = false;
bool AGameMode_Story::bRepeatTutorial = false;

AGameMode_Story::AGameMode_Story(const FObjectInitializer& PCIP)
	: ABaseGameMode(PCIP)
{
	static ConstructorHelpers::FObjectFinder<ULevelListDataAsset> LevelListObject(TEXT("/Game/Blueprints/UI/StoryLevelList"));
	LevelListDataAsset = LevelListObject.Object;

	static ConstructorHelpers::FObjectFinder<UScoreDataAsset> ScoreDataObject(TEXT("/Game/Blueprints/UI/NormalScores"));
	ScoreDataAsset = ScoreDataObject.Object;

	GameModeID = 0;
}

FString AGameMode_Story::InitNewPlayer(APlayerController* NewPlayerController, const TSharedPtr<const FUniqueNetId>& UniqueId, const FString& Options, const FString& Portal)
{
	FString First = ParseOption(Options, TEXT("level"));
	FString Tutorial = ParseOption(Options, TEXT("Repeat"));

	ASnakePlayer* Controller = Cast<ASnakePlayer>(NewPlayerController);
	if (Controller)
	{
		bool T = Tutorial.Len() > 0 ? Tutorial.ToBool() : false;
		Controller->SetTutorialRepeat(First.ToBool(), T);

		bLevelCompleted = First.ToBool();
		bRepeatTutorial = T;
	}

	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

void AGameMode_Story::TriggerPlayerWon()
{
	ASnakePlayer* Controller = Cast<ASnakePlayer>(GetWorld()->GetFirstPlayerController());
	ensure(Controller);

	UUserData* Data = GetSavedData();
	if (Data)
	{
		int32 LevelIndex = LevelListDataAsset->GetLevelIndex();
		int32 DeathCount = Controller->GetDeathCount();
		bool bDiff = Data->GetDifficulty();
		
		int32 Score = ScoreDataAsset->GetScore(LevelIndex, DeathCount);
		if (bDiff)
		{
			Data->SetHardScores(LevelIndex, Score);
		}
		//Always set Normal.
		//If the player is playing on hard it is only fair we give them the score for normal as well
		Data->SetNormalScores(LevelIndex, Score);

		Data->SetLevelCompleted(LevelIndex, true);
		Data->SaveIfDirty();

		TriggerEndgameScorescreen(Score);
	}

	FinishMatch();
}

void AGameMode_Story::TravelToNextLevel()
{
	USnakeInstance* Instance = Cast<USnakeInstance>(GetWorld()->GetGameInstance());
	if (Instance && LevelListDataAsset)
	{
		GetWorld()->ForceGarbageCollection(true);

		Instance->GoToState(SGameInstanceState::Playing);
		Instance->ShowLoadingScreen();

		//USnakeViewportClient* Viewport = Cast<USnakeViewportClient>(GetWorld()->GetGameViewport());
		//if (Viewport)
		//{
		//	Viewport->ShowLoadingScreen();
		//}

		ASnakePlayer* Controller = Cast<ASnakePlayer>(GetWorld()->GetFirstPlayerController());
		Controller->HandleNextLevelTransition();

		int32 LevelIndex = LevelListDataAsset->GetLevelIndex();
		LevelIndex++;

		LevelListDataAsset->SetLevelIndex(LevelIndex);
		FString MapName = LevelListDataAsset->GetTrueName();

		//Ensure the Game isn't online
		Instance->SetIsOnline(false);
		FString Story = "Story";
		FString const URL = FString::Printf(TEXT("/Game/Maps/%s?game=%s"), *MapName, *Story);

		GetWorld()->ServerTravel(URL, true, false);
	}
}

bool AGameMode_Story::CanEnterNextLevel() const
{
	if (LevelListDataAsset)
	{
		int32 Index = LevelListDataAsset->GetLevelIndex();
		int32 LevelCount = LevelListDataAsset->GetLevelCount();

		return Index >= (LevelCount - 1) ? false : true;
	}
	
	return false;
}

int32 AGameMode_Story::GetLevelIndex() const
{
	return LevelListDataAsset->GetLevelIndex();
}

void AGameMode_Story::SnakeKilled(ASnakePlayerState* Killer, ASnakePlayerState* Victim)
{
	Super::SnakeKilled(Killer, Victim);

	ASnakePlayer* Controller = Cast<ASnakePlayer>(GetWorld()->GetFirstPlayerController());
	if (Controller)
	{
		const int32 LevelIndex = LevelListDataAsset->GetLevelIndex();
		const int32 PlayerDeaths = Controller->GetDeathCount();

		int32 Score = ScoreDataAsset->GetScore(LevelIndex, PlayerDeaths);

		Controller->DisplayCurrentScore(Score);
	}
}

bool AGameMode_Story::GetLevelCompleted() const
{
	return bLevelCompleted;
}

bool AGameMode_Story::GetRepeatTutorial() const
{
	return bRepeatTutorial;
}