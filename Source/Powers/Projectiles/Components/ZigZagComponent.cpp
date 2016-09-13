// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "ZigZagComponent.h"
#include "Powers/Projectiles/BaseProjectile.h"

// Sets default values for this component's properties
UZigZagComponent::UZigZagComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UZigZagComponent::InitializeComponent()
{
	Super::InitializeComponent();

	Projectile = Cast<ABaseProjectile>(GetOwner());
	if (Projectile)
	{
		GetWorld()->GetTimerManager().SetTimer(ZigZag_TimerHandle, this, &UZigZagComponent::InitialDirection, 0.05f, false);
	}
}

void UZigZagComponent::InitialDirection()
{
	float Dur = Duration;
	if (!bFirst)
	{
		Dur /= 2;
		bFirst = true;
	}

	FVector Right = Projectile->GetActorRightVector();
	FVector Forward = Projectile->GetActorForwardVector();
	Right *= DirectionOffset;

	FVector Dir = Right + Forward;
	DirectionOffset *= -1;

	Dir.Normalize();
	Projectile->SetVelocity(Dir);

	if (Dur > 0.0f)
	{
		//Reset to continue the Zig Zag
		GetWorld()->GetTimerManager().SetTimer(ZigZag_TimerHandle, this, &UZigZagComponent::ChangeDirection, Dur, false);
	}
}

void UZigZagComponent::ChangeDirection()
{
	FVector D = Projectile->GetDirection();

	FVector Right = Projectile->GetActorRightVector();
	Right *= DirectionOffset;
	DirectionOffset *= -1;

	Right.Normalize();
	Projectile->SetVelocity(Right);

	if (Duration > 0.0f)
	{
		//Reset to continue the Zig Zag
		GetWorld()->GetTimerManager().SetTimer(ZigZag_TimerHandle, this, &UZigZagComponent::ChangeDirection, Duration, false);
	}
}