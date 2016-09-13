// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/PointOfInterest.h"
#include "GameFramework/Actor.h"
#include "POIGroup.generated.h"

//Collection of Points of Interest
USTRUCT(BlueprintType)
struct FPointOfInterestGroup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Group")
	TArray<class APointOfInterest*> Points;

	bool IsValid()
	{
		return Points.Num() > 0;
	}
};

UCLASS()
class SNAKE_PROJECT_API APOIGroup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APOIGroup();

	UPROPERTY(EditAnywhere, Category = "Group")
	FPointOfInterestGroup Group;

};
