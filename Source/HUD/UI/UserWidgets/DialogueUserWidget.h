// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "DialogueUserWidget.generated.h"

USTRUCT(BlueprintType)
struct FDialogueBoxContent
{
	GENERATED_BODY()

	/** Message to be displayed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
	FString DialogueMessage;

	/** The priority of this message. Lower values will have a lower priority*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
	int32 Priority;

	/** Length of time the message will be displayed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
	float Duration;

	/** Elasped time message has been displayed */
	UPROPERTY(BlueprintReadOnly, Category = "Content")
	float Progress;

};

UCLASS()
class SNAKE_PROJECT_API UDialogueUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Content that is being displayed. The highest priority content will be displayed while the rest are hidden. */
	UPROPERTY(BlueprintReadWrite, Category = "Properties")
	TArray<FDialogueBoxContent> DialogueContent;

	/** Content Tick. determine if this content has expired */
	UFUNCTION(BlueprintCallable, Category = "Control")
	void ContentTick(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Control")
	void AddDialogueContent(FDialogueBoxContent NewContent);

	/** Get the Current highest priority dialogue content */
	UFUNCTION(BlueprintPure, Category = "Control")
	FDialogueBoxContent GetDialogueContent();

	/** Get the content, with the Highest priority, dialogue message */
	UFUNCTION(BlueprintPure, Category = "Control")
	FString GetContentMessage();

};
