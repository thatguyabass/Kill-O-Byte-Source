// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Snake_ProjectClasses.h"

#define ECC_SnakeTraceChannel ECollisionChannel::ECC_GameTraceChannel1
#define ECC_ProjectileChannel ECollisionChannel::ECC_GameTraceChannel2
#define ECC_PowerUpChannel ECollisionChannel::ECC_GameTraceChannel3
#define ECC_JumpPadChannel ECollisionChannel::ECC_GameTraceChannel4
#define ECC_PlayerProjectile ECollisionChannel::ECC_GameTraceChannel5

#define MAX_PLAYER_NAME_LENGTH 16

//Projectile Log
DECLARE_LOG_CATEGORY_EXTERN(ProjectileLog, Log, All);