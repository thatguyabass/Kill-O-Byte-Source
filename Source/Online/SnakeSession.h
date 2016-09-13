// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CustomSessionSettings.h"
#include "Online.h"
#include "GameFramework/GameSession.h"
#include "SnakeSession.generated.h"

struct FGameSessionParams
{
	/** Name of Session Settings are stored with */
	FName SessionName;
	/** LAN Match */
	bool bIsLAN;
	/** Presence enabled session */
	bool bIsPresence;
	/** Id of player initiating lobby */
	TSharedPtr<FUniqueNetId> UserId;
	/** Current Search Result Choice to join */
	int32 BestSessionIndex;

	FGameSessionParams()
		: SessionName(NAME_None),
		bIsLAN(false),
		bIsPresence(false),
		BestSessionIndex(0)
	{
	}
};

UCLASS()
class SNAKE_PROJECT_API ASnakeSession : public AGameSession
{
	GENERATED_BODY()
	
public:
	ASnakeSession(const class FObjectInitializer& PCIP);
	
protected:
	/** Delegate for creating a new session */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	/** Delegate after starting a session */
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	/** Delegate for destroying a session */
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	/** Delegate for searching for sessions */
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	/** Delegate after joining a session */
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	/** Transient properties of a session during game creation/matchmaking */
	FGameSessionParams CurrentSessionParams;
	/** Current Host Settings */
	TSharedPtr<FCustomSessionSettings> HostSettings;
	/** Current Search Settings */
	TSharedPtr<FCustomOnlineSearchSettings> SearchSettings;

	/**
	* Delegate fired when a session create request has completed
	*
	* @Param SessionName the name of the session this callback is for
	* @Param bWasSuccessful true if the async action completed without error, false if an error occurred
	*/
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	* Delegate fired when a destroying an online session has completed
	*
	* @param SessionName the name of the session this callback is for
	* @param bWasSucessful True if async action completed without any errors
	*/
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	* Delegate fired when a session search query has completed
	*
	* @param bWasSucessful True if async action completed without any errors
	*/
	virtual void OnFindSessionsComplete(bool bWasSuccessful);

	/**
	* Delegate fired when a session join request has completed
	*
	* @param SessionName the name of the session this callback is for
	* @param bWasSucessful True if async action completed without any errors
	*/
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/**
	* Delegate fired when a session start request has completed
	*
	* @param SessionName the name of the session this callback is for
	* @param bWasSucessful True if async action completed without any errors
	*/
	virtual void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

	/**
	* Event Triggered when a presence session is created
	*
	* @ param SessionName Name of session that was created
	* @ param bWasSuccessful was the create successful
	*/
	DECLARE_EVENT_TwoParams(ANetworkSession, FOnCreatePresenceSessionComplete, FName, bool)
	FOnCreatePresenceSessionComplete CreatePresenceSessionCompleteEvent;

	DECLARE_EVENT_OneParam(ANetworkSession, FOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type);
	FOnJoinSessionComplete JoinSessionCompleteEvent;

	/*
	* Event triggered after session search completes
	*/
	DECLARE_EVENT_OneParam(ANetworkSession, FOnFindSessionsComplete, bool);
	FOnFindSessionsComplete FindSessionsCompleteEvent;

	/** Reset the Valiables that are keeping track of session join attempts */
	void ResetBestSessionVariables();

	/** Choose the best session from a list of search results */
	void ChooseBestSession();

	/** Entry point for matchmaking after search results are returned */
	void StartMatchmaking();

	/** Return point after each attempt to join a search result */
	void ContinueMatchmaking();

	/** Delegate triggered when no more search results are available */
	void OnNoMatchesAvailable();

public:

	/** Default Number of players allowed in a game */
	static const int32 DEFAULT_PLAYER_NUM = 8;

	/**
	*
	* @param UserId User that initiated the Request
	* @param SessionName Name of Session
	* @param bIsLAN is this going to host over LAN
	* @param bIsPresence is this session to create a presence session
	* @param MaxPlayerCount Maximum number of players allowed to connect
	*
	* @return bool true if successful, false otherwise
	*/
	bool HostSession(TSharedPtr<FUniqueNetId> UserID, FName SessionName, const FString& GameType, const FString& MapName, bool bIsLAN, bool IsPresence, int32 MaxPlayerCount);

	/**
	* Find an Online Session
	*
	* @param UserId user that initiated the request
	* @param SessionName name of session this search will generate
	* @param bIsLan are we searching for a LAN match
	* @param bIsPresence are we searching for a presence session
	*/
	void FindSessions(TSharedPtr<FUniqueNetId> UserId, FName SessionName, bool bIsLan, bool bIsPresence);

	/**
	* Find an Online Session
	*
	* @param UserId user that initiated the request
	* @param SessionName name of session this search will generate
	* @param SessionINdexInSearchResults Index of the Session found in the Search
	*
	* @return bool true if successful, else otherwise
	*/
	bool JoinSession(TSharedPtr<FUniqueNetId> UserId, FName SessionName, int32 SessionIndexInSearchResults);

	/**
	* Find an Online Session
	*
	* @param UserId user that initiated the request
	* @param SessionName name of session this search will generate
	* @param SearchResult Session to join
	*
	* @return bool true if successful
	*/
	bool JoinSession(TSharedPtr<FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	/** true if online async work in progress */
	bool IsBusy() const;

	/**
	* Get Search results found and the current search result
	*
	* @param SearchResultIndex Current Search result being grabbed
	* @param NumSearchResults Number of total search results found in FindGame()
	*
	* @return State of Search result
	*/
	EOnlineAsyncTaskState::Type GetSearchResultStatus(int32& SearchResultIndex, int32& NumSearchResults);

	/** Handle Match Start */
	virtual void HandleMatchHasStarted() override;

	/** Handle Match End */
	virtual void HandleMatchHasEnded() override;

	/** Get the Search results */
	const TArray<FOnlineSessionSearchResult>& GetSearchResults() const;

	/** @return the delegate fired when creating a presence session */
	FOnCreatePresenceSessionComplete& OnCreatePresenceSessionComplete() { return CreatePresenceSessionCompleteEvent; }

	/** @return the Delegate fired when joining a session */
	FOnJoinSessionComplete& OnJoinSessionComplete() { return JoinSessionCompleteEvent; }

	/** @return the delegate fired when search of sessions completes */
	FOnFindSessionsComplete& OnFindSessionsComplete() { return FindSessionsCompleteEvent; }

	/**
	* Travel to session URL as client for given session
	*
	* @param ControllerId Controller initialting the session travel
	* @param SessionName name of the session the client is traveling to
	*
	* @return bool true if successful
	*/
	bool TravelToSession(int32 ControllerId, FName SessionName);

	/** Handles to various registered delegates */
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

};