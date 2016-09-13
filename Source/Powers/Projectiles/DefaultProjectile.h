// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Powers/Projectiles/BaseProjectile.h"
#include "DefaultProjectile.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ADefaultProjectile : public ABaseProjectile
{
	GENERATED_BODY()
	
public:
	ADefaultProjectile(const class FObjectInitializer& PCIP);
	
	virtual void Tick(float DeltaTime);
	virtual void NotifyHit(class UPrimitiveComponent* MyComponent, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit);

	/** Execute when a collision has occured */
	virtual void OnImpact(const FHitResult& Hit) override;

protected:
	virtual void DealDamage(const FHitResult& Hit);

};
