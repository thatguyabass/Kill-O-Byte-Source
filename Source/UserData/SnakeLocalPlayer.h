// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UserData.h"
#include "Engine/LocalPlayer.h"
#include "SnakeLocalPlayer.generated.h"

/**
 * 
 */
UCLASS(config = Engine, transient)
class SNAKE_PROJECT_API USnakeLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()
	
public:
	USnakeLocalPlayer(const class FObjectInitializer& PCIP);

	virtual void SetControllerId(int32 ControllerId) override;

	virtual FString GetNickname() const;

	class UUserData* GetUserData() const;

	void ReloadUserData();

	/** Initialize the USaveGame Object */
	void LoadUserData();
	
private:
	/** Saved UserData*/
	UPROPERTY()
	class UUserData* UserData;

	/** @return OnlineSession Class to use for this player */
	//TSubclassOf<UOnlineSession> GetOnlineSessionClass() override;

};
