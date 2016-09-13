// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BounceProjectile.h"
#include "Powers/AttackHelper.h"

ABounceProjectile::ABounceProjectile(const FObjectInitializer& PCIP)
	: ADefaultProjectile(PCIP)
{
	MoveComp->bShouldBounce = true;
	
	MaxBounceCount = 3;

}

void ABounceProjectile::NotifyHit(class UPrimitiveComponent* MyComponent, class AActor* Other, class UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComponent, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
}

void ABounceProjectile::OnImpact(const FHitResult& Hit)
{
	BounceCount++;
	PlayAudio(OnHitAudioComponent);
	DealDamage(Hit);

	if (BounceCount > MaxBounceCount)
	{
		HideAndDestroy(Hit);
	}

	StoredHit = Hit;
	if (!TimerHandle.IsValid())
	{
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ABounceProjectile::DestroyProjectile, BounceTimeout, false);
	}
}

void ABounceProjectile::DealDamage(const FHitResult& Hit)
{
	if (Hit.GetActor())
	{
		FDamageEvent Event = AttackHelper::CreateDamageEvent(DamageType, AttackType);

		if (BounceCount <= 0)
		{
			++BounceCount;
		}

		if (Hit.GetActor() == ProjectileOwner)
		{
			return;
		}

		//Reduce the Damage for each bounce
		int32 DamageModified = FMath::CeilToInt(Damage / BounceCount);

		ASnakePlayerState* State = GetOwnerPlayerState();
		AController* Controller = GetOwnerController();
		Hit.GetActor()->TakeDamage(DamageModified, Event, Controller, State);
	}
}

void ABounceProjectile::DestroyProjectile()
{
	HideAndDestroy(StoredHit);
}

void ABounceProjectile::SetBounceCount(int32 InBounceCount)
{
	BounceCount = InBounceCount;
}