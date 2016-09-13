// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "EndlessBotSpawner.h"

AEndlessBotSpawner::AEndlessBotSpawner()
	: Super()
{

}

void AEndlessBotSpawner::FinishSpawning()
{
	bFinished = true;
}

bool AEndlessBotSpawner::CanSpawn()
{
	if (AliveBotCount >= MaxAliveBotCount || bFinished)
	{
		return false;
	}

	return true;
}

void AEndlessBotSpawner::MarkFinished()
{
	//
}