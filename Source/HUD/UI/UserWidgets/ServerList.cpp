// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "ServerList.h"
#include "Online/SnakeInstance.h"
#include "Online/SnakeSession.h"

#define LOCTEXT_NAMESPACE "Snake_Project.HUD.Menu"

UServerList::UServerList(const FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

void UServerList::NativeTick(FGeometry MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bSearchingForServer)
	{
		UpdateSearchStatus();
	}
}

void UServerList::BeginServerSearch(bool bLanMatch)
{
	if (!bSearchingForServer)
	{
		bLanMatch = bLanMatch;
		bSearchingForServer = true;
		ServerList.Empty();

		USnakeInstance* const Instance = Cast<USnakeInstance>(GetOwningLocalPlayer()->GetGameInstance());
		if (Instance)
		{
			Instance->FindSessions(GetOwningLocalPlayer(), bLanMatch);
		}
	}
}

void UServerList::OnServerSearchFinished()
{
	bSearchingForServer = false;

	UpdateServerList();
}

void UServerList::UpdateServerList()
{
	for (int32 c = 0; c < 50; c++)
	{
		TSharedRef<FServerEntry> NewServerEntry = MakeShareable(new FServerEntry());

		NewServerEntry->ServerName = "Dummy Server " + FString::FromInt(c + 1);
		NewServerEntry->CurrentPlayers = "1";
		NewServerEntry->MaxPlayers = "8";
		NewServerEntry->Ping = "100";
		NewServerEntry->SearchResultsIndex = c;

		ServerList.Add(NewServerEntry);
	}

	//Call the Event and build the GUI in blueprint
	FillServerEntryData();
}

void UServerList::ConnectToServer(int32 ServerIndex)
{
	if (bSearchingForServer)
	{
		//Unsafe Check
		return;
	}

	USnakeInstance* Instance = Cast<USnakeInstance>(GetOwningLocalPlayer()->GetGameInstance());
	if (Instance)
	{
		Instance->JoinSession(GetOwningLocalPlayer(), ServerIndex);
	}
}

void UServerList::UpdateSearchStatus()
{
	check(bSearchingForServer);

	bool bFinishSearch = true;

	ASnakeSession* GameSession = GetGameSession();
	if (GameSession)
	{
		int32 CurrentSearchIndex, NumSearchResults;
		EOnlineAsyncTaskState::Type SearchState = GameSession->GetSearchResultStatus(CurrentSearchIndex, NumSearchResults);

		switch (SearchState)
		{
		case EOnlineAsyncTaskState::InProgress:
			StatusText = LOCTEXT("Searching", "SEARCHING").ToString();
			bFinishSearch = false;
			break;

		case EOnlineAsyncTaskState::Done:
		{
			//Copy the Results into a List View Item
			ServerList.Empty();
			const TArray<FOnlineSessionSearchResult>& SearchResult = GameSession->GetSearchResults();
			check(SearchResult.Num() == NumSearchResults);
			if (SearchResult.Num() == 0)
			{
				StatusText = LOCTEXT("NoServersFound", "No Servers Found. Press Space Bar to Refresh.").ToString();
			}
			else
			{
				StatusText = LOCTEXT("ServerFound", "Servers Found. Press Space Bar to Refresh.").ToString();
			}

			for (int32 Index = 0; Index < NumSearchResults; Index++)
			{
				TSharedRef<FServerEntry> NewServerEntry = MakeShareable(new FServerEntry());

				const FOnlineSessionSearchResult& Result = SearchResult[Index];

				NewServerEntry->ServerName = Result.Session.OwningUserName;
				NewServerEntry->Ping = FString::FromInt(Result.PingInMs);
				NewServerEntry->CurrentPlayers = FString::FromInt(Result.Session.SessionSettings.NumPublicConnections - Result.Session.SessionSettings.NumPrivateConnections -
					Result.Session.NumOpenPublicConnections - Result.Session.NumOpenPrivateConnections);
				
				NewServerEntry->MaxPlayers = FString::FromInt(Result.Session.SessionSettings.NumPrivateConnections + Result.Session.SessionSettings.NumPublicConnections);
				NewServerEntry->SearchResultsIndex = Index;

				Result.Session.SessionSettings.Get(SETTING_GAMEMODE, NewServerEntry->GameType);
				Result.Session.SessionSettings.Get(SETTING_MAPNAME, NewServerEntry->MapName);

				ServerList.Add(NewServerEntry);
			}
			break;
		}
		case EOnlineAsyncTaskState::Failed:
			break;

		case EOnlineAsyncTaskState::NotStarted:
			StatusText = "";
			break;
		}
	}

	if (bFinishSearch)
	{
		OnServerSearchFinished();
	}
}

ASnakeSession* UServerList::GetGameSession() const
{
	USnakeInstance* Instance = Cast<USnakeInstance>(GetOwningLocalPlayer()->GetGameInstance());

	return Instance ? Instance->GetGameSession() : nullptr;
}

int32 UServerList::GetServerListCount()
{
	return ServerList.Num();
}

FServerEntry UServerList::GetServerEntry(int32 Index)
{
	return ServerList.IsValidIndex(Index) ? *ServerList[Index].Get() : FServerEntry();
}

bool UServerList::IsSearchInProgress()
{
	return bSearchingForServer;
}

FString UServerList::GetStatusMessage()
{
	return StatusText;
}

#undef LOCTEXT_NAMESPACE //"Snake_Project.HUD.Menu"