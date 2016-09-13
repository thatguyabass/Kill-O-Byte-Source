// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "MasterController.h"
#include "GameMode/PlayerState/SnakePlayerState.h"
#include "Online.h"
#include "Online/SnakeInstance.h"
#include "Viewport/SnakeViewportClient.h"
//#include "Runtime/Core/Public/Windows/WindowsCursor.h"

int32 AMasterController::OnScreenEffectContext = 1;
bool AMasterController::bShowCredits = false;

AMasterController::AMasterController(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	bWindowInFocus = true;
	bWindowInFocusLastTick = true;
	bPlayerInMenu = false;

	bMoviePlaying = false;
}

void AMasterController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);
}

void AMasterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleGameFocus();
	FSlateApplication::Get().SetAllUserFocusToGameViewport();
}

void AMasterController::ClientSetSpectatorCamera_Implementation(FVector Location, FRotator Rotation)
{
	SetInitialLocationAndRotation(Location, Rotation);
	SetViewTarget(this);
}

void AMasterController::ClientStartGame_Implementation()
{
	//Ensure the player can move
	SetIgnoreMoveInput(false);

	//Remove the viewport loading screen if one exists. This was placed here for post server travel during a level restart
	USnakeViewportClient* Viewport = Cast<USnakeViewportClient>(GetWorld()->GetGameViewport());
	if (Viewport)
	{
		Viewport->HideLoadingScreen();
	}

	//ClientStartOnlineGame();
}

void AMasterController::ClientGameEnded_Implementation(AActor* EndGameFocus, bool bIsWinner)
{
	Super::ClientGameEnded_Implementation(EndGameFocus, bIsWinner);

	SetIgnoreMoveInput(true);

	//ClientEndOnlineGame();
}

void AMasterController::ClientStartOnlineGame_Implementation()
{
	if (!IsPrimaryPlayer())
	{
		return;
	}

	ASnakePlayerState* MyPlayerState = Cast<ASnakePlayerState>(PlayerState);
	if (MyPlayerState)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid())
			{
				Sessions->StartSession(MyPlayerState->SessionName);
			}
		}
	}
	else
	{
		//Keep attempting to start the online session
		GetWorld()->GetTimerManager().SetTimer(StartOnline_TimerHandle, this, &AMasterController::ClientStartOnlineGame, 0.2f, false);
	}
}

void AMasterController::ClientEndOnlineGame_Implementation()
{
	if (!IsPrimaryPlayer())
	{
		return;
	}

	ASnakePlayerState* MyPlayerState = Cast<ASnakePlayerState>(PlayerState);
	if (MyPlayerState)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid())
			{
				Sessions->EndSession(MyPlayerState->SessionName);
			}
		}
	}
}

void AMasterController::HandleReturnToMainMenu()
{
	USnakeInstance* Instance = Cast<USnakeInstance>(GetGameInstance());
	if (ensure(Instance))
	{
		Instance->ShowLoadingScreen();
	}

	USnakeViewportClient* Viewport = Cast<USnakeViewportClient>(GetWorld()->GetGameViewport());
	if (Viewport)
	{
		Viewport->ShowLoadingScreen();
	}

	CleanupSessionOnReturnToMenu();
}

void AMasterController::CleanupSessionOnReturnToMenu()
{
	USnakeInstance* Instance = GetWorld() ? Cast<USnakeInstance>(GetWorld()->GetGameInstance()) : nullptr;
	if (Instance)
	{
		Instance->CleanupSessionOnReturnToMenu();
	}
}

void AMasterController::ClientReturnToMainMenu_Implementation(const FString& ReturnReason)
{
	USnakeInstance* Instance = GetWorld() ? Cast<USnakeInstance>(GetWorld()->GetGameInstance()) : nullptr;
	if (Instance)
	{
		//Change this to display a message before returning to the root main menu
		Instance->GoToState(SGameInstanceState::MainMenu);
	}
}

UUserData* AMasterController::GetSavedData() const
{
	USnakeLocalPlayer* const LocalPlayer = Cast<USnakeLocalPlayer>(Player);
	if (LocalPlayer)
	{
		return LocalPlayer->GetUserData();
	}

	return nullptr;
}

int32 AMasterController::GetNormalScore(int32 LevelIndex, bool bArcade)
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		return bArcade ? Data->GetArcadeNormalScores(LevelIndex) : Data->GetNormalScores(LevelIndex);
	}

	return -1;
}

void AMasterController::SetNormalScore(int32 LevelIndex, int32 NormalScore, bool bArcade)
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		bArcade ? Data->SetArcadeNormalScores(LevelIndex, NormalScore) : Data->SetNormalScores(LevelIndex, NormalScore);
		Data->SaveIfDirty();
	}
}

int32 AMasterController::GetHardScore(int32 LevelIndex, bool bArcade)
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		return bArcade ? Data->GetArcadeHardScores(LevelIndex) : Data->GetHardScores(LevelIndex);
	}

	return -1;
}

void AMasterController::SetHardScore(int32 LevelIndex, int32 HardScore, bool bArcade)
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		bArcade ? Data->SetArcadeHardScores(LevelIndex, HardScore) : Data->SetHardScores(LevelIndex, HardScore);
		Data->SaveIfDirty();
	}
}

void AMasterController::SetNormalWave(int32 LevelIndex, int32 InWaveIndex)
{
	UUserData* Data = GetSavedData();
	
	if (Data)
	{
		Data->SetWaveNormal(LevelIndex, InWaveIndex);
		Data->SaveIfDirty();
	}
}

int32 AMasterController::GetNormalWave(int32 LevelIndex)
{
	UUserData* Data = GetSavedData();

	if (Data)
	{
		return Data->GetWaveNormal(LevelIndex);
	}

	return -1;
}

void AMasterController::SetHardWave(int32 LevelIndex, int32 InWaveIndex)
{
	UUserData* Data = GetSavedData();

	if (Data)
	{
		Data->SetWaveHard(LevelIndex, InWaveIndex);
		Data->SaveIfDirty();
	}
}

int32 AMasterController::GetHardWave(int32 LevelIndex)
{
	UUserData* Data = GetSavedData();

	if (Data)
	{
		return Data->GetWaveHard(LevelIndex);
	}

	return -1;
}



void AMasterController::ResetNormalScores()
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		Data->ResetNormalScores();
	}
}

void AMasterController::ResetHardScores()
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		Data->ResetHardScores();
	}
}

void AMasterController::ResetArcadeNormalScores()
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		Data->ResetArcadeNormalScores();
	}
}

void AMasterController::ResetArcadeHardScores()
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		Data->ResetArcadeHardScores();
	}
}

void AMasterController::ResetNormalWaveScores()
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		Data->ResetArcadeWaveNormal();
	}
}

void AMasterController::ResetHardWaveScores()
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		Data->ResetArcadeWaveHard();
	}
}

void AMasterController::ResetAllScores()
{
	ResetNormalScores();
	ResetHardScores();

	ResetArcadeNormalScores();
	ResetArcadeHardScores();

	ResetNormalWaveScores();
	ResetHardWaveScores();
}

bool AMasterController::GetDifficulty()
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		return Data->GetDifficulty();
	}

	return false;
}

void AMasterController::SetDifficulty(bool InValue)
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		Data->SetDifficulty(InValue);
		Data->SaveIfDirty();
	}
}

FScoreContentArray AMasterController::GetScoreContentArray(int32 LevelIndex)
{
	ensure(ScoreDataAsset);

	return ScoreDataAsset->GetScoreContentArray(LevelIndex);
}

FScoreContent AMasterController::GetScoreContent(int32 LevelIndex, int32 ContentIndex)
{
	ensure(ScoreDataAsset);

	return ScoreDataAsset->GetScoreContent(LevelIndex, ContentIndex);
}

FString AMasterController::GetScoreString(int32 LevelIndex, int32 ContentIndex)
{
	ensure(ScoreDataAsset);

	return ScoreDataAsset->BuildString(LevelIndex, ContentIndex);
}

TArray<FString> AMasterController::GetScoreStrings(int32 LevelIndex)
{
	ensure(ScoreDataAsset);

	return ScoreDataAsset->BuildStrings(LevelIndex);
}

int32 AMasterController::GetScore(int32 LevelIndex, int32 PlayerDeaths)
{
	ensure(ScoreDataAsset);

	return ScoreDataAsset->GetScore(LevelIndex, PlayerDeaths);
}

int32 AMasterController::GetArcadeWaveLowest()
{
	ensure(ArcadeScoreDataAsset);

	return ArcadeScoreDataAsset->GetLowestWave();
}

int32 AMasterController::GetArcadeWaveMedium()
{
	ensure(ArcadeScoreDataAsset);

	return ArcadeScoreDataAsset->GetMediumWave();
}

int32 AMasterController::GetArcadeWaveHighest()
{
	ensure(ArcadeScoreDataAsset);

	return ArcadeScoreDataAsset->GetHighestWave();
}

bool AMasterController::GetLevelCompleted(int32 LevelIndex)
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		return Data->GetLevelCompleted(LevelIndex);
	}

	return false;
}

void AMasterController::SetLevelCompleted(int32 LevelIndex, bool InValue)
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		Data->SetLevelCompleted(LevelIndex, InValue);
		Data->SaveIfDirty();
	}
}

void AMasterController::SetRepeatTutorial(bool InValue)
{
	bRepeatTutorial = InValue;
}

bool AMasterController::GetRepeatTutorial() const
{
	return bRepeatTutorial;
}

void AMasterController::UnlockAllLevels()
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		const int32 Length = Data->GetLevelsCompletedLength();
		for (int32 c = 0; c < Length; c++)
		{
			Data->SetLevelCompleted(c, true);
		}
		Data->SaveIfDirty();
	}
}

void AMasterController::ResetAllLevels()
{
	UUserData* Data = GetSavedData();
	if (Data)
	{
		const int32 Length = Data->GetLevelsCompletedLength();
		for (int32 c = 0; c < Length; c++)
		{
			Data->SetLevelCompleted(c, false);
		}
		Data->SaveIfDirty();
	}
}

void AMasterController::HandleGameFocus()
{
	ULocalPlayer* LocPlayer = Cast<ULocalPlayer>(Player);
	if (!LocPlayer)
	{
		return;
	}

	if (!LocPlayer->ViewportClient->Viewport || !LocPlayer->ViewportClient->Viewport->IsForegroundWindow())
	{
		bWindowInFocus = false;
	}
	else
	{
		bWindowInFocus = true;
	}

	if (bWindowInFocus != bWindowInFocusLastTick)
	{
		if (bWindowInFocus)
		{
			LocPlayer->ViewportClient->Viewport->SetUserFocus(true);
			FSlateApplication::Get().SetAllUserFocusToGameViewport(EFocusCause::WindowActivate);
		}

		if (!bPlayerInMenu)
		{
			UGameplayStatics::SetGamePaused(this, !bWindowInFocus);
		}
	}

	bWindowInFocusLastTick = bWindowInFocus;
}

int32 AMasterController::AddWidgetToOnScreenEffects(UUserWidget* InWidget)
{
	if (!InWidget)
	{
		return -1;
	}

	FOnScreenEffectContainer Cont(InWidget, OnScreenEffectContext);
	OnScreenEffectWidgets.Add(Cont);
	OnScreenEffectContext++;

	return Cont.ContextIndex;
}

FOnScreenEffectContainer AMasterController::FindOnScreenContainerWithContext(int32 ContextIndex)
{
	FOnScreenEffectContainer Cont;

	for (int32 c = 0; c < OnScreenEffectWidgets.Num(); c++)
	{
		if (OnScreenEffectWidgets[c].Compare(ContextIndex))
		{
			Cont = OnScreenEffectWidgets[c];
		}
	}

	return Cont;
}

bool AMasterController::IsWidgetAtContextValid(int32 ContextIndex)
{
	return FindOnScreenContainerWithContext(ContextIndex).IsValid();
}

UUserWidget* AMasterController::GetWidgetAtContext(int32 ContextIndex)
{
	FOnScreenEffectContainer Cont = FindOnScreenContainerWithContext(ContextIndex);

	if (Cont.IsValid())
	{
		return Cont.Widget;
	}

	return nullptr;
}

void AMasterController::RemoveAllOnScreenEffectWidgets()
{
	for (int32 c = 0; c < OnScreenEffectWidgets.Num(); c++)
	{
		FOnScreenEffectContainer Cont = OnScreenEffectWidgets[c];
		Cont.Widget->RemoveFromParent();
		Cont.Widget = nullptr;
	}

	OnScreenEffectWidgets.Reset();
}

void AMasterController::RemoveOnScreenEffectWidget(int32 ContextIndex)
{
	FOnScreenEffectContainer Cont = FindOnScreenContainerWithContext(ContextIndex);
	if (Cont.IsValid())
	{
		Cont.Widget->RemoveFromParent();
		Cont.Widget = nullptr;

		int32 Index = OnScreenEffectWidgets.Find(Cont);
		OnScreenEffectWidgets.RemoveAt(Index);
	}
}

bool AMasterController::GetInputSettings() const
{
	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		return SavedData->GetInputSetting();
	}

	return true;
}

void AMasterController::SetShowCredits(bool In)
{
	bShowCredits = In;
}

bool AMasterController::GetShowCredits()
{
	return bShowCredits;
}