// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FloatyTextManager.generated.h"

USTRUCT(BlueprintType)
struct FFloatyTextWidget
{
	GENERATED_BODY()

	/** Widget to spawn at location */
	UPROPERTY(BlueprintReadWrite, Category = "Floaty Text Widget")
	UUserWidget* Widget;

	/** How long the widget has been visible */
	float Progress;

};

UCLASS()
class SNAKE_PROJECT_API AFloatyTextManager : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AFloatyTextManager();

	// Called every frame
	virtual void Tick( float DeltaTime) override;

	/** How long the Text exists in the world */
	UPROPERTY(EditAnywhere, Category = "Duration")
	float Duration;

	/** Initialize Floaty Text */
	void InitializeFloatyText(AController* PointReceiver, int32 Points, FVector WorldLocation);

	/** Destroy the Floaty Text */
	void DestroyFloatyText();

	/** Create the Text Widget Event */
	UFUNCTION(BlueprintImplementableEvent, Category = "Floaty Event")
	void CreateFloatyText();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floaty Event")
	void RemoveFloatyText();

	/** Store the Widget in order to remove it later */
	UPROPERTY(BlueprintReadWrite, Category = "Floaty Text")
	TArray<FFloatyTextWidget> Widgets;

	/** Get the Score */
	UFUNCTION(BlueprintPure, Category = "Floaty Text")
	FString GetText();

	UFUNCTION(BlueprintPure, Category = "Floaty Text")
	FVector GetWorldLocation();

private:
	FTimerHandle Visible_TimerHandle;

	/** Text the Widget will display */
	FString Text;
	
	FVector Location;

};
