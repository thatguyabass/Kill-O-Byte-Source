// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Powers/Projectiles/BombProjectile.h"
#include "SlowProjectile.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ASlowProjectile : public ABombProjectile
{
	GENERATED_BODY()
	
public:
	ASlowProjectile(const class FObjectInitializer& PCIP);

	/** Speed Effect is modified by the Data Table, Manual modification will be overridden during construction */
	UPROPERTY(EditAnywhere, Category = "Slow Properties")
	FSpeedPower SlowEffect;

	virtual void OnImpact(const FHitResult& Hit) override;

	virtual void SetSlowPercent(float InPercent) override;

private:
	/** Slow Pawns that are in the affected area */
	void SlowArea();

	virtual void DealDamage(const FHitResult& Hit) override;

};
