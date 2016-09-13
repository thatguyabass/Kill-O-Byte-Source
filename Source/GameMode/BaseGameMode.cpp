// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BaseGameMode.h"
#include "Online/SnakeInstance.h"
#include "Online/SnakeSession.h"
#include "SnakeCharacter/SnakeLink.h"
#include "SnakeCharacter/SnakePlayer.h"
#include "GameState/SnakeGameState.h"
#include "GameMode/PlayerStart/CustomPlayerStart.h"
#include "HUD/SnakeHUD.h"
#include "Bots/Bot/Bot.h"
#include "Viewport/SnakeViewportClient.h"
#include "UserData/UserData.h"
#include "UserData/SnakeLocalPlayer.h"
#include "PlayerStart/CheckPoint.h"

ABaseGameMode::ABaseGameMode(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	static ConstructorHelpers::FClassFinder<APawn> Pawn(TEXT("/Game/Blueprints/Character/BP_Snake_Link_Head"));
	DefaultPawnClass = Pawn.Class;

	static ConstructorHelpers::FClassFinder<APlayerController> PC(TEXT("/Game/Blueprints/Character/BP_SnakePlayer"));
	PlayerControllerClass = PC.Class;

	PlayerStateClass = ASnakePlayerState::StaticClass();
	SpectatorClass = ASpectatorPawn::StaticClass();
	HUDClass = AHUD::StaticClass();
	GameStateClass = ASnakeGameState::StaticClass();

	WarmupTime = 10;
	RoundTime = 300;
	PostGameTime = 10;

	//Scores
	SnakePoints = 100;
	LinkPoints = 10;
	DeathPoints = -20;
	BotKillPoints = 75;
	
	bUseSeamlessTravel = true;
}

void ABaseGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ABaseGameMode::InitGameState()
{
	Super::InitGameState();
}

FString ABaseGameMode::InitNewPlayer(APlayerController* NewPlayerController, const TSharedPtr<const FUniqueNetId>& UniqueId, const FString& Options, const FString& Portal)
{
	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

void ABaseGameMode::HandleMatchIsWaitingToStart()
{
	if (bDelayedStart)
	{
		//Start Warmup
		ASnakeGameState* const MyGameState = Cast<ASnakeGameState>(GameState);
		if (MyGameState && MyGameState->RemainingTime == 0)
		{
			if (WarmupTime > 0.0f)
			{
				MyGameState->RemainingTime = WarmupTime;
			}
			else
			{
				MyGameState->RemainingTime = 0.0f;
			}
		}
	}
}

void ABaseGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	ASnakeGameState* const MyGameState = Cast<ASnakeGameState>(GameState);
	MyGameState->RemainingTime = RoundTime;
	
	// Notify Players
	for (FConstControllerIterator Player = GetWorld()->GetControllerIterator(); Player; Player++)
	{
		AMasterController* PC = Cast<AMasterController>(*Player);
		if (PC)
		{
			PC->ClientStartGame();
		}
	}
}

bool ABaseGameMode::IsWinner(ASnakePlayerState* PlayerState) const
{
	//Override this
	return false;
}

void ABaseGameMode::DetermineWinner()
{
	//Override this
}

void ABaseGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AMasterController* PlayerController = Cast<AMasterController>(NewPlayer);

	if (PlayerController && IsMatchInProgress())
	{
		PlayerController->ClientStartGame();
	}
}

void ABaseGameMode::FinishMatch()
{
	if (IsMatchInProgress())
	{
		EndMatch();
		DetermineWinner();

		for (FConstControllerIterator Controller = GetWorld()->GetControllerIterator(); Controller; Controller++)
		{
			ASnakePlayerState* PlayerState = Cast<ASnakePlayerState>((*Controller)->PlayerState);
			bool bIsWinner = IsWinner(PlayerState);

			(*Controller)->GameHasEnded(nullptr, bIsWinner);
		}

		for (FConstPawnIterator Pawn = GetWorld()->GetPawnIterator(); Pawn; Pawn++)
		{
			if((*Pawn)->IsPendingKill())
			{
				continue;
			}

			(*Pawn)->DetachFromControllerPendingDestroy();

			ASnakeLink* Link = Cast<ASnakeLink>(*Pawn);
			if (Link && Link->IsHead())
			{
				Link->DespawnAndDestroy();
			}

			ABot* Bot = Cast<ABot>(*Pawn);
			if(Bot)
			{
				Bot->HideAndDestroy();
			}
		}

		ASnakeGameState* const MyGameState = Cast<ASnakeGameState>(GameState);
		MyGameState->RemainingTime = PostGameTime;
	}
}

void ABaseGameMode::RequestFinishAndExitToMainMenu()
{
	FinishMatch();

	AMasterController* PlayerController = nullptr;
	for (FConstPlayerControllerIterator Controller = GetWorld()->GetPlayerControllerIterator(); Controller; Controller++)
	{
		AMasterController* Player = Cast<AMasterController>(*Controller);
		if (Player && !Player->IsLocalController())
		{
			Player->ClientReturnToMainMenu(TEXT("Host has left the Game"));
		}
		else
		{
			PlayerController = Player;
		}
	}

	//This could be called at during a state end in the Game Instance 
	if (PlayerController)
	{
		PlayerController->HandleReturnToMainMenu();
	}
}

void ABaseGameMode::TryAgain()
{
	USnakeViewportClient* View = Cast<USnakeViewportClient>(GetWorld()->GetGameViewport());
	if (View)
	{
		View->ShowLoadingScreen();
	}

	USnakeInstance* Instance = Cast<USnakeInstance>(GetGameInstance());
	if (Instance)
	{
		Instance->ShowLoadingScreen();
	}

	for (FConstPlayerControllerIterator Controller = GetWorld()->GetPlayerControllerIterator(); Controller; Controller++)
	{
		ASnakePlayer* Player = Cast<ASnakePlayer>(*Controller);
		if (Player)
		{
			Player->RemoveAllWidgets();
			Player->OnGameRestart.Broadcast();
		}
	}

	RestartGame();
}

void ABaseGameMode::TriggerEndgameScorescreen(int32 Score)
{
	ASnakePlayer* Controller = Cast<ASnakePlayer>(GetWorld()->GetFirstPlayerController());
	if (Controller)
	{
		Controller->AddEndGameScoreboard(-1);
		Controller->OnScoreAssigned(Score);
	}
}

TSubclassOf<AGameSession> ABaseGameMode::GetGameSessionClass() const
{
	return ASnakeSession::StaticClass();
}

void ABaseGameMode::LinkKilled(ASnakePlayerState* Killer)
{
	if (Killer)
	{
		Killer->ScoreLinkKill(LinkPoints);
	}
}

void ABaseGameMode::SnakeKilled(ASnakePlayerState* Killer, ASnakePlayerState* Victim)
{
	if (Killer && Killer != Victim)
	{
		Killer->ScoreSnakeKill(SnakePoints);
	}

	if (Victim)
	{
		Victim->ScoreDeath(DeathPoints);
	}
}

void ABaseGameMode::BotKilled(float Points, ASnakePlayerState* Killer)
{
	if (Killer)
	{
		Killer->ScoreBotKill(Points);
	}
}

/** MultiPlayer */
void ABaseGameMode::TravelToLobby()
{
	USnakeInstance* Instance = Cast<USnakeInstance>(GetWorld()->GetGameInstance());
	Instance->GoToState(SGameInstanceState::Lobby);
	Instance->ShowLoadingScreen();

	FString TravelURL = "/Game/Maps/Lobby?listen?game=Lobby";
	GetWorld()->ServerTravel(TravelURL);
}

/** Single Player */
void ABaseGameMode::TravelToScorescreen()
{
	USnakeInstance* Instance = Cast<USnakeInstance>(GetWorld()->GetGameInstance());
	Instance->GoToState(SGameInstanceState::StoryScorescreen);
	Instance->ShowLoadingScreen();
}

void ABaseGameMode::TravelToNextLevel()
{
	//Override
}

bool ABaseGameMode::CanEnterNextLevel() const
{
	return false;
}

void ABaseGameMode::FindAllCheckpoints()
{
	for (ACheckpoint* Checkpoint : Checkpoints)
	{
		Checkpoint->OnActiveCheckpointChange.Clear();
	}

	Checkpoints.Empty();

	for (TActorIterator<ACheckpoint> Checkpoint(GetWorld()); Checkpoint; ++Checkpoint)
	{
		ACheckpoint* Point = (*Checkpoint);
		if (Point)
		{
			Checkpoints.Add(Point);
			Checkpoints.Last()->OnActiveCheckpointChange.AddDynamic(this, &ABaseGameMode::SetActiveCheckpoint);

			if (Point->bStartingCheckpoint)
			{
				ActiveCheckpoint = Point;
			}
		}
	}
}

AActor* ABaseGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (Checkpoints.Num() == 0)
	{
		FindAllCheckpoints();
	}

	return ActiveCheckpoint ? ActiveCheckpoint : AGameMode::ChoosePlayerStart_Implementation(Player);
}

bool ABaseGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;
}

void ABaseGameMode::SetActiveCheckpoint(ACheckpoint* Checkpoint)
{
	if (Checkpoint)
	{
		if (ActiveCheckpoint)
		{
			ActiveCheckpoint->FlagActiveCheckpoint(false);
		}
		ActiveCheckpoint = Checkpoint;
	}
}

ACheckpoint* ABaseGameMode::GetActiveCheckpoint()
{
	return ActiveCheckpoint;
}

int32 ABaseGameMode::GetSnakePoints() const
{
	return SnakePoints;
}
int32 ABaseGameMode::GetLinkPoints() const
{
	return LinkPoints;
}
int32 ABaseGameMode::GetDeathPoints() const
{
	return DeathPoints;
}

UUserData* ABaseGameMode::GetSavedData()
{
	USnakeLocalPlayer* const LocalPlayer = Cast<USnakeLocalPlayer>(GetWorld()->GetGameInstance()->GetFirstGamePlayer());
	if (LocalPlayer)
	{
		return LocalPlayer->GetUserData();
	}

	return nullptr;
}

APawn* ABaseGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	// don't allow pawn to be spawned with any pitch or roll
	FRotator StartRotation(ForceInit);
	StartRotation.Yaw = StartSpot->GetActorRotation().Yaw;
	FVector StartLocation = StartSpot->GetActorLocation();

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = Instigator;
	SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save default player pawns into a map
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	APawn* ResultPawn = GetWorld()->SpawnActor<APawn>(GetDefaultPawnClassForController(NewPlayer), StartLocation, StartRotation, SpawnInfo);
	if (ResultPawn == NULL)
	{
		//UE_LOG(LogGameMode, Warning, TEXT("Couldn't spawn Pawn of type %s at %s"), *GetNameSafe(DefaultPawnClass), *StartSpot->GetName());
	}
	return ResultPawn;
}

int32 ABaseGameMode::GetLevelIndex() const
{
	return -1;
}