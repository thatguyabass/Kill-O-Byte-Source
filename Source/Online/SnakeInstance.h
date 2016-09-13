// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "OnlineSessionInterface.h"
#include "OnlineIdentityInterface.h"
#include "SnakeInstance.generated.h"

class FVariantData;

namespace SGameInstanceState
{
	extern const FName None;
	extern const FName MainMenu;
	extern const FName Lobby;
	extern const FName PendingInvite;
	extern const FName Playing;
	extern const FName StoryScorescreen;
}

class FPendingInvite
{
public:
	FPendingInvite()
		: ControllerId(-1),
		bPrivilegesCheckedAndAllowed(false)
	{}

	int32 ControllerId;
	TSharedPtr<FUniqueNetId> UserId;
	FOnlineSessionSearchResult InviteResult;
	bool bPrivilegesCheckedAndAllowed;
};

UCLASS()
class SNAKE_PROJECT_API USnakeInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	USnakeInstance(const class FObjectInitializer& PCIP);
	
	bool Tick(float DeltaTime);

	virtual void Init() override;
	virtual void Shutdown() override;

	/** Start this Game Instance*/
	virtual void StartGameInstance() override;

	/** Start this game instance from within the Editor */
	//bool StartPIEGameInstance(ULocalPlayer* LocalPlayer, bool bInSimulatedInEditor, bool bAnyBlueprintErrors, bool bStartInSpectatorMode);

	class ASnakeSession* GetGameSession() const;

	/** Travel directly to a Session */
	void TravelToSession(const FName& SessionName);

	/** Server Travel */
	//void ServerTravel(FString InTravelURL);

	/** Host a game session */
	bool HostGame(ULocalPlayer* LocalPlayer, const FString& GameType, const FString& InTravelURL);

	/** Join a Game session using the Search Session Index*/
	bool JoinSession(ULocalPlayer* LocalPlayer, int32 SessionIndexInSearchResults);

	/** Join a Game session using the Online Search Results */
	bool JoinSession(ULocalPlayer* LocalPlayer, const FOnlineSessionSearchResult& SearchResults);

	/** Initialize the Session Searching */
	bool FindSessions(ULocalPlayer* LocalPlayer, bool bLanMatch);

	void GoToState(FName State);

	void RemoveExistingLocalPlayer(ULocalPlayer* ExistingPlayer);

	TSharedPtr<FUniqueNetId> GetUniqueNetIdFromControllerId(const int ControllerId);

	/** Is this game Online? */
	bool GetIsOnline() { return bIsOnline; }

	/** Set the Online Flag */
	void SetIsOnline(bool bIsOnline);

	/** Add local players to party (SplitScreen) */
	//void AddPlayerToParty();

	/** Remove Local player from party */
	//void RemovePlayersFromParty();

	/** Start Task to get user privileges */
	void StartOnlinePrivilegeTask(const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate, EUserPrivileges::Type Privilege, TSharedPtr<FUniqueNetId> UserId);

	/** Clean up the Checking Privilege State */
	void CleanupOnlinePrivilegeTask();

	/** Check if the player is able to play online */
	bool ValidatePlayerForOnlinePlay(ULocalPlayer* LocalPlayer);

	/** Shuts down the Session, and frees any net drivers */
	void CleanupSessionOnReturnToMenu();

	/** Flag user as a quiter */
	void LabelPlayerAsQuitter(ULocalPlayer* LocalPlayer) const;

	/** User Has a License */
	bool HasLicense() const { return bIsLicensed; }

	/** Display the Loading Screen using the Loading Screen Module. The display will auto remove itself after the load is completed */
	void ShowLoadingScreen();
	
private:
	UPROPERTY(config)
	FString MainMenuMap;

	FName CurrentState;
	FName PendingState;

	FPendingInvite PendingInvite;

	/** are we online? */
	bool bIsOnline;

	/** Does the User have a license to player the game */
	bool bIsLicensed;

	/** Loading Screen */
	TSharedPtr<class FSnakeLoadingScreen> LoadingScreenWidget;

	/** Main Menu */
	//TSharedPtr<class FMainMenu> MainMenuUI;

	/** URL to travel to after pending network operations */
	FString TravelURL;

	/** Controller to Ignore for pairing changes. -1 to skip */
	int32 IgnorePairingChangeForControllerId;

	EOnlineServerConnectionStatus::Type CurrentConnectionStatus;

	/** Ticker Callback Delegate*/
	FTickerDelegate TickDelegate;

	/** Tick Delegate Handle */
	FDelegateHandle TickDelegateHandle;
	FDelegateHandle TravelLocalSessionFailureDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	FDelegateHandle OnSearchSessionsCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnEndSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	FDelegateHandle OnCreatePresenceSessionCompleteDelegateHandle;

	void HandleSessionInviteAccepted(int32 LocalUserNum, bool bWasSuccessful, const FOnlineSessionSearchResult& SearchResult);
	//void HandleSessionInviteAccepted(const bool bWasSuccesful, const int32 ControllerId, TSharedPtr<FUniqueNetId> UserId, const FOnlineSessionSearchResult &InviteResult);
	void HandleSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, TSharedPtr<FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult);

	void HandleNetworkConnectionStatusChanged(EOnlineServerConnectionStatus::Type ConnectionStatus);

	/** Callback which is intended to be called upon finding sessions */
	void OnSearchSessionsComplete(bool bWasSuccessful);

	bool LoadFrontEndMap(const FString& MapName);

	void OnPreLoadMap();
	void OnPostLoadMap();

	/** Delegate Function executed after checking privileges for starting quick match */
	void OnUserCanPlayInvite(const FUniqueNetId& UserId, EUserPrivileges::Type Privileges, uint32 PrivilegeResults);

	/** Delegate for ending a session */
	FOnEndSessionCompleteDelegate OnEndSessionCompleteDelegate;

	void OnEndSessionComplete(FName SessionName, bool bWasSuccessful);

	void HandleStateChange();
	void EndCurrentState();
	void BeginNewState(FName NewState);

	/** Display the Main Menu to the player */
	void BeginMainMenuState();
	/** Remove the Main Menu from the Users viewport */
	void EndMainMenuState();

	void BeginLobbyState();
	void EndLobbyState();

	void BeginPendingInviteState();
	void EndPendingInviteState();

	void BeginPlayingState();
	void EndPlayingState();

	void BeginStoryScorescreenState();
	void EndStoryScorescreenState();

	void AddNetworkFailureHandlers();
	void RemoveNetworkFailureHandlers();

	/** Called when an error has occured when traveling to a local session */
	void TravelLocalSessionFailure(UWorld* World, ETravelFailure::Type Type, const FString& ErrorString);

	/** Called upon joining a session */
	void OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result);

	/** Callback which is intended to be called upon session creation */
	void OnCreatePresenceSessionComplete(FName SessionName, bool bWasSuccessful);

	/** Callback is called after adding all local players to the session */
	void OnRegisterLocalPlayerComplete(const FUniqueNetId& PlayerId, EOnJoinSessionCompleteResult::Type Result);

	/** Called after all the local players are registered */
	void FinishSessionCreation(EOnJoinSessionCompleteResult::Type Result);

	/** Callback which is called after adding local players to a session they're joining */
	void OnRegisterJoiningLocalPlayerComplete(const FUniqueNetId& PlayerId, EOnJoinSessionCompleteResult::Type Result);

	/** Called after all the local players are registered in the session they are joining */
	void FinishJoinSession(EOnJoinSessionCompleteResult::Type Result);

	bool IsLocalPlayerOnline(ULocalPlayer* LocalPlayer);

	/** Sets a rich presence string for all local players. */
	void SetPresenceForLocalPlayers(const FVariantData& Presence);

	/** Travel Directly to the name session */
	void InternalTravelToSession(const FName& SessionName);

	/** Time Delegate Handle */
	FTimerHandle TimerHandle;
};
