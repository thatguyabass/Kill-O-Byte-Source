// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "HUDButton.h"

UHUDButton::UHUDButton(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	TextureColor = FLinearColor::White;
	TextureColor.A = 1.0f;

	ButtonSize = FVector2D(100, 100);
	PositionAlignment = EAlignment::Center;
	OffsetAlignment = EAlignment::Center;

	TextScale = 1.0f;
	TextColor = FLinearColor::White;
	TextColor.A = 1.0f;
	TextAlignment = EAlignment::Center;
}

void UHUDButton::ExecuteCallback()
{
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Black, ButtonName.ToString());
}


void UHUDButton::GetPositionAlignment(const FVector2D& ScreenSize, FVector2D& Out, const TEnumAsByte<EAlignment> Alignment)
{
	switch (Alignment)
	{
	case EAlignment::TopLeft:		Out = FVector2D(0, 0);								break;
	case EAlignment::TopRight:		Out = FVector2D(ScreenSize.X, 0);					break;
	case EAlignment::Center:		Out = FVector2D(ScreenSize.X / 2, ScreenSize.Y / 2); break;
	case EAlignment::BottomLeft:	Out = FVector2D(0, ScreenSize.Y);					break;
	case EAlignment::BottomRight:	Out = ScreenSize;									break;
	}
}

void UHUDButton::GetOffsetAlignment(FVector2D& Out, const FVector2D& Size, const TEnumAsByte<EAlignment> Alignment)
{
	switch (Alignment)
	{
	case EAlignment::TopLeft:		Out = Size * -1;							break;
	case EAlignment::TopRight:		Out = FVector2D(0, Size.Y * -1);			break;
	case EAlignment::Center:		Out = FVector2D(Size.X / 2, Size.Y / 2) * -1; break;
	case EAlignment::BottomLeft:	Out = FVector2D(Size.X * -1, 0);			break;
	case EAlignment::BottomRight:	Out = FVector2D(0, 0);						break;
	}
}

void UHUDButton::DrawButton(AHUD* HUD, UFont* Font, const FVector2D& ScreenSize)
{
	FVector2D Position;
	FVector2D Offset;
	FVector2D FinalPosition;

	GetPositionAlignment(ScreenSize, Position, PositionAlignment);
	GetOffsetAlignment(Offset, ButtonSize, OffsetAlignment);
	FinalPosition = Position + Offset + ButtonPosition;

	HUD->DrawTexture(Texture, FinalPosition.X, FinalPosition.Y, ButtonSize.X, ButtonSize.Y, 0, 0, 1, 1, TextureColor);
	HUD->AddHitBox(FinalPosition, ButtonSize, ButtonName, true, 0);

	if (Text != "")
	{
		FVector2D TextSize;

		HUD->GetTextSize(Text, TextSize.X, TextSize.Y, Font, TextScale);
		GetOffsetAlignment(Offset, TextSize, TextAlignment);//Subsitute the Button Alignment for the Text Alignment
		FinalPosition = Position + Offset + ButtonPosition;

		HUD->DrawText(Text, TextColor, FinalPosition.X, FinalPosition.Y, Font, TextScale);
	}
}

void UHUDButton::CheckButton()
{
	if (ButtonName == "None")
	{
		FString Name = ButtonName.ToString();
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, Name + " has no Name. This will prevent cause problems when using the button");
	}
}
