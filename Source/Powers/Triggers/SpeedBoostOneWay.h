// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Powers/Triggers/SpeedBoost.h"
#include "SpeedBoostOneWay.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ASpeedBoostOneWay : public ASpeedBoost
{
	GENERATED_BODY()
	
public:
	ASpeedBoostOneWay();
	
	UPROPERTY(VisibleDefaultsOnly, Category = "SpeedBoost|Component")
	USceneComponent* RootComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "SpeedBoost|Component")
	UBoxComponent* ExitComponent;

	/** store the players that have entered from the wrong direction */
	TArray<class AActor*> BadPlayers;

	void PostInitializeComponents() override;

	virtual void OnBeginEnterOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	void OnBeginExitOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnEndExitOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
