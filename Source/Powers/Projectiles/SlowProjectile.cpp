// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SlowProjectile.h"
#include "Powers/AttackHelper.h"

ASlowProjectile::ASlowProjectile(const FObjectInitializer& PCIP)
	: ABombProjectile(PCIP)
{
	ExplosionRadius = 300.0f;

	SlowEffect.SpeedModifier = 0.25f;
	SlowEffect.Duration = 2.0f;
}

//Override the Speed Modifier. This will override any modifications made during construction 
void ASlowProjectile::SetSlowPercent(float InSlowPercent)
{
	SlowEffect.SpeedModifier = InSlowPercent;
}

void ASlowProjectile::OnImpact(const FHitResult& Hit)
{
	SlowArea();
	Super::OnImpact(Hit);
}

void ASlowProjectile::SlowArea()
{
	FName SlowAreaMultiOverlap = FName(TEXT("SlowAreaMultiOverlap"));
	FCollisionQueryParams Params(SlowAreaMultiOverlap, false, this);
	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_Pawn);

	TArray<FOverlapResult> OverlapOut;
	GetWorld()->OverlapMultiByObjectType(OverlapOut, GetActorLocation(), FQuat::Identity, ObjParams, FCollisionShape::MakeSphere(ExplosionRadius), Params);

	for (int32 c = 0; c < OverlapOut.Num(); c++)
	{
		if (OverlapOut[c].GetActor() == ProjectileOwner)
		{
			continue;
		}

		ASnakeLink* Link = Cast<ASnakeLink>(OverlapOut[c].GetActor());
		if (Link)
		{
			Link->ApplySlow(SlowEffect);
		}

		ABot* Bot = Cast<ABot>(OverlapOut[c].GetActor());
		if (Bot)
		{
			Bot->ApplySlow(SlowEffect);
		}
	}
}

void ASlowProjectile::DealDamage(const FHitResult& Hit)
{
	ADefaultProjectile::DealDamage(Hit);
}