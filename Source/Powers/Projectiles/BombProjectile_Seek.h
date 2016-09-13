// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Powers/Projectiles/BombProjectile.h"
#include "BombProjectile_Seek.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ABombProjectile_Seek : public ABombProjectile
{
	GENERATED_BODY()
	
public:
	ABombProjectile_Seek(const class FObjectInitializer& PCIP);

	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

	/** Event when this projectile collides with another actor */
	virtual void OnImpact(const FHitResult& Hit) override;

	/** Blueprint OnImpact Event. Vector is Hit location, if hit null, uses actor location */
	UFUNCTION(BlueprintImplementableEvent, Category = "Homing")
	void OnImpact_BlueprintEvent(FVector HitLocation);

	/** Initialize the Homing Variables. Set up the Tank Rocket Target */
	virtual void InitializeHoming(USceneComponent* InTargetComponent, float IdealDistance) override;

	virtual void HideAndDestroy(const FHitResult& Hit) override;

	void BeginOverlapCheck(class AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

protected:
	/** Target the movement component is moving towards. Reference is used for cleaning up once destroyed */
	class ARocketTankTargetDecal* TargetDecal;

	UPROPERTY(EditAnywhere, Category = "Homing")
	FVector2D FlightTimeRange;

	UPROPERTY()	
	float FlightTime;

	UPROPERTY()
	float FlightProgress;

	UPROPERTY(EditAnywhere, Category = "Homing")
	FVector2D HeightOffsetRange;

	float HeightOffset;

	UPROPERTY(EditAnywhere, Category = "Homing")
	float MidPoint;

	UPROPERTY()
	FVector InitialLocation;

	UPROPERTY()
	FVector TargetHeight;

};
