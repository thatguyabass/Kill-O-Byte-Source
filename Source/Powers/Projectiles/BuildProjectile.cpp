// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BuildProjectile.h"
#include "SnakeCharacter/SnakeLink.h"
#include "Powers/Shield/Repulsion.h"
#include "Powers/Triggers/StasisField.h"

ABuildProjectile::ABuildProjectile(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	MoveComp->InitialSpeed = 500.0f;
	
	Slow.SpeedModifier = 0.05f;
	Slow.Duration = 5.0f;
	Slow.bIsActive = false;
}

void ABuildProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABuildProjectile::OnBeginOverlap);
}

void ABuildProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ABuildProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABuildProjectile::NotifyHit(class UPrimitiveComponent* MyComponent, class AActor* Other, class UPrimitiveComponent* OtherComponent,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComponent, Other, OtherComponent, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (Role == ROLE_Authority)
	{
		OnImpact(Hit);
	}
}


/** Overlap is required because once the actor is attached to another, it no longer receives Hit events. Overlap allows us to execute attached collision */
void ABuildProjectile::OnBeginOverlap(class AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Role == ROLE_Authority)
	{
		//Self Check, due to the multi overlap
		if (Other == this)
		{
			return;
		}

		// Repulsion Check, Prevent the Projectile from Dying when entering the overlap
		ARepulsion* Repel = Cast<ARepulsion>(Other);
		if (Repel)
		{
			return;
		}

		/** Stasis Check */
		AStasisField* Stasis = Cast<AStasisField>(Other);
		if(Stasis)
		{
			return;
		}

		//Head Check, ignore the shooting snakes head.
		if (Other && ProjectileOwner && ProjectileOwner == Other)
		{
			return;
		}

		//Projectile Check, ignore other projectiles fired from the same snake
		ABaseProjectile* Projectile = Cast<ABaseProjectile>(Other);
		if (!AttachedToLink && Projectile && Projectile->ProjectileOwner && Projectile->ProjectileOwner == ProjectileOwner)
		{
			return;
		}

		// Attached Projectile
		ABuildProjectile* Build = Cast<ABuildProjectile>(Other);
		if (Build && AttachedToLink && Build->AttachedToLink)
		{
			if (AttachedToLink->GetHead() == Build->AttachedToLink->GetHead())
			{
				return;
			}
		}

		if (Build && (Build->AttachedToLink || AttachedToLink))
		{
			AttachedProjectiles.Add(Build);

			FHitResult Hit(ForceInit);

			if (Build->AttachedToLink)
			{
				Attach(Build->AttachedToLink);
				MultiCastAttachToRootComponent(Build);
			}
			else if (AttachedToLink)
			{
				Build->Attach(AttachedToLink);
				Build->MultiCastAttachToRootComponent(this);
			}
		}

		ASnakeLink* Link = Cast<ASnakeLink>(Other);
		if (Link && Link->IsHead())
		{
			ASnakePlayerState* State = GetOwnerPlayerState();
			ReduceLinkHealth(Link);
			DamageAttachedLink();//Damage the Links that this is attached too
			CleanProjectile();
		}

		if (!Build && !Link)
		{
			DamageAttachedLink();
			CleanProjectile();
		}
	}
}

void ABuildProjectile::OnImpact(const FHitResult& Hit)
{
	if (Role == ROLE_Authority)
	{
		//Check if we have hit a Snake Link
		ASnakeLink* Link = Cast<ASnakeLink>(Hit.GetActor());
		if (Link)
		{
			if (Link->IsHead())
			{
				ASnakePlayerState* State = GetOwnerPlayerState();
				ReduceLinkHealth(Link);
				DamageAttachedLink();//Damage the Links that this is attached too
				CleanProjectile();
			}
			else
			{
				//Link->AttachedProjectiles.Add(this);
				Attach(Link);
				MultiCastAttachToRootComponent(Hit.GetActor());
			}
		}
		else
		{
			ReduceLinkHealth(Hit.GetActor());

			//Destroy this object if the object isn't a link
			CleanProjectile();
		}
	}
}

void ABuildProjectile::DamageAttachedLink()
{
	if (Role == ROLE_Authority)
	{
		if (AttachedToLink)
		{
			ReduceLinkHealth(AttachedToLink);
			for (int32 c = 0; c < AttachedProjectiles.Num(); c++)
			{
				if (AttachedToLink->GetHealth() > 0)
				{
					AttachedProjectiles[c]->ReduceLinkHealth(AttachedToLink);
				}
			}
		}
	}
}

void ABuildProjectile::ReduceLinkHealth(AActor* ActorToDamage)
{
	FDamageEvent Event;
	Event.DamageTypeClass = DamageType;

	ASnakePlayerState* PlayerState = GetOwnerPlayerState();
	ActorToDamage->TakeDamage(Damage, Event, Instigator ? Instigator->Controller : nullptr, PlayerState);
}

void ABuildProjectile::CleanProjectile()
{
	if (!bPendingDestroy)
	{
		FRotator Rotation = FRotator();
		for (int32 c = 0; c < AttachedProjectiles.Num(); c++)
		{
			if (AttachedProjectiles[c])
			{
				AttachedProjectiles[c]->CleanProjectile();
				AttachedProjectiles[c]->HideAndDestroy();
			}
		}
		AttachedProjectiles.Empty();

		HideAndDestroy();
	}
}

void ABuildProjectile::Attach(ASnakeLink* Link)
{
	AttachedToLink = Link;
	AttachedToLink->ApplySlow(Slow);
}

bool ABuildProjectile::IsAttached()
{
	return (AttachedToLink ? true : false);
}

void ABuildProjectile::MultiCastAttachToRootComponent_Implementation(AActor* Other)
{
	if (Other)
	{
		//Turn off the Height Check
		Frequency = FLT_MAX;

		AttachRootComponentTo(Other->GetRootComponent(), NAME_None, EAttachLocation::KeepWorldPosition);
		CollisionComp->SetCollisionResponseToAllChannels(ECR_Overlap);
		FHitResult Hit(1.0f);
		MoveComp->StopSimulating(Hit);

		SetOwner(Other);
	}
}

void ABuildProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABuildProjectile, AttachedProjectiles);
	DOREPLIFETIME(ABuildProjectile, AttachedToLink);
}