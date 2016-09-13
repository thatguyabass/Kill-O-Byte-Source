// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "MenuGameMode.h"
#include "Online/SnakeSession.h"
#include "HUD/Controllers/MenuController.h"

AMenuGameMode::AMenuGameMode(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	static ConstructorHelpers::FClassFinder<AMenuController> MenuControllerFinder(TEXT("/Game/Blueprints/UI/BP_MenuController"));
	PlayerControllerClass = MenuControllerFinder.Class;
}

void AMenuGameMode::RestartPlayer(AController* NewPlayer)
{
	//Don't spawn a player for the menu
}

TSubclassOf<AGameSession> AMenuGameMode::GetGameSessionClass() const
{
	return ASnakeSession::StaticClass();
}

void AMenuGameMode::PostLogin(APlayerController* InPlayer)
{
	Super::PostLogin(InPlayer);

	InPlayer->SetCinematicMode(true, false, false, true, true);
	InPlayer->bShowMouseCursor = true;
}