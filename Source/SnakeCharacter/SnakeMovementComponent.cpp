// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SnakeMovementComponent.h"


float USnakeMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	//MaxSpeed *= Slow * Speed;

	return MaxSpeed;
}

