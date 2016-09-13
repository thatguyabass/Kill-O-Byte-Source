// Fill out your copyright notice in the Description page of Project Settings.

/**
*
* This class Runs delegates to ensure the player has the correct Privileges to run an online game
* 
*/

#pragma once
#include "Online.h"

class FOnlineHelper : public TSharedFromThis < FOnlineHelper >
{
public:
	virtual ~FOnlineHelper();

	void Construct(TWeakObjectPtr<class USnakeInstance> _GameInstance, TWeakObjectPtr<ULocalPlayer> _PlayerOwner);

	/** Join Server */
	void OnJoinServer();

protected:
	/** Get the Current PC owner */
	ULocalPlayer* GetPlayerOwner() const;

	/** Weak Pointer to the Game Instance */
	TWeakObjectPtr<class USnakeInstance> GameInstance;

	/** Weak Pointer to the PC owner */
	TWeakObjectPtr<ULocalPlayer> PlayerOwner;

	/** Start the check for whether the owner of the menu has online privileges */
	void StartOnlinePrivilegeTask(const IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate& Delegate);

	/** Clean up Online Sessions */
	void CleanupOnlinePrivilegeTask();

	/** Delegate Function executed after checking privileges for hosting an online game */
	void OnUserCanPlayOnlineHost(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults);

	/** Delegate Function Executed after checking privileges for joining an online game */
	void OnUserCanPlayOnlineJoin(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, uint32 PrivilegeResults);

};