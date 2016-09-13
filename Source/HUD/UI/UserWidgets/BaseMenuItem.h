// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "BaseMenuItem.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API UBaseMenuItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UBaseMenuItem(const class FObjectInitializer& PCIP);

	UPROPERTY(BlueprintReadWrite, Category = "Menu Item Property")
	bool bIsActive;

	/** Don't Highlight this menu item. Defaults False */
	UPROPERTY(BlueprintReadWrite, Category = "Menu Item Property")
	bool bDoNotHighlight;

	/** Can the client use this button if they are a client? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menu Item Property")
	bool bCanUseAsClient;

	/** Can this Widget Highlight? */
	bool bCanHighlight;

	/** Row index of this menu item. Rows run Vertically */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Item Property")
	int32 ButtonRow;

	/** Button Index. Index Run's Horizontally */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Item Property")
	int32 ButtonIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Item Property")
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Item Property")
	UTexture2D* DefaultTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Item Property")
	UTexture2D* ActiveTexture;

	UFUNCTION(BlueprintCallable, Category = "Widget|MenuItem")
	FSlateBrush GetBackground();

	/** Hack Event to tigger mouse event dispatcher in blueprint */
	UFUNCTION(BlueprintImplementableEvent, Category = "Widget|MenuItem")
	void InternalMouseEnter();

	/** Event Trigged when the mouse enters the widget */
	UFUNCTION(BlueprintCallable, Category = "Widget|MenuItem")
	void OnMouseEnter_Event(bool TriggerEvent = true);

	/** Event when the mouse Leaves this widget */
	UFUNCTION(BlueprintCallable, Category = "Widget|MenuItem")
	void OnMouseLeave_Event();

};
