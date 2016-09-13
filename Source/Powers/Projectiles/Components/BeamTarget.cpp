// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BeamTarget.h"


// Sets default values
ABeamTarget::ABeamTarget()
{
	Speed = 2500.0f;
	bMove = true;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called every frame
void ABeamTarget::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (bMove)
	{
		const FVector Forward = GetActorForwardVector();
		const FVector MyLocation = GetActorLocation();

		const FVector NewLocation = MyLocation + ((Forward * Speed) * DeltaTime);

		SetActorLocation(NewLocation);
	}
}

void ABeamTarget::StartDestroy()
{
	SetLifeSpan(2.0f);
}

void ABeamTarget::SetBeamOwner(ALazor* InOwner)
{
	if (InOwner)
	{
		BeamOwner = InOwner;
	}
}

ALazor* ABeamTarget::GetBeamOwner() const
{
	return BeamOwner;
}

void ABeamTarget::ExecuteTick_Implementation()
{

}