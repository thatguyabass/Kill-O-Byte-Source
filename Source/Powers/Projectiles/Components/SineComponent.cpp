// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SineComponent.h"
#include "Powers/Projectiles/BaseProjectile.h"

// Sets default values for this component's properties
USineComponent::USineComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;

	Elapsed = 0.0f;
	StartingElapsed = 0.0f;

	Oscillation = 0.5f;
	Offset = 10.0f;
}


// Called when the game starts
void USineComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	Projectile = Cast<ABaseProjectile>(GetOwner());

	Elapsed = StartingElapsed;

	StoredForward = GetOwner()->GetActorForwardVector();
	StoredRight = GetOwner()->GetActorRightVector();
}


// Called every frame
void USineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Projectile)
	{
		Elapsed += DeltaTime;

		float Sine = Elapsed / Oscillation;
		Sine = FMath::Sin(Sine);

		float DirectionOffset = Sine * Offset;

		FVector Direction = StoredForward + (StoredRight * DirectionOffset);

		Projectile->SetVelocity(Direction);
	}
}