// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Snake_Project : ModuleRules
{
	public Snake_Project(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] 
        { 
            "Core", 
            "CoreUObject", 
            "Engine", 
			"OnlineSubsystem",
            "OnlineSubsystemUtils",
            "OnlineSubsystemSteam",
            "UMG",
            "AIModule",
            "RHI"
        });

        PrivateDependencyModuleNames.AddRange(new string[] 
        { 
            "InputCore", 
            "Slate", 
            "SlateCore"
        });

        PrivateDependencyModuleNames.AddRange( new string[] 
        {
				"InputCore",
				"Slate",
				"SlateCore",
				"SnakeLoadingScreen",
                "MoviePlayer"
		});

        //PrivateDependencyModuleNames.Add("OnlineSubsystem");
        //if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
        //{
        //    if (UEBuildConfiguration.bCompileSteamOSS == true)
        //    {
        //        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        //    }
        //}
	}
}
