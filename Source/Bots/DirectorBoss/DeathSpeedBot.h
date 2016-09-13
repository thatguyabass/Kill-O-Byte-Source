// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/SpeedyBoss/SpeedyBot.h"
#include "DeathSpeedBot.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EDeathSpeedState
{
	Charging,
	Moving,
	Cleanup
};

UCLASS()
class SNAKE_PROJECT_API ADeathSpeedBot : public ASpeedyBot
{
	GENERATED_BODY()
	
public:
	ADeathSpeedBot();

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UBoxComponent* OverlapComponent;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnComponentBeginOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** This bot will not take any damage */
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:
	UFUNCTION(BlueprintPure, Category = "SpeedBot|State")
	EDeathSpeedState GetBotState();

	UFUNCTION(BlueprintCallable, Category = "SpeedBot|State")
	void SetBotState(EDeathSpeedState InBotState);

protected:
	EDeathSpeedState BotState;

};
