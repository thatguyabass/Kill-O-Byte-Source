// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "ServerListItem.h"

UServerListItem::UServerListItem(const FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

void UServerListItem::SetServerEntry(FServerEntry Entry)
{
	ServerEntry = Entry;
}

FString UServerListItem::GetServerName() const
{
	return ServerEntry.ServerName;
}

FString UServerListItem::GetPlayerCount()
{
	return FString("8 / 1");
}

FString UServerListItem::GetPing() const
{
	return ServerEntry.Ping;
}

int32 UServerListItem::GetServerEntryIndex() const
{
	return ServerEntry.SearchResultsIndex;
}