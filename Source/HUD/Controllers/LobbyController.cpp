// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "LobbyController.h"
#include "Online/SnakeInstance.h"
#include "GameMode/PlayerState/SnakePlayerState.h"
#include "Online.h"

ALobbyController::ALobbyController(const FObjectInitializer& PCIP)
	: AMasterController(PCIP)
{
	bLobbyUp = false;
	bLaunchStarted = false;
	LaunchTime = 5.0f;

	MapName = "Lobby";
	bReplicates = true;
}

void ALobbyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALobbyController::ClientStartGame_Implementation()
{
	FindLobbyInfo();
	if (!bLobbyUp)
	{
		AddLobbySlowConnect();
	}
}

void ALobbyController::ClientGameEnded_Implementation(AActor* EndGameFocus, bool bIsWinner)
{
	Super::ClientGameEnded_Implementation(EndGameFocus, bIsWinner);
}

void ALobbyController::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
{
	Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);

	//Remove Lobby 
	RemoveLobby();
}

void ALobbyController::ClientHideLobby_Implementation()
{
	//Remove Lobby 
	RemoveLobby();
}

void ALobbyController::SetupInputComponent()
{

}

void ALobbyController::StartLaunch(FString InMapName)
{
	if (!bLaunchStarted)
	{
		bLaunchStarted = true;
		Info->bLaunchStarted = true;

		MapName = InMapName;
		GetWorld()->GetTimerManager().SetTimer(Launch_TimeHandle, this, &ALobbyController::StartGame, LaunchTime, false);
	}
}

void ALobbyController::StartGame()
{
	USnakeInstance* Instance = Cast<USnakeInstance>(GetWorld()->GetGameInstance());
	Instance->GoToState(SGameInstanceState::Playing);

	TravelURL = FString::Printf(TEXT("/Game/Maps/%s?listen?game=%s"), *MapName, *GetGameModeType());

	GetWorld()->ServerTravel(TravelURL, false, false);
}

void ALobbyController::BackToMenu()
{
	USnakeInstance* Instance = Cast<USnakeInstance>(GetWorld()->GetGameInstance());
	if (Instance)
	{
		RemoveLobby();
		Instance->GoToState(SGameInstanceState::MainMenu);
	}
}

FString ALobbyController::GetLaunchTime()
{
	FTimerManager& Manager = GetWorld()->GetTimerManager();
	if (Launch_TimeHandle.IsValid())
	{
		const float MatchTime = Manager.GetTimerRemaining(Launch_TimeHandle);
		if (MatchTime < 0)
		{
			return FString("0");
		}

		const int32 Seconds = (int32)(MatchTime) % 60;
		const FString Time = FString::Printf(TEXT("%d"), Seconds);

		return Time;
	}

	return FString("0");
}

void ALobbyController::FindLobbyInfo()
{
	for (TActorIterator<ALobbyInfo> Iter(GetWorld()); Iter; ++Iter)
	{
		if (*Iter)
		{
			Info = *Iter;
			break;
		}
	}

	if (Info)
	{
		ASnakePlayerState* State = Cast<ASnakePlayerState>(PlayerState);
		if (State)
		{
			//Turn this into a direct color transfer. 
			State->SetTeamColor(Info->GetColorData(State->GetTeamNumber()));
		}
	}
}

void ALobbyController::SetTeamID(int32 TeamNumber)
{
	if (Role < ROLE_Authority)
	{
		ServerSetTeamID(TeamNumber);
	}
	else
	{
		ASnakePlayerState* State = Cast<ASnakePlayerState>(PlayerState);
		if (State)
		{
			//Pass in team color as well
			State->SetTeamNumber(TeamNumber);
			if (Info)
			{
				State->SetTeamColor(Info->GetColorData(TeamNumber));
			}
			else
			{
				FindLobbyInfo();
			}
		}
	}
}

FString ALobbyController::GetGameModeType() const
{
	return Info->GetGameModeShortHand();
}

void ALobbyController::ServerSetTeamID_Implementation(int32 TeamNumber)
{
	SetTeamID(TeamNumber);
}

void ALobbyController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyController, Info);
}

void ALobbyController::AddLobby_Implementation()
{
	bLobbyUp = true;
}

void ALobbyController::AddLobbySlowConnect()
{
	if (LobbyMenu == nullptr)
	{
		AddLobby();

		//Recall this function to attempt again 
		GetWorld()->GetTimerManager().SetTimer(Refresh_TimerHandle, this, &ALobbyController::AddLobbySlowConnect, 0.1f, false);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, "Lobby Up");
	}
}