// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Online.h"

/**
* General Session Settings for a Networked Game
*/
class FCustomSessionSettings : public FOnlineSessionSettings
{
public:
	FCustomSessionSettings(bool bIsLAN = false, bool bIsPresence = false, int32 MaxPlayerCount = 4);
	virtual ~FCustomSessionSettings() {}
};

/** 
* General Search for a networked game
*/ 
class FCustomOnlineSearchSettings : public FOnlineSessionSearch
{
public:
	FCustomOnlineSearchSettings(bool bSearchingLAN = false, bool bSearchingPresence = false);
	virtual ~FCustomOnlineSearchSettings() {}
};

/**
* Search Settings from an empty dedicated server
*/
class FOnlineSearchSettingsEmptyDedicated : public FCustomOnlineSearchSettings
{
public:
	FOnlineSearchSettingsEmptyDedicated(bool bSearchingLan = false, bool bSearchingPresence = false);
	virtual ~FOnlineSearchSettingsEmptyDedicated() {}
};