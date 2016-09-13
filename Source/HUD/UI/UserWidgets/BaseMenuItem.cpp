// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BaseMenuItem.h"

UBaseMenuItem::UBaseMenuItem(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	bIsActive = false;
	bDoNotHighlight = false;
	bCanHighlight = true;
	bCanUseAsClient = true;
	ButtonIndex = 0;
	ButtonRow = 0;
}

FSlateBrush UBaseMenuItem::GetBackground()
{
	UTexture2D* Texture = nullptr;
	
	// Reset every frame 
	bCanHighlight = true;
	if (bDoNotHighlight)
	{
		bCanHighlight = false;
	}

	if (bIsActive && bCanHighlight)
	{
		Texture = ActiveTexture;
	}
	else
	{
		Texture = DefaultTexture;
	}

	if (Texture)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(Texture);
		float Width = Texture->GetSizeX();
		float Height = Texture->GetSizeY();
		Brush.ImageSize = FVector2D(Width, Height);
		return Brush;
	}

	return FSlateNoResource();
}

void UBaseMenuItem::OnMouseEnter_Event(bool TriggerEvent)
{
	if (TriggerEvent)
	{
		InternalMouseEnter();
	}
	bIsActive = true;
}

void UBaseMenuItem::OnMouseLeave_Event()
{
	bIsActive = false;
}