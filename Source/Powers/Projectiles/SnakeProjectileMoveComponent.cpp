// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SnakeProjectileMoveComponent.h"
#include "BaseProjectile.h"
#include "SnakeCharacter/SnakeLink.h"

void USnakeProjectileMoveComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
	if (GetOwner()->Role < ROLE_Authority)
	{
		return;
	}

	Super::HandleImpact(Hit, TimeSlice, MoveDelta);
}