// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "ParticleSystemComponentPlus.h"

//UParticleSystemComponentPlus::UParticleSystemComponentPlus(const FObjectInitializer* PCIP)
//	: Super(PCIP)
//{
//
//}

void UParticleSystemComponentPlus::ActivateParticleSystem(bool bFlagAsJustAttached)
{
	ActivateSystem(bFlagAsJustAttached);
}

void UParticleSystemComponentPlus::DeactivateParticleSystem()
{
	DeactivateSystem();
}