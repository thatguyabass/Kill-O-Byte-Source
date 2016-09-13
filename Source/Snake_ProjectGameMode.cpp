// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "Snake_ProjectGameMode.h"
#include "SnakeCharacter/SnakePlayer.h"
#include "SnakeCharacter/SnakeLink.h"
#include "Kismet/KismetSystemLibrary.h"

ASnake_ProjectGameMode::ASnake_ProjectGameMode(const class FObjectInitializer& PCIP)
	: Super(PCIP)//, INITIAL_SCORE(0)
{
	static ConstructorHelpers::FClassFinder<APawn> Pawn(TEXT("/Game/Blueprints/Character/BP_Snake_Link_Head"));
	if (Pawn.Class)
	{
		DefaultPawnClass = Pawn.Class;
	}

	//Lives get added during Begin Play - Ensuring the Value is Initialized
	Lives = 0;
	InitialLives = 3;
	//Score = INITIAL_SCORE;

	RespawnProgress = 0.0f;
	bPlayerDead = false;
	bGameOver = false;
}

void ASnake_ProjectGameMode::SetCurrentState(EGameState State)
{
	CurrentState = State;

	HandleNewState();
}
EGameState ASnake_ProjectGameMode::GetCurrentState()
{
	return CurrentState;
}

void ASnake_ProjectGameMode::HandleNewState()
{
	switch (CurrentState)
	{
	case EGameState::PreGame:
	{
		ASnakePlayer* Player = Cast<ASnakePlayer>(UGameplayStatics::GetPlayerController(this, 0));
		Player->UnPossess();
		break;
	}
	case EGameState::Playing: break;
	case EGameState::GameOver:
	{
		SetMouseState(true);

		for (auto Spawner : TimeSpawners)
		{
			//Turn off spawning
			Spawner->bSpawnEnabled = false;
		}

		bGameOver = true;
		break;
	}
	case EGameState::Reset:
	{
		SetMouseState(false);

		for (auto Spawner : TimeSpawners)
		{
			//Reset Time Spawners - This Method will reenabled spawning
			Spawner->Reset();
		}

		//Check and see if the player is dead. Work around for initial spawn which will spawn two characters if this check isn't performed 
		if(bPlayerDead)
			Respawn();

		AddLives(InitialLives);
		ResetScore();

		bGameOver = false;

		SetCurrentState(EGameState::Playing);
		break;
	}
	default: /* Do Nothing */ break;
	}
}

void ASnake_ProjectGameMode::PlayerDead(ACharacter* Character)
{
	if (Character)
	{
		RemoveLife();

		Character->DetachFromControllerPendingDestroy();
		ASnakeLink* Link = Cast<ASnakeLink>(Character);
		if (Link)
		{
			Link->HideAndDestroy();
		}
		bPlayerDead = true;
	}
}

int32 ASnake_ProjectGameMode::GetLives()
{
	return Lives;
}
void ASnake_ProjectGameMode::AddLives(int32 NewLives)
{
	Lives += NewLives;
}
void ASnake_ProjectGameMode::RemoveLife()
{
	Lives -= 1;
	if (Lives <= 0)
	{
		//SetCurrentState(EGameState::GameOver);
	}
}

int32 ASnake_ProjectGameMode::GetScore()
{
	return Score;
}
void ASnake_ProjectGameMode::AddScore(int32 NewScore)
{
	Score += NewScore;
}
void ASnake_ProjectGameMode::ResetScore()
{
	//Score = INITIAL_SCORE;
}

void ASnake_ProjectGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundSpawners;
	UGameplayStatics::GetAllActorsOfClass(this, ATimeSpawner::StaticClass(), FoundSpawners);

	for (auto Spawner : FoundSpawners)
	{
		ATimeSpawner* Time = Cast<ATimeSpawner>(Spawner);
		if (Time)
			TimeSpawners.Add(Time);
	}

	SetCurrentState(EGameState::Reset);
}

void ASnake_ProjectGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurrentState)
	{
		case EGameState::Playing:
		{
			if (bPlayerDead)
			{
				RespawnProgress += DeltaTime;
				if (RespawnProgress > MinRespawnDelay)
				{
					Respawn();
				}
			}
			break;
		}
		case EGameState::GameOver:
		{
			break;
		}
	}
}

void ASnake_ProjectGameMode::Respawn()
{
	//UWorld* World = GetWorld();
	//if(World)
	//{
	//	bPlayerDead = false;
	//	RespawnProgress = 0;

	//	ASnakePlayer* Player = Cast<ASnakePlayer>(UGameplayStatics::GetPlayerController(this, 0));

	//	AActor* PlayerStart = FindPlayerStart(Player);

	//	FVector Loc = PlayerStart->GetActorLocation();
	//	FRotator Rot = PlayerStart->GetActorRotation();

	//	ASnakeLink* NewSnake = World->SpawnActor<ASnakeLink>(DefaultPawnClass, Loc, Rot);
	//	Player->Possess(NewSnake);
	//}
}

void ASnake_ProjectGameMode::PlayAgain()
{
	SetCurrentState(EGameState::Reset);
}

void ASnake_ProjectGameMode::Quit()
{
	APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0);
	Controller->ConsoleCommand("Quit");
}

void ASnake_ProjectGameMode::SetMouseState(bool State)
{
	APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0);
	if (Controller)
	{
		Controller->bShowMouseCursor = State;
		Controller->bEnableClickEvents = State;
		Controller->bEnableMouseOverEvents = State;
	}
}

void ASnake_ProjectGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}