// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "OnlineHelper.h"
#include "SnakeInstance.h"

FOnlineHelper::~FOnlineHelper()
{
	auto Sessions = Online::GetSessionInterface();
	CleanupOnlinePrivilegeTask();
}

void FOnlineHelper::Construct(TWeakObjectPtr<USnakeInstance> _GameInstance, TWeakObjectPtr<ULocalPlayer> _PlayerOwner)
{
	check(_GameInstance.IsValid());
	auto Session = Online::GetSessionInterface();

	PlayerOwner = _PlayerOwner;
	GameInstance = _GameInstance;
}

ULocalPlayer* FOnlineHelper::GetPlayerOwner() const
{
	return PlayerOwner.Get();
}

void FOnlineHelper::CleanupOnlinePrivilegeTask()
{
	if (GameInstance.IsValid())
	{
		GameInstance->CleanupOnlinePrivilegeTask();
	}
}

void FOnlineHelper::OnUserCanPlayOnlineHost(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
	CleanupOnlinePrivilegeTask();
	if (PrivilegeResults == (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures)
	{
		if (GameInstance.IsValid())
		{
			GameInstance->SetIsOnline(true);
		}

	}
	else if (GameInstance.IsValid())
	{
		//Display lack of Online Privilege Message 
	}
}

void FOnlineHelper::OnJoinServer()
{
	StartOnlinePrivilegeTask(IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateSP(this, &FOnlineHelper::OnUserCanPlayOnlineJoin));
}

void FOnlineHelper::OnUserCanPlayOnlineJoin(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults)
{
	CleanupOnlinePrivilegeTask();
	if (PrivilegeResults == (uint32)IOnlineIdentity::EPrivilegeResults::NoFailures)
	{
		if (GameInstance.IsValid())
		{
			GameInstance->SetIsOnline(true);
		}
	}
	else if (GameInstance.IsValid())
	{
		//Display lack of Online Privilege Message 
	}
}

void FOnlineHelper::StartOnlinePrivilegeTask(const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate)
{
	//if (GameInstance.IsValid())
	//{
	//	TSharedPtr<FUniqueNetId> UserId;
	//	if (PlayerOwner.IsValid())
	//	{
	//		UserId = PlayerOwner->GetPreferredUniqueNetId();
	//	}
	//	GameInstance->StartOnlinePrivilegeTask(Delegate, EUserPrivileges::CanPlayOnline, UserId);
	//}
}