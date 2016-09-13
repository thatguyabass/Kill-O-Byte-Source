// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "MapItemUserWidget.h"

UMapItemUserWidget::UMapItemUserWidget(const FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

FMapItemData& UMapItemUserWidget::GetData()
{
	return MapData;
}

void UMapItemUserWidget::SetMapData(FMapItemData Data)
{
	MapData = Data;
}

FString UMapItemUserWidget::GetSafeName()
{
	return MapData.SafeName;
}

FString UMapItemUserWidget::GetTrueName()
{
	return MapData.TrueName;
}

int32 UMapItemUserWidget::GetMinPlayers()
{
	return 0;//MapData.MinPlayers;
}

int32 UMapItemUserWidget::GetMaxPlayers()
{
	return 1;//MapData.MaxPlayers;
}

FSlateBrush UMapItemUserWidget::GetMapImage()
{
	if (MapData.MapImage)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(MapData.MapImage);
		float Width = MapData.MapImage->GetSizeX();
		float Height = MapData.MapImage->GetSizeY();
		Brush.ImageSize = FVector2D(Width, Height);
		return Brush;
	}

	return FSlateNoResource();
}