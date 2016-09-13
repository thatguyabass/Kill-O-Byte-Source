// Fill out your copyright notice in the Description page of Project Settings.

/**
* This class acts as a storage object for settings and other menu data
* 
* Changed data will be stored and received using UUserData
*/

#pragma once

#include "Utility/Utility.h"
#include "GameFramework/Actor.h"
#include "MenuInfo.generated.h"

USTRUCT(BlueprintType)
struct FResolution
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resolution")
	int32 ScreenWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resolution")
	int32 ScreenHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resolution")
	int32 RefreshRate;

	FIntPoint ToPoint()
	{
		return FIntPoint(ScreenWidth, ScreenHeight);
	}

	bool IsValid()
	{
		return (ScreenWidth != 0 && ScreenHeight != 0);
	}

	bool operator==(FResolution InRes) const
	{
		if (ScreenWidth != InRes.ScreenWidth || ScreenHeight != InRes.ScreenHeight || RefreshRate != InRes.RefreshRate)
		{
			return false;
		}

		return true;
	}

	FResolution()
	{
		ScreenWidth = 0;
		ScreenHeight = 0;
		RefreshRate = 0;
	}

	FResolution(FVector2D& InSize)
	{
		ScreenWidth = InSize.X;
		ScreenHeight = InSize.Y;
		RefreshRate = 0;
	}
};

UCLASS()
class SNAKE_PROJECT_API AMenuInfo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMenuInfo();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** Store the new Resolution. This will remain Dirty (unless no change has been made) until applied. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Resolution")
	void SetResolution(int32 Index);

	/** Get the Resolution at this index */
	UFUNCTION(BlueprintPure, Category = "Settings|Resolution")
	FResolution GetResolution(int32 Index);

	/** Find all supported resolutions for this graphics card */
	UFUNCTION(BlueprintPure, Category = "Settings|Resolution")
	TArray<FResolution>& GetAllResolutions();

	/** Get the Current Viewport Size */
	UFUNCTION(BlueprintPure, Category = "Settings|Resolution")
	F2DInt32 GetCurrentResolution();

	/** Set teh current screenmode type */
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetFullscreenSettings(int32 Type);

	/** Get the Current Screen Mode Type. */
	UFUNCTION(BlueprintPure, Category = "Settings|Graphics")
	int32 GetFullscreenSettings() const;

	/** Get the Current Graphics Level */
	UFUNCTION(BlueprintPure, Category = "Settings|Graphics")
	int32 GetGraphicsQuaility() const;

	/** Set the Graphics Quality. 1 = Low, 2 = Med, 3 = High, 4 = Epic!(What unreal calls Ultra) */
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetGraphicsQuality(int32 GraphicsQuality);

	/** Get the Current Gamma Index */
	UFUNCTION(BlueprintPure, Category = "Settings|Graphics")
	int32 GetCurrentGammaIndex() const;

	/** Set the Gamma Level Index. Index is used with teh Multi Menu Item. 5 is normal gamma, 1 Really low, 10 Really high */
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetGammaIndex(int32 Index);

	/** Get the Current Gamma Level */
	UFUNCTION(BlueprintPure, Category = "Settings|Graphics")
	float GetCurrentGamma() const;

	/** Modify the Current Gamma Levels. 
	*	@Param Direction if > 1 Gamma levels will increase, if < 0 Levels decrease 
	*/
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void AdjustGamma(int32 Direction);

	/** Set input. If true, a controller is the exepected input device. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Input")
	void SetInputSettings(bool Value);

	UFUNCTION(BlueprintPure, Category = "Settings|Input")
	bool GetInputSettings() const;

	/** Single Player Map information */
	UPROPERTY(EditAnywhere, Category = "Level List")
	class ULevelListDataAsset* StoryLevelListDataAsset;

	/** Arcade Map Information */
	UPROPERTY(EditAnywhere, Category = "Level List")
	class ULevelListDataAsset* ArcadeLevelListDataAsset;

	/** Get the map info at this index */
	UFUNCTION(BlueprintCallable, Category = "Map Selection")
	FMapItemData GetMapData(int32 Index, bool bArcade = false);

	/** Return the map info at the Current Index */
	UFUNCTION(BlueprintCallable, Category = "Map Selection")
	FMapItemData GetCurrentMapData(bool bArcade = false);

	/** Apply new Settings */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ApplyChanges();

	/** Revert all options to the Default Options */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void RevertChanges();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void UpdateOptions();

	/** Console Method to reset gamma incase something bad happens */
	UFUNCTION(Exec)
	void ResetGamma();

protected:
	/** Current User's Saved Settings */
	UPROPERTY()
	class UUserSettings* UserSettings;

	/** Graphics Quality Option */
	int32 GraphicsQualityOpt;

	/** Gamma Option */
	float GammaOpt;
	float GammaIncrement;
	int32 GammaOptIndex;
	int32 DefaultGammaIndex;

	/** Use a controller or Mouse and Keyboard. If true, controller is expected */
	bool bUseControllerOpt;

	/** Full Screen option */
	EWindowMode::Type bFullScreenOpt;

	UPROPERTY()
	TArray<FResolution> Resolutions;

	/** Collect all the Supported Resolutions */
	void GatherResolutions();

	UFUNCTION()
	class UUserData* GetSavedData() const;

private:
	FResolution StoredResolution;

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetUseVSync(bool InFlag);

	UFUNCTION(BlueprintPure, Category = "Settings")
	bool GetUseVSync();

};
