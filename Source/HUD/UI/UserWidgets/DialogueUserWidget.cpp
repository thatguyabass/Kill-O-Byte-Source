// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "DialogueUserWidget.h"

void UDialogueUserWidget::ContentTick(float DeltaTime)
{
	for (int32 c = 0; c < DialogueContent.Num(); c++)
	{
		FDialogueBoxContent& C = DialogueContent[c];
		C.Progress += DeltaTime;
		if (C.Progress > C.Duration)
		{
			DialogueContent.RemoveAt(c);
			c--;
		}
	}
}

void UDialogueUserWidget::AddDialogueContent(FDialogueBoxContent NewContent)
{
	DialogueContent.Add(NewContent);
}

FDialogueBoxContent UDialogueUserWidget::GetDialogueContent()
{
	int32 Max = 0;
	int32 Index = -1;
	for (int32 c = 0; c < DialogueContent.Num(); c++)
	{
		if (DialogueContent[c].Priority >= Max)
		{
			Max = DialogueContent[c].Priority;
			Index = c;
		}
	}

	return Index > -1 ? DialogueContent[Index] : FDialogueBoxContent();
}

FString UDialogueUserWidget::GetContentMessage()
{
	return GetDialogueContent().DialogueMessage;
}