// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SnakeInstance.h"
#include "SnakeSession.h"
#include "GameMode/GameState/SnakeGameState.h"
#include "HUD/Controllers/MenuController.h"
#include "SnakeLoadingScreen/SnakeLoadingScreen.h"
#include "Viewport/SnakeViewportClient.h"

namespace SGameInstanceState
{
	const FName None = FName(TEXT("None"));
	const FName PendingInvite = FName(TEXT("PendingInvite"));
	const FName MainMenu = FName(TEXT("MainMenu"));
	const FName Lobby = FName(TEXT("Lobby"));
	const FName Playing = FName(TEXT("Playing"));
	const FName StoryScorescreen = FName(TEXT("StoryScorescreen"));
}

USnakeInstance::USnakeInstance(const class FObjectInitializer& PCIP)
	: Super(PCIP),
	bIsOnline(false)//Default true
{
	CurrentState = SGameInstanceState::None;
}

void USnakeInstance::Init()
{
	Super::Init();

	CurrentConnectionStatus = EOnlineServerConnectionStatus::Connected;

	//Game requires the ability to ID users
	const auto OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		//check(OnlineSub);
		const auto IdentityInterface = OnlineSub->GetIdentityInterface();
		check(IdentityInterface.IsValid());
		const auto SessionInterface = OnlineSub->GetSessionInterface();
		check(SessionInterface.IsValid());

		FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &USnakeInstance::OnPreLoadMap);
		FCoreUObjectDelegates::PostLoadMap.AddUObject(this, &USnakeInstance::OnPostLoadMap);

		//OnlineSub->AddOnConnectionStatusChangedDelegate_Handle(FOnConnectionStatusChangedDelegate::CreateUObject(this, &USnakeInstance::HandleNetworkConnectionStatusChanged));

		OnEndSessionCompleteDelegate = FOnEndSessionCompleteDelegate::CreateUObject(this, &USnakeInstance::OnEndSessionComplete);

		int32 ControllerId = 0;
		//SessionInterface->AddOnSessionInviteAcceptedDelegate_Handle(ControllerId, FOnSessionInviteAcceptedDelegate::CreateUObject(this, &USnakeInstance::HandleSessionInviteAccepted));
		//SessionInterface->AddOnSessionUserInviteAcceptedDelegate_Handle(FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &USnakeInstance::HandleSessionUserInviteAccepted));
	}		

	//Register delegate for tick callback
	TickDelegate = FTickerDelegate::CreateUObject(this, &USnakeInstance::Tick);
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate);
}

void USnakeInstance::Shutdown()
{
	Super::Shutdown();

	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}

void USnakeInstance::HandleSessionInviteAccepted(int32 LocalUserNum, bool bWasSuccessful, const FOnlineSessionSearchResult& InviteResult)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogOnline, Warning, TEXT("Invite accept returned no search result."));
		return;
	}

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			ULocalPlayer* Local = GetFirstGamePlayer();
			if (Local)
			{
				JoinSession(Local, InviteResult);
			}
		}
	}
}

void USnakeInstance::HandleSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, TSharedPtr<FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult)
{
	UE_LOG(LogOnline, Verbose, TEXT("HandleSessionUserInviteAccepted: bSuccess %d, ControllerId: %d User: &d"), bWasSuccessful, ControllerId, UserId.IsValid() ? *UserId->ToString() : TEXT("NULL"));

	if (!bWasSuccessful)
	{
		UE_LOG(LogOnline, Warning, TEXT("Invite accept returned no search result."));
		return;
	}

	if (!UserId.IsValid())
	{
		UE_LOG(LogOnline, Warning, TEXT("Invite Accept returned no User"));
		return;
	}

	//Set the Pending Invite and then go to the initial screen, which is where it will be processed 
	PendingInvite.ControllerId = ControllerId;
	PendingInvite.UserId = UserId;
	PendingInvite.InviteResult = InviteResult;
	PendingInvite.bPrivilegesCheckedAndAllowed = false;

	GoToState(SGameInstanceState::PendingInvite);
}

void USnakeInstance::HandleNetworkConnectionStatusChanged(EOnlineServerConnectionStatus::Type ConnectionStatus)
{
	UE_LOG(LogOnlineGame, Warning, TEXT("USnakeInstance::HandleNetworkConnectionStatusChanged: %s"), EOnlineServerConnectionStatus::ToString(ConnectionStatus));

	CurrentConnectionStatus = ConnectionStatus;
}

void USnakeInstance::StartGameInstance()
{
	GoToState(SGameInstanceState::MainMenu);
}

void USnakeInstance::OnPreLoadMap()
{
	//Enabled Split Screen here
}
void USnakeInstance::OnPostLoadMap()
{
	//Hide Loading Screen Here
}

void USnakeInstance::OnUserCanPlayInvite(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
	CleanupOnlinePrivilegeTask();

	if (PrivilegeResults == (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures)
	{
		if (UserId == *PendingInvite.UserId)
		{
			PendingInvite.bPrivilegesCheckedAndAllowed = true;
		}
	}
	else
	{
		//Display online privilege failure
		GoToState(SGameInstanceState::MainMenu);
	}
}

bool USnakeInstance::Tick(float DeltaTime)
{
	//Dedicated Server doesn't need to worry about game state
	if (IsRunningDedicatedServer())
	{
		return true;
	}

	//Check if the state has changed
	HandleStateChange();

	//If there is a pending invite, and we are still within the main menu, and the session has properly shutdown, accept it
	if (PendingInvite.UserId.IsValid() && PendingInvite.bPrivilegesCheckedAndAllowed && CurrentState == SGameInstanceState::PendingInvite)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid())
			{
				EOnlineSessionState::Type SessionState = Sessions->GetSessionState(GameSessionName);

				if (SessionState == EOnlineSessionState::NoSession)
				{
					ULocalPlayer* NewPlayer = GetFirstGamePlayer();

					if (NewPlayer)
					{
						NewPlayer->SetControllerId(PendingInvite.ControllerId);
						NewPlayer->SetCachedUniqueNetId(PendingInvite.UserId);
						SetIsOnline(true);
						JoinSession(NewPlayer, PendingInvite.InviteResult);
					}

					PendingInvite.UserId.Reset();
				}
			}
		}
	}

	return true;
}

bool USnakeInstance::HostGame(ULocalPlayer* LocalPlayer, const FString& GameType, const FString& InTravelURL)
{
	ASnakeSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		//Add callback delage for completion
		OnCreatePresenceSessionCompleteDelegateHandle = GameSession->OnCreatePresenceSessionComplete().AddUObject(this, &USnakeInstance::OnCreatePresenceSessionComplete);

		TravelURL = InTravelURL;
		bool const bIsLanMatch = InTravelURL.Contains(TEXT("?bIsLanMatch"));

		//determine the map name from the travelURL
		const FString& MapNameSubStr = "/Game/Maps/";
		const FString& ChoppedMapName = TravelURL.RightChop(MapNameSubStr.Len());
		const FString& MapName = ChoppedMapName.LeftChop(ChoppedMapName.Len() - ChoppedMapName.Find("?game"));

		//if (GameSession->HostSession(LocalPlayer->GetPreferredUniqueNetId(), GameSessionName, GameType, MapName, bIsLanMatch, true, ASnakeSession::DEFAULT_PLAYER_NUM))
		//{
		//	//If any erros occured in the above, pending state would be set
		//	if ((PendingState == CurrentState) || (PendingState == SGameInstanceState::None))
		//	{
		//		GoToState(SGameInstanceState::Lobby);
		//		return true;
		//	}
		//}
	}

	return false;
}

bool USnakeInstance::JoinSession(ULocalPlayer* LocalPlayer, int32 SessionIndexInSearchResults)
{
	ASnakeSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		AddNetworkFailureHandlers();

		//OnJoinSessionCompleteDelegateHandle = GameSession->OnJoinSessionComplete().AddUObject(this, &USnakeInstance::OnJoinSessionComplete);
		//if (GameSession->JoinSession(LocalPlayer->GetPreferredUniqueNetId(), GameSessionName, SessionIndexInSearchResults))
		//{
		//	// if any error occured above, pending will be set
		//	if (PendingState == CurrentState || PendingState == SGameInstanceState::None)
		//	{
		//		//Show Loading Screen
		//		GoToState(SGameInstanceState::Lobby);
		//		return true;
		//	}
		//}
	}

	return false;
}

bool USnakeInstance::JoinSession(ULocalPlayer* LocalPlayer, const FOnlineSessionSearchResult& SearchResult)
{
	ASnakeSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		AddNetworkFailureHandlers();

		//OnJoinSessionCompleteDelegateHandle = GameSession->OnJoinSessionComplete().AddUObject(this, &USnakeInstance::OnJoinSessionComplete);
		//if (GameSession->JoinSession(LocalPlayer->GetPreferredUniqueNetId(), GameSessionName, SearchResult))
		//{
		//	// if any error occured above, pending will be set
		//	if (PendingState == CurrentState || PendingState == SGameInstanceState::None)
		//	{
		//		//Show Loading Screen
		//		GoToState(SGameInstanceState::Lobby);
		//		return true;
		//	}
		//}
	}

	return false;
}

bool USnakeInstance::FindSessions(ULocalPlayer* LocalPlayer, bool bLanMatch)
{
	bool bResult = false;

	check(LocalPlayer != nullptr);
	if (LocalPlayer)
	{
		//ASnakeSession* const Session = GetGameSession();
		//if (Session)
		//{
		//	Session->OnFindSessionsComplete().RemoveAll(this);
		//	OnSearchSessionsCompleteDelegateHandle = Session->OnFindSessionsComplete().AddUObject(this, &USnakeInstance::OnSearchSessionsComplete);

		//	Session->FindSessions(LocalPlayer->GetPreferredUniqueNetId(), GameSessionName, bLanMatch, true);

		//	bResult = true;
		//}
	}

	return bResult;
}

void USnakeInstance::OnSearchSessionsComplete(bool bWasSuccessful)
{
	ASnakeSession* const Session = GetGameSession();
	if (Session)
	{
		//Session->OnFindSessionsComplete().RemoveUObject(this, &USnakeInstance::OnSearchSessionsComplete);
		Session->OnJoinSessionComplete().Remove(OnSearchSessionsCompleteDelegateHandle);
	}
}

ASnakeSession* USnakeInstance::GetGameSession() const
{
	UWorld* const World = GetWorld();
	if (World)
	{
		AGameMode* const Game = World->GetAuthGameMode();
		if (Game)
		{
			return Cast<ASnakeSession>(Game->GameSession);
		}
	}

	return nullptr;
}

void USnakeInstance::GoToState(FName State)
{
	PendingState = State;
}

void USnakeInstance::OnCreatePresenceSessionComplete(FName SessionName, bool bWasSuccessful)
{
	ASnakeSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		GameSession->OnCreatePresenceSessionComplete().Remove(OnCreatePresenceSessionCompleteDelegateHandle);

		//AddSplitScreen here

		FinishSessionCreation(bWasSuccessful ? EOnJoinSessionCompleteResult::Success : EOnJoinSessionCompleteResult::UnknownError);
	}
}
void USnakeInstance::FinishSessionCreation(EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		// Travel to the Specified Match URL
		GetWorld()->ServerTravel(TravelURL);
	}
	else
	{
		FString ReturnReason = NSLOCTEXT("NetworkErros", "CreateSessionFailed", "Failed to create session").ToString();
		FString OKButton = NSLOCTEXT("DialogButtons", "Okay", "OK").ToString();
	}
}

void USnakeInstance::OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result)
{
	//Unhook the delegate
	ASnakeSession* const GameSession = GetGameSession();
	if (GameSession)
	{
		GameSession->OnJoinSessionComplete().Remove(OnJoinSessionCompleteDelegateHandle);
	}

	//Add the Splitscreen players  Here

	//If there is only one player
	FinishJoinSession(Result);
}

void USnakeInstance::OnRegisterLocalPlayerComplete(const FUniqueNetId& PlayerId, EOnJoinSessionCompleteResult::Type Result)
{
	FinishSessionCreation(Result);
}
void USnakeInstance::OnRegisterJoiningLocalPlayerComplete(const FUniqueNetId& PlayerId, EOnJoinSessionCompleteResult::Type Result)
{
	FinishJoinSession(Result);
}

void USnakeInstance::FinishJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		//Failed to Join messages 

		RemoveNetworkFailureHandlers();
		return;
	}

	InternalTravelToSession(GameSessionName);
}

void USnakeInstance::InternalTravelToSession(const FName& SessionName)
{
	APlayerController* const PlayerController = GetFirstLocalPlayerController();

	if (!PlayerController)
	{
		//Return to menu
		return;
	}

	//Travel to Session
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (!OnlineSub)
	{
		//Return to Menu
		return;
	}

	FString URL;
	IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
	if (!Sessions.IsValid() || !Sessions->GetResolvedConnectString(SessionName, URL))
	{
		//Return to menu
		return;
	}

	PlayerController->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
}

void USnakeInstance::TravelToSession(const FName& SessionName)
{
	AddNetworkFailureHandlers();
	//Show Loading Screen

	InternalTravelToSession(SessionName);
}

bool USnakeInstance::LoadFrontEndMap(const FString& MapName)
{
	bool bSuccess = true;

	//Check if already loaded
	UWorld* const World = GetWorld();
	if (World)
	{
		const FString LevelName = World->PersistentLevel->GetOutermost()->GetName();
		if (LevelName == MapName)
		{
			return bSuccess;
		}
	}

	FString Error;
	EBrowseReturnVal::Type BrowseRet = EBrowseReturnVal::Failure;
	FURL URL(*FString::Printf(TEXT("%s"), *MapName));

	//Load the Front end Level
	if (URL.Valid && !HasAnyFlags(RF_ClassDefaultObject))
	{
		BrowseRet = GetEngine()->Browse(*WorldContext, URL, Error);

		//Handle any Error
		if (BrowseRet != EBrowseReturnVal::Success)
		{
			UE_LOG(LogLoad, Fatal, TEXT("%s"), *FString::Printf(TEXT("Failed to Enter %s: %s. Please check the log for errors."), *MapName, *Error));
			bSuccess = false;
		}
	}

	return bSuccess;
}

void USnakeInstance::HandleStateChange()
{
	if (PendingState != CurrentState && PendingState != SGameInstanceState::None)
	{
		//Stop the Current State
		EndCurrentState();

		//Start the New State
		BeginNewState(PendingState);

		//Reset the Pending State to a nutural State
		PendingState = SGameInstanceState::None;
	}
}

void USnakeInstance::EndCurrentState()
{
	if (CurrentState == SGameInstanceState::MainMenu)
	{
		EndMainMenuState();
	}
	else if (CurrentState == SGameInstanceState::Playing)
	{
		EndPlayingState();
	}
	else if (CurrentState == SGameInstanceState::StoryScorescreen)
	{
		EndStoryScorescreenState();
	}
	//else if (CurrentState == SGameInstanceState::Lobby)
	//{
	//	EndLobbyState();
	//}
	else if (CurrentState == SGameInstanceState::PendingInvite)
	{
		EndPendingInviteState();
	}

	//Reset the Current State
	CurrentState = SGameInstanceState::None;
}

void USnakeInstance::BeginNewState(FName NewState)
{
	CurrentState = NewState;

	if (CurrentState == SGameInstanceState::MainMenu)
	{
		BeginMainMenuState();
	}
	//else if (CurrentState == SGameInstanceState::Lobby)
	//{
	//	BeginLobbyState();
	//}
	else if (CurrentState == SGameInstanceState::Playing)
	{
		BeginPlayingState();
	}
	else if (CurrentState == SGameInstanceState::StoryScorescreen)
	{
		BeginStoryScorescreenState();
	}
	else if (CurrentState == SGameInstanceState::PendingInvite)
	{
		BeginPendingInviteState();
	}
}

void USnakeInstance::SetPresenceForLocalPlayers(const FVariantData& PresenceData)
{
	const auto Presence = Online::GetPresenceInterface();
	if (Presence.IsValid())
	{
		//for (int32 c = 0; c < LocalPlayers.Num(); c++)
		//{
		//	const TSharedPtr<FUniqueNetId> UserId = LocalPlayers[c]->GetPreferredUniqueNetId();

		//	if (UserId.IsValid())
		//	{
		//		FOnlineUserPresenceStatus PresenceStatus;
		//		PresenceStatus.Properties.Add(DefaultPresenceKey, PresenceData);

		//		Presence->SetPresence(*UserId, PresenceStatus);
		//	}
		//}
	}
}

void USnakeInstance::BeginMainMenuState()
{
	SetIsOnline(false);

	// Set presence to menu state for the owning player
	SetPresenceForLocalPlayers(FVariantData(FString(TEXT("OnMenu"))));

	//Load the Main Menu Map
	LoadFrontEndMap(MainMenuMap);

	ULocalPlayer* const Player = GetFirstGamePlayer();

	if (Player)
	{
		AMenuController* Controller = Cast<AMenuController>(Player->PlayerController);
		Controller->InitializeMenu();
		Controller->CreateOnlineHelper();

		RemoveNetworkFailureHandlers();
	}
}

void USnakeInstance::EndMainMenuState()
{
	AMenuController* Controller = Cast<AMenuController>(GetFirstGamePlayer()->PlayerController);
	if (Controller)
	{
		Controller->DestroyOnlineHelper();
		Controller->DestroyMenu();
	}
}

void USnakeInstance::BeginLobbyState()
{
	//Set Local Presence
	SetPresenceForLocalPlayers(FVariantData(FString(TEXT("InLobby"))));
}

void USnakeInstance::EndLobbyState()
{
	//RemovePlayersFromParty();

	if (PendingState != SGameInstanceState::Playing)
	{
		UWorld* const World = GetWorld();
		ASnakeGameState* const GameState = World != nullptr ? World->GetGameState<ASnakeGameState>() : nullptr;
		if (GameState)
		{
			GameState->RequestFinishAndExitToMainMenu();
		}
		else
		{
			CleanupSessionOnReturnToMenu();
		}
	}
}

void USnakeInstance::BeginPendingInviteState()
{
	if (LoadFrontEndMap(MainMenuMap))
	{
		StartOnlinePrivilegeTask(IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateUObject(this, &USnakeInstance::OnUserCanPlayInvite), EUserPrivileges::CanPlayOnline, PendingInvite.UserId);
	}
	else
	{
		GoToState(SGameInstanceState::MainMenu);
	}
}

void USnakeInstance::EndPendingInviteState()
{
	CleanupOnlinePrivilegeTask();
}

void USnakeInstance::BeginPlayingState()
{
	//Set SplitScreen

	//Set Presence for playing in a Map
	SetPresenceForLocalPlayers(FVariantData(FString(TEXT("InGame"))));

	// Make sure the viewport has focus
	FSlateApplication::Get().SetAllUserFocusToGameViewport();
}

void USnakeInstance::EndPlayingState()
{
	//Disable Splitscreen

	//Clear the players presence
	SetPresenceForLocalPlayers(FVariantData(FString(TEXT("OnMenu"))));

	UWorld* const World = GetWorld();
	ASnakeGameState* const GameState = World != nullptr ? World->GetGameState<ASnakeGameState>() : nullptr;
	if (GameState)
	{
		GameState->RequestFinishAndExitToMainMenu();
	}
	else
	{
		CleanupSessionOnReturnToMenu();
	}
}

void USnakeInstance::BeginStoryScorescreenState()
{
	//Initialize the Main Menu
	BeginMainMenuState();

	ULocalPlayer* const Player = GetFirstGamePlayer();
	AMenuController* Controller = Cast<AMenuController>(Player->PlayerController);
	if (Controller)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "In Story Scoreboard State");
	}
}

void USnakeInstance::EndStoryScorescreenState()
{
	//Temp
	EndMainMenuState();
}

void USnakeInstance::StartOnlinePrivilegeTask(const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate, EUserPrivileges::Type Privilege, TSharedPtr<FUniqueNetId> UserId)
{
	//Create Wait Message
	//Display Wait Message 

	auto Identity = Online::GetIdentityInterface();
	if (Identity.IsValid() && UserId.IsValid())
	{
		Identity->GetUserPrivilege(*UserId, Privilege, Delegate);
	}
	else
	{
		// Can only get away with faking the UniqueNetId here because the delegates don't use it
		Delegate.ExecuteIfBound(FUniqueNetIdString(), Privilege, (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures);
	}
}

void USnakeInstance::CleanupOnlinePrivilegeTask()
{
	if (GEngine && GEngine->GameViewport)
	{

	}
}

bool USnakeInstance::ValidatePlayerForOnlinePlay(ULocalPlayer* LocalPlayer)
{
	//Get the View port here
	//Display a message on the Viewport if the user doesn't have an online connection

	if (!IsLocalPlayerOnline(LocalPlayer))
	{
		//Not Online
		//Display Propt for the user so they are aware they aren't online
		return false;
	}

	return true;
}

bool USnakeInstance::IsLocalPlayerOnline(ULocalPlayer* LocalPlayer)
{
	if (!LocalPlayer)
	{
		return false;
	}

	//Find the Online Subsystem 
	const auto Online = IOnlineSubsystem::Get();
	if (Online)
	{
		//Find the users identity
		const auto IdentityInterface = Online->GetIdentityInterface();
		if (IdentityInterface.IsValid())
		{
			//do they have a unique ID
			auto UniqueId = LocalPlayer->GetCachedUniqueNetId();
			if (UniqueId.IsValid())
			{
				//Are they logged in?
				const auto LoginStatus = IdentityInterface->GetLoginStatus(*UniqueId);
				if (LoginStatus == ELoginStatus::LoggedIn)
				{
					//They are online
					return true;
				}
			}
		}
	}

	return false;
}

void USnakeInstance::SetIsOnline(bool bIsOnline)
{
	this->bIsOnline = bIsOnline;

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		for (int32 c = 0; c < LocalPlayers.Num(); c++)
		{
			ULocalPlayer* LocalPlayer = LocalPlayers[c];

			const TSharedPtr<const FUniqueNetId> PlayerID = LocalPlayer->GetPreferredUniqueNetId();
			if (PlayerID.IsValid())
			{
				OnlineSub->SetUsingMultiplayerFeatures(*PlayerID, bIsOnline);
			}
		}
	}
}

void USnakeInstance::RemoveNetworkFailureHandlers()
{
	//Remove the local session/travel failure bindings
	if (GEngine->OnTravelFailure().IsBoundToObject(this))
	{
		//GEngine->OnTravelFailure().RemoveUObject(this, &USnakeInstance::TravelLocalSessionFailure);
		GEngine->OnTravelFailure().Remove(TravelLocalSessionFailureDelegateHandle);
	}
}

void USnakeInstance::AddNetworkFailureHandlers()
{
	//Add local session/Travel failure Bindings
	if (!GEngine->OnTravelFailure().IsBoundToObject(this))
	{
		TravelLocalSessionFailureDelegateHandle = GEngine->OnTravelFailure().AddUObject(this, &USnakeInstance::TravelLocalSessionFailure);
	}
}

void USnakeInstance::TravelLocalSessionFailure(UWorld* World, ETravelFailure::Type Type, const FString& Reason)
{
	APlayerController* Player = UGameplayStatics::GetPlayerController(this, 0);
	if (Player)
	{
		GoToState(SGameInstanceState::MainMenu);
	}
}

void USnakeInstance::OnEndSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogOnline, Verbose, TEXT("USnakeInstance::OnEndSessionComplete : Session=%s bWasSuccessful=%s"), *SessionName.ToString(), bWasSuccessful ? TEXT("true") : TEXT("false"));
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
			Sessions->ClearOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegateHandle);
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
		}
	}

	//Continue Cleanup
	CleanupSessionOnReturnToMenu();
}

void USnakeInstance::CleanupSessionOnReturnToMenu()
{
	bool bPendingOnlineOp = false;

	//End online game and then destroy it
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			EOnlineSessionState::Type SessionState = Sessions->GetSessionState(GameSessionName);
			UE_LOG(LogOnline, Log, TEXT("Session %s is '%s'"), *GameSessionName.ToString(), EOnlineSessionState::ToString(SessionState));

			if (EOnlineSessionState::InProgress == SessionState)
			{
				UE_LOG(LogOnline, Log, TEXT("Ending session %s on return to main menu"), *GameSessionName.ToString());
				OnEndSessionCompleteDelegateHandle = Sessions->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
				Sessions->EndSession(GameSessionName);
				bPendingOnlineOp = true;
			}
			else if (EOnlineSessionState::Ending == SessionState)
			{
				UE_LOG(LogOnline, Log, TEXT("Waiting for session %s to end on return to main menu"), *GameSessionName.ToString());
				OnEndSessionCompleteDelegateHandle = Sessions->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
				bPendingOnlineOp = true;
			}
			else if (EOnlineSessionState::Ended == SessionState || EOnlineSessionState::Pending == SessionState)
			{
				UE_LOG(LogOnline, Log, TEXT("Destroying session %s on return to main menu"), *GameSessionName.ToString());
				OnDestroySessionCompleteDelegateHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
				Sessions->DestroySession(GameSessionName);
				bPendingOnlineOp = true;
			}
			else if (EOnlineSessionState::Starting == SessionState)
			{
				UE_LOG(LogOnline, Log, TEXT("Waiting for session %s to start, and then we will end it to return to main menu"), *GameSessionName.ToString());
				OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
				bPendingOnlineOp = true;
			}
		}
	}
}

void USnakeInstance::LabelPlayerAsQuitter(ULocalPlayer* LocalPlayer) const
{
	//Get Player State
	//Label them as a quitter
}

void USnakeInstance::ShowLoadingScreen()
{
	ISnakeLoadingScreenModule* const LoadingScreenModule = FModuleManager::LoadModulePtr<ISnakeLoadingScreenModule>("SnakeLoadingScreen");
	if (LoadingScreenModule)
	{
		LoadingScreenModule->StartInGameLoadingScreen();
	}
}

//void USnakeInstance::AddPlayerToParty()
//{
//	auto PartyInterface = Online::GetPartyInterface();
//	const auto FirstUniqueNetId = GetFirstGamePlayer() ? GetFirstGamePlayer()->GetPreferredUniqueNetId() : nullptr;
//	if (GetIsOnline() && PartyInterface.IsValid() && FirstUniqueNetId.IsValid())
//	{
//		TArray<TSharedRef<FUniqueNetId>> SplitscreenPlayerIds;
//		for (int32 c = 1; c < LocalPlayers.Num(); c++)
//		{
//			auto SplitscreenUniqueId = LocalPlayers[c]->GetPreferredUniqueNetId();
//			if (SplitscreenUniqueId.IsValid())
//			{
//				SplitscreenPlayerIds.Add(SplitscreenUniqueId.ToSharedRef());
//			}
//		}
//		PartyInterface->AddLocalUsers(*FirstUniqueNetId, SplitscreenPlayerIds, IOnlineParty::FOnLocalUserOperationCompleteDelegate());
//	}
//}
//
//void USnakeInstance::RemovePlayersFromParty()
//{
//	//Remove the local players from the party
//	auto PartyInterface = Online::GetPartyInterface();
//	if (GetIsOnline() && PartyInterface.IsValid())
//	{
//		TArray<TSharedRef<FUniqueNetId>> PlayerIds;
//		for (int32 c = 0; c < LocalPlayers.Num(); c++)
//		{
//			auto PlayerUniqueId = LocalPlayers[c]->GetPreferredUniqueNetId();
//			if (PlayerUniqueId.IsValid())
//			{
//				//Dont remove a player that has just accepted an invite
//				if (!PendingInvite.UserId.IsValid() || (*PlayerUniqueId != *PendingInvite.UserId))
//				{
//					PlayerIds.Add(PlayerUniqueId.ToSharedRef());
//				}
//			}
//		}
//		PartyInterface->RemoveLocalUsers(PlayerIds, IOnlineParty::FOnLocalUserOperationCompleteDelegate());
//	}
//}