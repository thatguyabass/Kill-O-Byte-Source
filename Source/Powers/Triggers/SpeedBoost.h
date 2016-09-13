// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"
#include "GameFramework/Actor.h"
#include "SpeedBoost.generated.h"

UCLASS()
class SNAKE_PROJECT_API ASpeedBoost : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpeedBoost();

	UPROPERTY(VisibleDefaultsOnly, Category = "SpeedBoost|Component")
	UBoxComponent* EnterComponent;

	UPROPERTY(EditAnywhere, Category = "SpeedBoost|Porperty")
	FSpeedPower SpeedBoost;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	virtual void OnBeginEnterOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	void ApplySpeedBoost(class ASnakeLinkHead* Head);

};
