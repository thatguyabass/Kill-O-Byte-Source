// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "AttackTypeDataAsset.h"

UAttackTypeDataAsset::UAttackTypeDataAsset(const FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

FLinearColor UAttackTypeDataAsset::GetPrimaryColor() const
{
	return Data.ColorData.PrimaryColor;
}

FLinearColor UAttackTypeDataAsset::GetSecondaryColor() const
{
	return Data.ColorData.SecondaryColor;
}

EAttackMode UAttackTypeDataAsset::GetAttackMode() const
{
	return Data.AttackMode;
}