// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "GameMode_Lobby.h"
#include "HUD/Controllers/LobbyController.h"
#include "PlayerState/SnakePlayerState.h"
#include "GameState/SnakeGameState.h"

AGameMode_Lobby::AGameMode_Lobby(const FObjectInitializer& PCIP)
	: ABaseGameMode(PCIP)
{
	DefaultPawnClass = ASpectatorPawn::StaticClass();
	HUDClass = AHUD::StaticClass();

	static ConstructorHelpers::FClassFinder<APlayerController> Controller(TEXT("/Game/Blueprints/UI/BP_LobbyController"));
	PlayerControllerClass = Controller.Class;

	PlayerStateClass = ASnakePlayerState::StaticClass();
	GameStateClass = ASnakeGameState::StaticClass();

	bDelayedStart = false;
}

void AGameMode_Lobby::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AGameMode_Lobby::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ALobbyController* Controller = Cast<ALobbyController>(Exiting);
	if (Controller)
	{
		Controller->ClientHideLobby();
	}
}

void AGameMode_Lobby::FinishMatch()
{
	//Nothing, no match has been started
}

void AGameMode_Lobby::DefaultTimer()
{
	//Nothing, Prevent the base DefaultTimer from being called 
}

AActor* AGameMode_Lobby::ChoosePlayerStart_Implementation(AController* Player)
{
	return nullptr;
}