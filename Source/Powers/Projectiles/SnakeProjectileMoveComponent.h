// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/ProjectileMovementComponent.h"
#include "SnakeProjectileMoveComponent.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API USnakeProjectileMoveComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()

public:
	AActor* ProjectileOwner;

	/** Handle Impacts */
	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice = 0.f, const FVector& MoveDelta = FVector::ZeroVector) override;

};