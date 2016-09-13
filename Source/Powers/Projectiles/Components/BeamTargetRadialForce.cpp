// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BeamTargetRadialForce.h"

ABeamTargetRadialForce::ABeamTargetRadialForce()
	: Super()
{
	OverlapComponent = CreateDefaultSubobject<USphereComponent>(TEXT("BeamTargetOverlapComponent"));
	OverlapComponent->SetSphereRadius(100.0f);
	//OverlapComponent->AttachTo(RootComponent);
	RootComponent = OverlapComponent;

	ForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("BeamTargetRadialForceComponent"));
	ForceComponent->ImpulseStrength = 20.0f;
	ForceComponent->ForceStrength = 20.0f;
	ForceComponent->DestructibleDamage = 100.0f;
	ForceComponent->Radius = 100.0f;
	ForceComponent->AttachTo(RootComponent);
}

void ABeamTargetRadialForce::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &ABeamTargetRadialForce::OnBeginOverlap);
}

void ABeamTargetRadialForce::ExecuteTick_Implementation()
{
	if (ForceComponent)
	{
		ForceComponent->FireImpulse();
	}
}

void ABeamTargetRadialForce::OnBeginOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnBeamTargetOverlap(Other, GetBeamOwner());
}