// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "FloatyTextManager.h"


// Sets default values for this component's properties
AFloatyTextManager::AFloatyTextManager()
{
	Duration = 1.5f;
	Text = "";
	Location = FVector::ZeroVector;

	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;
}

void AFloatyTextManager::InitializeFloatyText(AController* PointReceiver, int32 Points, FVector WorldLocation)
{
	//if (PointReceiver)//make it so the Widget is in an array so they can all get cleaned properly
	//{
		Text = FString::FromInt(Points);
		Location = WorldLocation;

		//Fire off the Create Widget Event
		CreateFloatyText();

		//GetWorldTimerManager().SetTimer(Visible_TimerHandle, this, &AFloatyTextManager::DestroyFloatyText, Duration, false);
	//}
}

void AFloatyTextManager::DestroyFloatyText()
{
	//Fire the event to remove the text from the viewport
	RemoveFloatyText();
}

// Called every frame
void AFloatyTextManager::Tick(float DeltaTime)
{
	Super::Tick( DeltaTime);

	for (int32 c = 0; c < Widgets.Num(); c++)
	{
		Widgets[c].Progress += DeltaTime;
		if (Widgets[c].Progress > Duration)
		{
			if (Widgets[c].Widget->IsInViewport())
			{
				Widgets[c].Widget->RemoveFromViewport();
			}
			else
			{
				Widgets.RemoveAt(c);
				c--;
			}
		}
	}
}

FString AFloatyTextManager::GetText()
{
	return Text;
}

FVector AFloatyTextManager::GetWorldLocation()
{
	return Location;
}