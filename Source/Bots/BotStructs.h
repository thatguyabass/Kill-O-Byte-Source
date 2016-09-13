// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType)
enum class EBotType : uint8
{
	Normal = 0,
	Special = 1
};

UENUM(BlueprintType)
enum class EBotClass : uint8
{
	Grunt = 0,
	Drone = 1,
	Tank = 2,
	Shield = 3,
	Mortar = 4,
	Lazer = 5,
	Medium = 6,
	Seeker = 7,
	Boss = 8
};

UENUM(BlueprintType)
enum class EDirectorDecisionState : uint8
{
	PreIdle,		//Move to the correct location before transitioning to idle 
	Idle,			//Movement state
	Vulnerable,		//Charge State 
	PostVulnerable,	//Ability has been executed
	Transition,		//Transition between battle states 
	Final			//Final State
};

UENUM(BlueprintType)
enum class EDirectorBattleState : uint8
{
	Speed,
	Mortar,
	Lazer,
	Final
};