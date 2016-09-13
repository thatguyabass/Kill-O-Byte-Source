// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "DamageType_AttackMode.h"

UDamageType_AttackMode::UDamageType_AttackMode(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	AttackMode = EAttackMode::Attack05_White;
}