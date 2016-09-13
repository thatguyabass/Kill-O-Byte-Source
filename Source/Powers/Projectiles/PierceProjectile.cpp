// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "PierceProjectile.h"
#include "Powers/AttackHelper.h"

APierceProjectile::APierceProjectile(const FObjectInitializer& PCIP)
	: ADefaultProjectile(PCIP)
{
	PierceOverlapComponent = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("PierceOverlapComponent"));
	PierceOverlapComponent->AttachTo(RootComponent);
	PierceOverlapComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PierceOverlapComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	PierceOverlapComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PierceOverlapComponent->SetSphereRadius(75.0f);

	MaxPenetration = 4;
}

void APierceProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PierceOverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &APierceProjectile::PierceOverlapCheck);
}

void APierceProjectile::PierceOverlapCheck(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Other == ProjectileOwner || OtherComp->GetCollisionObjectType() == ECC_WorldStatic || bPendingDestroy)
	{
		return;
	}

	// Look for this actor in the OverlapedActors List
	if (FindInList(Other))
	{
		return;
	}

	PenetrationCount++;
	FHitResult Hit(ForceInit);
	Hit.Actor = Other;
	DealDamage(Hit);

	AddUniqueIgnoreActor(Other);

	FVector Scale = GetActorScale3D();
	Scale = Scale - (1 / FMath::CeilToFloat(MaxPenetration));
	SetActorScale3D(Scale);

	if (PenetrationCount > MaxPenetration)
	{
		PierceOverlapComponent->OnComponentBeginOverlap.RemoveDynamic(this, &APierceProjectile::PierceOverlapCheck);
		HideAndDestroy(Hit);
	}
}

void APierceProjectile::OnImpact(const FHitResult& Hit)
{
	ADefaultProjectile::OnImpact(Hit);
}

void APierceProjectile::DealDamage(const FHitResult& Hit)
{
	if (Hit.GetActor() && !bPendingDestroy)
	{
		FDamageEvent Event = AttackHelper::CreateDamageEvent(DamageType, AttackType);

		// Zero Check, Prevent divide by 0 
		if (PenetrationCount <= 0)
		{
			++PenetrationCount;
		}
		int32 ScaledDamage = FMath::CeilToInt(Damage / PenetrationCount);

		ASnakePlayerState* State = GetOwnerPlayerState();
		AController* Controller = GetOwnerController();
		Hit.GetActor()->TakeDamage(ScaledDamage, Event, Controller, State);
	}
}

bool APierceProjectile::FindInList(AActor* Other)
{
	if (Other == nullptr)
	{
		return false;
	}

	// Loop through the list and compare them
	for (int32 c = 0; c < OverlapedActors.Num(); c++)
	{
		if (OverlapedActors[c] == Other)
		{
			return true;
		}
	}

	//Add the other actor to the overlap list 
	OverlapedActors.Add(Other);

	return false;
}