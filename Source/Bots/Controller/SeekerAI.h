// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Controller/GruntAI.h"
#include "SeekerAI.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ASeekerAI : public AGruntAI
{
	GENERATED_BODY()
	
public:
	ASeekerAI(const class FObjectInitializer& PCIP);

	virtual void FindTarget() override;

	/** Distance required before final acceleration */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Seeker Mine")
	float AccelerationDistance;

	UPROPERTY(EditAnywhere, Category = "Seeker Mine")
	float AcceptableDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Seeker Mine")
	float SpeedModifier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Seeker Mine")
	float ExplodeDelay;

	UFUNCTION(BlueprintCallable, Category = "Seeker Mine")
	void Explode();

	UFUNCTION(BlueprintCallable, Category = "Seeker Mine")
	bool MoveToTarget();

	virtual FCollisionObjectQueryParams GetObjectQuery() override;

	virtual void BotDead(class ASnakePlayerState* Killer) override;

private:
	float BaseMovement;

	FTimerHandle Explode_TimerHandle;
	
};
