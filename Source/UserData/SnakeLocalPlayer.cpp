// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SnakeLocalPlayer.h"
#include "Online/SnakeInstance.h"
#include "Online.h"
#include "OnlineSubsystemUtilsClasses.h"

USnakeLocalPlayer::USnakeLocalPlayer(const FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

//TSubclassOf<UOnlineSession> USnakeLocalPlayer::GetOnlineSessionClass()
//{
//	return UOnlineSessionClient::StaticClass();
//}

UUserData* USnakeLocalPlayer::GetUserData() const
{
	if (!UserData)
	{
		USnakeLocalPlayer* const MutableThis = const_cast<USnakeLocalPlayer*>(this);
		//Casting away constant to enable caching implementation
		MutableThis->LoadUserData();
	}
	return UserData;
}

void USnakeLocalPlayer::ReloadUserData()
{
	UserData = nullptr;

	GetUserData();
}

void USnakeLocalPlayer::LoadUserData()
{
	if (UserData && (GetControllerId() != UserData->GetUserIndex() || GetNickname() != UserData->GetName()))
	{
		UserData->SaveIfDirty();
		UserData = nullptr;
	}

	if (UserData == nullptr)
	{
		// Use the platform id here to be resilient in the face of controller swapping and similar situations
		FPlatformUserId PlatformId = GetControllerId();

		auto Identity = Online::GetIdentityInterface();
		if (Identity.IsValid() && GetPreferredUniqueNetId().IsValid())
		{
			PlatformId = Identity->GetPlatformUserIdFromUniqueNetId(*GetPreferredUniqueNetId());
		}

		UserData = UUserData::LoadUserData(GetNickname(), PlatformId);
	}
}

void USnakeLocalPlayer::SetControllerId(int32 NewControllerId)
{
	ULocalPlayer::SetControllerId(NewControllerId);

	// if we changed controller id, then we need to load the appropriate User Data
	if (UserData && (GetControllerId() != UserData->GetUserIndex() || GetNickname() != UserData->GetName()))
	{
		UserData->SaveIfDirty();
		UserData = nullptr;
	}

	if (!UserData)
	{
		LoadUserData();
	}
}

FString USnakeLocalPlayer::GetNickname() const
{
	FString UserNickname = Super::GetNickname();

	if (UserNickname.Len() > MAX_PLAYER_NAME_LENGTH)
	{
		UserNickname = UserNickname.Left(MAX_PLAYER_NAME_LENGTH) + "...";
	}

	bool bReplace = (UserNickname.Len() == 0);

	// check for dupes and prevent reentry
	static bool bReEntry = false;
	if (!bReEntry)
	{
		bReEntry = true;
		USnakeInstance* Instance = GetWorld() ? Cast<USnakeInstance>(GetWorld()->GetGameInstance()) : nullptr;
		if (Instance)
		{
			//Check all the names to find any that are the same as this one
			const TArray<ULocalPlayer*>& LocalPlayers = Instance->GetLocalPlayers();
			for (int32 c = 0; c < LocalPlayers.Num(); c++)
			{
				const ULocalPlayer* LocalPlayer = LocalPlayers[c];
				if (this == LocalPlayer)
				{
					break;
				}

				if (UserNickname == LocalPlayer->GetNickname())
				{
					bReplace = true;
					break;
				}
			}
		}
		bReEntry = false;
	}

	if (bReplace)
	{
		UserNickname = FString::Printf(TEXT("Player%i"), GetControllerId() + 1);
	}

	return UserNickname;
}