// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "MenuController.h"
#include "Online/SnakeInstance.h"
#include "Online/OnlineHelper.h"
#include "HUD/Controllers/LevelListDataAsset.h"
#include "Audio/BackgroundMusic.h"
#include "Viewport/SnakeViewportClient.h"
#include "Powers/AttackHelper.h"
#include "Movie/DemoMoviePlayer.h"

bool AMenuController::bPlayDemo = false;

AMenuController::AMenuController(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	LobbyName = "Lobby";
	LobbyGameType = "Lobby";

	bIsLanMatch = false;

	bRepeatTutorial = false;
}

void AMenuController::InitializeMenu()
{
	FindMenuInfo();
	if (MenuWidget == nullptr)
	{
		AddMenu();

		GetWorldTimerManager().SetTimer(Add_TimerHandle, this, &AMenuController::InitializeMenu, 0.1f, false);
		GEngine->PreExit();
	}
}

void AMenuController::DestroyMenu()
{
	if (MenuWidget && MenuWidget->IsInViewport())
	{
		RemoveMenu();

		GetWorldTimerManager().SetTimer(Remove_TimerHandle, this, &AMenuController::DestroyMenu, 0.1f, false);
	}
}

void AMenuController::StartGame(int32 LevelIndex, bool bArcade)
{
	//A level has already been selected, no not launch another.
	if (bAudioFadeInprogress)
	{
		return;
	}

	SetCinematicMode(true, true, true);

	//If music is valid fade, fade out the audio before the level transition.
	ABackgroundMusic* Music = GetBackgroundMusicActor();
	if (Music)
	{
		//Store the level and arcade flags internally 
		InternalLevelIndex = LevelIndex;
		bInternalArcadeFlag = bArcade;

		//Variables are Ticked in Tick, once the duration has been met the level will be loaded 
		bAudioFadeInprogress = true;
		AudioFadeProgress = 0.0f;
		AudioFadeDuration = Music->FadeDuration;
		Music->FadeAudioOut();
	}
	else
	{
		//No Music found, load the level 
		StartGame_Internal(LevelIndex, bArcade);
	}
}

void AMenuController::StartGame_Internal(int32 LevelIndex, bool bArcade)
{
	USnakeInstance* GameInstance = Cast<USnakeInstance>(GetGameInstance());
	USnakeViewportClient* View = Cast<USnakeViewportClient>(GetWorld()->GetGameViewport());
	ULevelListDataAsset* LevelListDataAsset = bArcade ? ArcadeLevelListDataAsset : StoryLevelListDataAsset;

	if (GameInstance && Player != nullptr && LevelListDataAsset && View)
	{
		GameInstance->GoToState(SGameInstanceState::Playing);

		//Show both incase one doesn't fire correctly. If both fire, they will end up overlapping which will hide the other. 
		//View Loading screen is removed in the Master Controller using the ClientStartGame_Implementation Method 
		View->ShowLoadingScreen();
		GameInstance->ShowLoadingScreen();

		LevelListDataAsset->SetLevelIndex(LevelIndex);
		FString MapName = LevelListDataAsset->GetTrueName();

		//Set the Difficulty
		AttackHelper::SetDifficultyScale(GetDifficulty());

		//Ensure the game isn't online 
		GameInstance->SetIsOnline(false);
		FString GameMode = bArcade ? "Arcade" : "Story";

		bool LevelCompleted = GetLevelCompleted(LevelIndex);
		FString FirstTime = LevelCompleted ? "true" : "false";
		FString Tutorial = LevelCompleted ? (bRepeatTutorial ? "true" : "false") : "";
		
		FString const URL = FString::Printf(TEXT("/Game/Maps/%s?game=%s?level=%s?Repeat=%s"), *MapName, *GameMode, *FirstTime, *Tutorial);

		GetWorld()->ServerTravel(URL, false, false);
	}
}

void AMenuController::HostGame()
{
	USnakeInstance* GameInstance = Cast<USnakeInstance>(GetGameInstance());
	if (ensure(GameInstance) && Player != nullptr)
	{
		//Ensure the Instance is set to Online or other will not be able to connect dispite being able to search and find the game 
		GameInstance->SetIsOnline(true);
		FString const StartURL = FString::Printf(TEXT("/Game/Maps/%s?game=%s%s%s?%s=%d"), *LobbyName, *LobbyGameType, GameInstance->GetIsOnline() ? TEXT("?listen") : TEXT(""), bIsLanMatch ? TEXT("?bIsLanMatch") : TEXT(""), TEXT(""), 8);

		GameInstance->HostGame(Cast<ULocalPlayer>(Player), LobbyGameType, StartURL);
	}
}

void AMenuController::SetLAN(FString InString)
{
	if (InString == "Yes")
	{
		bIsLanMatch = true;
	}
	else if (InString == "No")
	{
		bIsLanMatch = false;
	}
}

void AMenuController::CreateOnlineHelper()
{
	OnlineHelper = MakeShareable(new FOnlineHelper());
	OnlineHelper->Construct(Cast<USnakeInstance>(GetGameInstance()), Cast<ULocalPlayer>(Player));
}

void AMenuController::DestroyOnlineHelper()
{
	OnlineHelper = nullptr;
}

void AMenuController::OnJoinServer()
{
	//Ensure the Online Helper is Valid
	if (!OnlineHelper.IsValid())
	{
		CreateOnlineHelper();
	}
	
	OnlineHelper->OnJoinServer();
}

void AMenuController::FindMenuInfo()
{
	if (Info == nullptr)
	{
		for (TActorIterator<AMenuInfo> Iter(GetWorld()); Iter; ++Iter)
		{
			if (*Iter)
			{
				Info = *Iter;
				break;
			}
		}
	}
}

bool AMenuController::InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamePad)
{
	//If the audio isn't fadding process key input
	if (!bAudioFadeInprogress)
	{
		return Super::InputKey(Key, EventType, AmountDepressed, bGamePad);
	}

	return false;
}

void AMenuController::BeginPlay()
{
	Super::BeginPlay();

	USnakeViewportClient* Viewport = Cast<USnakeViewportClient>(GetWorld()->GetGameViewport());
	if (Viewport)
	{
		Viewport->HideLoadingScreen();
	}
}

void AMenuController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bAudioFadeInprogress)
	{
		AudioFadeProgress += DeltaTime;
		if (AudioFadeProgress > AudioFadeDuration)
		{
			bAudioFadeInprogress = false;
			AudioFadeProgress = 0.0f;
			StartGame_Internal(InternalLevelIndex, bInternalArcadeFlag);
		}
	}
}

ABackgroundMusic* AMenuController::GetBackgroundMusicActor()
{
	ABackgroundMusic* Music = nullptr;

	for (TActorIterator<ABackgroundMusic> Background(GetWorld()); Background; ++Background)
	{
		if (*Background)
		{
			Music = *Background; 
			break;
		}
	}

	return Music;
}

void AMenuController::PlayDemoMovie()
{
	DemoMovie::PlayDemoMovie();
}

void AMenuController::SetPlayDemo(bool bInFlag)
{
	bPlayDemo = bInFlag;
}

bool AMenuController::GetPlayDemo()
{
	return bPlayDemo;
}