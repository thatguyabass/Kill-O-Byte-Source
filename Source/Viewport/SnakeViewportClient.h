// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameViewportClient.h"
#include "SnakeViewportClient.generated.h"

// if problems with the loading screen appear. duplicate the module code here. 
//Till then this will remain incomplete.

struct FSnakeLoadingScreenBrush : public FSlateDynamicImageBrush, public FGCObject
{
	FSnakeLoadingScreenBrush(const FName InTextureName, const FVector2D& InImageSize)
		: FSlateDynamicImageBrush(InTextureName, InImageSize)
	{
		ResourceObject = LoadObject<UObject>(nullptr, *InTextureName.ToString());
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector)
	{
		if (ResourceObject)
		{
			Collector.AddReferencedObject(ResourceObject);
		}
	}
};

class SLoadingScreen : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLoadingScreen) {}
	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs);

private:
	EVisibility GetLoadingIndicatorVisibility() const
	{
		return EVisibility::Visible;
	}

	/** Loading screen image brush */
	TSharedPtr<FSnakeLoadingScreenBrush> LoadingScreenBrush;
};

UCLASS(Within = Engine, Transient, config = Engine)
class USnakeViewportClient : public UGameViewportClient
{
	GENERATED_BODY()

public:
	void ShowLoadingScreen();
	void HideLoadingScreen();

	virtual void Tick(float DeltaTime) override;

protected:
	/** Loading Screen Widget Reference */
	TSharedPtr<SLoadingScreen> LoadingScreenWidget;

};
