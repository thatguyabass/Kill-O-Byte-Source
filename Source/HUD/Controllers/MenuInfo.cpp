// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "MenuInfo.h"
#include "UserData/UserSettings.h"
#include "UserData/UserData.h"
#include "UserData/SnakeLocalPlayer.h"
#include "HUD/Controllers/LevelListDataAsset.h"

// Sets default values
AMenuInfo::AMenuInfo()
{
	GammaOptIndex = 0;
	GammaIncrement = 0.1f;
	DefaultGammaIndex = 10;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AMenuInfo::BeginPlay()
{
	Super::BeginPlay();

	UserSettings = CastChecked<UUserSettings>(GEngine->GetGameUserSettings());
	bFullScreenOpt = UserSettings->GetFullscreenMode();
	GraphicsQualityOpt = UserSettings->GetGraphicsQuality();

	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		GammaOpt = SavedData->GetGamma();
		GammaOptIndex = SavedData->GetGammaIndex();

		bUseControllerOpt = SavedData->GetInputSetting();
	}

	GatherResolutions();
}

void AMenuInfo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMenuInfo::GatherResolutions()
{
	Resolutions.Empty();

	FScreenResolutionArray Array;
	if (RHIGetAvailableResolutions(Array, true))
	{
		for (int32 c = 0; c < Array.Num(); c++)
		{
			//Remove Resolutions Smaller then 1280 X 720. Resolutions smaller then this will cause UI issues.
			if (Array[c].Width < 1280 || Array[c].Height < 720)
			{
				continue;
			}

			FResolution Res;
			Res.ScreenWidth = Array[c].Width;
			Res.ScreenHeight = Array[c].Height;
			Res.RefreshRate = Array[c].RefreshRate;
			Resolutions.Add(Res);
		}
	}
}

void AMenuInfo::SetResolution(int32 Index)
{
	if (!Resolutions.IsValidIndex(Index))
	{
		return;
	}

	StoredResolution = Resolutions[Index];
}

FResolution AMenuInfo::GetResolution(int32 Index)
{
	if (Resolutions.IsValidIndex(Index))
	{
		return Resolutions[Index];
	}

	return FResolution();
}

TArray<FResolution>& AMenuInfo::GetAllResolutions()
{
	if (Resolutions.Num() == 0)
	{
		GatherResolutions();
	}

	return Resolutions;
}

int32 AMenuInfo::GetFullscreenSettings() const
{
	return (int32)bFullScreenOpt;
}

void AMenuInfo::SetFullscreenSettings(int32 Type)
{
	bFullScreenOpt = TEnumAsByte<EWindowMode::Type>(Type);

	if (bFullScreenOpt == EWindowMode::Fullscreen)
	{
		int32 LastIndex = Resolutions.Find(Resolutions.Last());
		SetResolution(LastIndex);

		ApplyChanges();
	}
}

int32 AMenuInfo::GetGraphicsQuaility() const
{
	return GraphicsQualityOpt;
}

void AMenuInfo::SetGraphicsQuality(int32 GraphicsQuality)
{
	GraphicsQualityOpt = GraphicsQuality + 1;
}

int32 AMenuInfo::GetCurrentGammaIndex() const
{
	return GammaOptIndex;
}

void AMenuInfo::SetGammaIndex(int32 Index)
{
	GammaOptIndex = Index;
}

void AMenuInfo::AdjustGamma(int32 Direction)
{
	SetGammaIndex(GammaOptIndex + Direction);

	const float Mod = GammaIncrement * Direction;
	GammaOpt += Mod;
	GEngine->DisplayGamma = GammaOpt;
}

float AMenuInfo::GetCurrentGamma() const
{
	return GammaOpt;
}

void AMenuInfo::SetInputSettings(bool Value)
{
	bUseControllerOpt = Value;
}

bool AMenuInfo::GetInputSettings() const
{
	return bUseControllerOpt;
}

FMapItemData AMenuInfo::GetMapData(int32 Index, bool bArcade)
{
	ULevelListDataAsset* LevelListDataAsset = bArcade ? ArcadeLevelListDataAsset : StoryLevelListDataAsset;
	return LevelListDataAsset->GetMapData(Index);
}

FMapItemData AMenuInfo::GetCurrentMapData(bool bArcade)
{
	ULevelListDataAsset* LevelListDataAsset = bArcade ? ArcadeLevelListDataAsset : StoryLevelListDataAsset;
	return LevelListDataAsset->GetCurrentMapData();
}

F2DInt32 AMenuInfo::GetCurrentResolution()
{
	F2DInt32 Res = F2DInt32();

	if (!StoredResolution.IsValid())
	{
		FVector2D Size;
		GEngine->GameViewport->GetViewportSize(Size);
		StoredResolution = FResolution(Size);
	}

	Res.X = StoredResolution.ScreenWidth;
	Res.Y = StoredResolution.ScreenHeight;

	return Res;
}

UUserData* AMenuInfo::GetSavedData() const
{
	USnakeLocalPlayer* const LocalPlayer = Cast<USnakeLocalPlayer>(GetWorld()->GetFirstLocalPlayerFromController());
	if (LocalPlayer)
	{
		return LocalPlayer->GetUserData();
	}

	return nullptr;
}

void AMenuInfo::ApplyChanges()
{
	UserSettings->SetScreenResolution(StoredResolution.ToPoint());
	UserSettings->SetFullscreenMode(bFullScreenOpt);
	UserSettings->SetGraphicsQuality(GraphicsQualityOpt);
	UserSettings->ApplySettings(false);

	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		SavedData->SetGamma(GammaOpt);
		SavedData->SetGammaIndex(GammaOptIndex);
		SavedData->SetInputSettings(bUseControllerOpt);
		SavedData->SaveIfDirty();
	}
}

void AMenuInfo::RevertChanges()
{
	GammaOptIndex = DefaultGammaIndex;
	GammaOpt = 2.0f;
	GEngine->DisplayGamma = GammaOpt;
	
	UUserData* SavedData = GetSavedData();
	if (SavedData)
	{
		SavedData->SetGamma(GammaOpt);
		SavedData->SetGammaIndex(GammaOptIndex);
		SavedData->SaveIfDirty();
	}
}

void AMenuInfo::UpdateOptions()
{

}

void AMenuInfo::ResetGamma()
{
	GEngine->DisplayGamma = 2.2f;
}

void AMenuInfo::SetUseVSync(bool InFlag)
{
	UserSettings->SetVSyncEnabled(InFlag);
}

bool AMenuInfo::GetUseVSync()
{
	return UserSettings->IsVSyncEnabled();
}