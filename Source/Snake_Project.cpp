// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"

class FCustomModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
		//Hot Reload Fix
		//FSlateStyleRegistry::UnRegisterSlateStyle(FSnakeStyle::GetStyleSetName());
		//FSnakeStyle::Initialize();
	}

	virtual void ShutdownModule() override
	{
		//FSnakeStyle::Shutdown();
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FCustomModule, Snake_Project, "Kill-O-Byte");

//Projectile Log
DEFINE_LOG_CATEGORY(ProjectileLog);