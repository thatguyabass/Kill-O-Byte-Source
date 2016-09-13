// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BombProjectile_Seek.h"
#include "Bots/Misc/RocketTankTargetDecal.h"

ABombProjectile_Seek::ABombProjectile_Seek(const FObjectInitializer& PCIP)
	: ABombProjectile(PCIP)
{
	HeightOffset = 500.0f;
	FlightTime = 5;

	FlightTimeRange = FVector2D(1.0f, 5.0f);
	HeightOffsetRange = FVector2D(150, 1000);

	MidPoint = 0.5f;
}

void ABombProjectile_Seek::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ABombProjectile_Seek::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);

	if (bHomingProjectile)
	{
		FVector Target = MoveComp->HomingTargetComponent->GetComponentLocation();

		FlightProgress += DeltaTime;
		float Alpha = FlightProgress / FlightTime;

		FVector AtoB = FMath::Lerp(InitialLocation, TargetHeight, Alpha);
		FVector BtoC = FMath::Lerp(TargetHeight, Target, Alpha);
		FVector Final = FMath::Lerp(AtoB, BtoC, Alpha);

		FVector Direction = BtoC - AtoB;
		Direction.Normalize();
		FRotator Rotation = Direction.Rotation();

		SetActorRotation(Rotation);
		SetActorLocation(Final, true);
	}
}

void ABombProjectile_Seek::OnImpact(const FHitResult& Hit)
{
	Super::OnImpact(Hit);

	FVector HitLocation = GetActorLocation();
	if (Hit.IsValidBlockingHit())
	{
		HitLocation = Hit.Location;
	}
	OnImpact_BlueprintEvent(HitLocation);
}

void ABombProjectile_Seek::InitializeHoming(USceneComponent* InTargetComponent, float IdealDistance)
{
	Super::InitializeHoming(InTargetComponent, IdealDistance);

	ARocketTankTargetDecal* HomingTarget = Cast<ARocketTankTargetDecal>(InTargetComponent->GetOwner());
	if (HomingTarget)
	{
		TargetDecal = HomingTarget;
		HomingTarget->Show(this);
	}

	bHomingProjectile = true;

	FVector Distance = InTargetComponent->GetComponentLocation() - GetActorLocation();
	float Per = FMath::Clamp(Distance.Size() / IdealDistance, 0.0f, 1.0f);

	FlightTime = FMath::Lerp(FlightTimeRange.X, FlightTimeRange.Y, Per);
	HeightOffset = FMath::Lerp(HeightOffsetRange.X, HeightOffsetRange.Y, Per);

	InitialLocation = GetActorLocation();
	float Point = Distance.Size() * MidPoint;
	TargetHeight = GetActorLocation() + (FVector::UpVector * HeightOffset) + (GetActorForwardVector() * Point);
}

void ABombProjectile_Seek::HideAndDestroy(const FHitResult& Hit)
{
	Super::HideAndDestroy(Hit);

	if(TargetDecal)
	{
		TargetDecal->Hide();
		TargetDecal = nullptr;
	}
}

void ABombProjectile_Seek::BeginOverlapCheck(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Other == this)
	{
		return;
	}

	ABaseProjectile* Projectile = Cast<ABaseProjectile>(Other);
	if (Projectile)
	{
		AddUniqueIgnoreActor(Other);
	}
}