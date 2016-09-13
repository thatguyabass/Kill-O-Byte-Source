// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "SnakeMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API USnakeMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:

	//void SetModifiers(float Speed );

	virtual float GetMaxSpeed() const override;
	
	
};
