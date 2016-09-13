// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "HUDButton.generated.h"

/**
 * 
 */
UENUM()
enum class EAlignment
{
	TopLeft,
	TopRight,
	Center,
	BottomLeft,
	BottomRight
};

UCLASS()
class SNAKE_PROJECT_API UHUDButton : public UActorComponent
{
	GENERATED_BODY()

public:

	UHUDButton(const class FObjectInitializer& PCIP);

	UPROPERTY(EditAnywhere, Category = ButtonConfig)
	UTexture* Texture;

	UPROPERTY(EditAnywhere, Category = ButtonConfig)
	FLinearColor TextureColor;

	UPROPERTY(EditAnywhere, Category = ButtonConfig)
	FVector2D ButtonSize;

	UPROPERTY(EditAnywhere, Category = ButtonConfig)
	FVector2D ButtonPosition;
	
	UPROPERTY(EditAnywhere, Category = ButtonConfig)
	TEnumAsByte<EAlignment> PositionAlignment;

	UPROPERTY(EditAnywhere, Category = ButtonConfig)
	TEnumAsByte<EAlignment> OffsetAlignment;

	UPROPERTY(EditAnywhere, Category = ButtonConfig)
	FName ButtonName;

	UPROPERTY(EditAnywhere, Category = ButtonConfig)
	FString Text;

	UPROPERTY(EditAnywhere, Category = ButtonConfig)
	float TextScale;

	UPROPERTY(EditAnywhere, Category = ButtonConfig)
	FLinearColor TextColor;

	UPROPERTY(EditAnywhere, Category = ButtonConfig)
	TEnumAsByte<EAlignment> TextAlignment;

	void ExecuteCallback();

	void DrawButton(AHUD* HUD, UFont* Font, const FVector2D& ScreenSize);
	void CheckButton();

private:
	void GetPositionAlignment(const FVector2D& ScreenSize, FVector2D& Out, const TEnumAsByte<EAlignment> Alignment);
	void GetOffsetAlignment(FVector2D& Out, const FVector2D& Size, const TEnumAsByte<EAlignment> Alignment);

};
