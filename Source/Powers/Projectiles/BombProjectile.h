// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Powers/Projectiles/DefaultProjectile.h"
#include "BombProjectile.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ABombProjectile : public ADefaultProjectile
{
	GENERATED_BODY()
public:
	ABombProjectile(const class FObjectInitializer& PCIP);

	UPROPERTY(BlueprintReadWrite, Category = "Bomb|Property")
	float ExplosionRadius;

	virtual void SetExplosionRadius(float InRadius) override;
	virtual float GetExplosionRadius() const override;

protected:
	/** Damage all Snake Links caught in the Explosion */
	virtual void DealDamage(const FHitResult& Hit) override;

};
