// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Powers/Projectiles/DefaultProjectile.h"
#include "TubeBossProjectile.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ATubeBossProjectile : public ADefaultProjectile
{
	GENERATED_BODY()
	
public:
	ATubeBossProjectile(const class FObjectInitializer& PCIP);

	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile|Components")
	USphereComponent* TubeOverlapComp;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void TubeComponentBeginOverlap(class AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	virtual void HideAndDestroy(const FHitResult& Hit = FHitResult()) override;

};
