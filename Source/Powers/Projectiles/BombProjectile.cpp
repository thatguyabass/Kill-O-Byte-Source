// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BombProjectile.h"
#include "SnakeCharacter/SnakeLink.h"
#include "Powers/AttackHelper.h"

ABombProjectile::ABombProjectile(const class FObjectInitializer& PCIP)
	: ADefaultProjectile(PCIP)
{
	ExplosionRadius = 300.0f;

	MoveComp->InitialSpeed = 400.0f;
	Damage = 2;
}

void ABombProjectile::SetExplosionRadius(float InRadius)
{
	ExplosionRadius = InRadius;
}

float ABombProjectile::GetExplosionRadius() const
{
	return ExplosionRadius;
}

void ABombProjectile::DealDamage(const FHitResult& Hit)
{
	ADefaultProjectile::DealDamage(Hit);

	FName TestMove = FName(TEXT("TestMove"));
	FCollisionQueryParams Params(TestMove, false, this);
	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjParams.AddObjectTypesToQuery(ECC_ProjectileChannel);

	TArray<FOverlapResult> OverlapOut;
	GetWorld()->OverlapMultiByObjectType(OverlapOut, GetActorLocation(), FQuat::Identity, ObjParams, FCollisionShape::MakeSphere(ExplosionRadius), Params);
		
	TArray<AActor*> OverlapActors;
	for (int32 c = 0; c < OverlapOut.Num(); c++)
	{
		if (OverlapOut[c].GetActor())
		{
			OverlapActors.Add(OverlapOut[c].GetActor());
		}
	}

	if (!Cast<ASnakeLink>(ProjectileOwner))
	{
		TArray<ASnakeLink*> PlayersHit;
		for (int32 c = 0; c < OverlapActors.Num(); c++)
		{
			ASnakeLink* Link = Cast<ASnakeLink>(OverlapActors[c]);
			if (Link)
			{
				bool bFound = false;
				ASnakeLink* Head = Link->GetHead();
				for (int32 v = 0; v < PlayersHit.Num(); v++)
				{
					if (Head == PlayersHit[v])
					{
						bFound = true;
						break;
					}
				}

				if (!bFound)
				{
					PlayersHit.Add(Link);
				}
				else
				{
					OverlapActors.RemoveAt(c);
					c--;
				}
			}
		}
	}

	for (int32 c = 0; c < OverlapActors.Num(); c++)
	{
		//If the overlap actor is the object we hit OR the projectile owner, Continue with the loop. 
		AActor* Overlap = OverlapActors[c];
		if (Overlap == Hit.GetActor() || Overlap == ProjectileOwner)
		{
			continue;
		}
			
		//check if the overlap actor is another projectile owned by the same owner. If true, continue with the loop.
		ABaseProjectile* Projectile = Cast<ABaseProjectile>(Overlap);
		if (Projectile && Projectile->ProjectileOwner == ProjectileOwner)
		{
			continue;
		}

		const float Distance = (Overlap->GetActorLocation() - GetActorLocation()).Size2D();
		const float Diff = FMath::Clamp<float>(Distance / ExplosionRadius, 0.0f, 1.0f);

		int32 LinearDamage = FMath::CeilToInt(Damage * Diff);
		FDamageEvent Event = AttackHelper::CreateDamageEvent(DamageType, AttackType);

		ASnakePlayerState* State = GetOwnerPlayerState();
		AController* Controller = GetOwnerController();
		Overlap->TakeDamage(LinearDamage, Event, Controller, State);
	}
}