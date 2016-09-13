// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "StasisField.generated.h"

UCLASS()
class SNAKE_PROJECT_API AStasisField : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStasisField();

	UPROPERTY(VisibleDefaultsOnly, Category = "Trigger|Components")
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, Category = "Trigger|Property")
	struct FSpeedPower Slow;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void BeginOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void EndOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:

	float LastModifier;

};
