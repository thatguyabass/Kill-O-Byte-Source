// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Powers/Projectiles/DefaultProjectile.h"
#include "BounceProjectile.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ABounceProjectile : public ADefaultProjectile
{
	GENERATED_BODY()
	
public:
	ABounceProjectile(const class FObjectInitializer& PCIP);

	UPROPERTY(EditAnywhere, Category = "Projectile|Bounce")
	int32 MaxBounceCount;

	/** Time after first bounce before self-destruction*/
	UPROPERTY(EditAnywhere, Category = "Projectile|Bounce")
	float BounceTimeout;

	virtual void NotifyHit(class UPrimitiveComponent* MyComponent, class AActor* Other, class UPrimitiveComponent* OtherComp,
		bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	
	virtual void OnImpact(const FHitResult& Hit) override;

	virtual void SetBounceCount(int32 InBounceCount) override;

protected:
	virtual void DealDamage(const FHitResult& Hit) override;

	/** Total Bounces */
	int32 BounceCount;

	FHitResult StoredHit;
	FTimerHandle TimerHandle;

	void DestroyProjectile();

};
