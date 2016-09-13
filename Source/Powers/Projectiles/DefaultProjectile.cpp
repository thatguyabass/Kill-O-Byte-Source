// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "DefaultProjectile.h"
#include "SnakeCharacter/SnakeLink.h"
#include "Powers/AttackHelper.h"

ADefaultProjectile::ADefaultProjectile(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	Damage = 1;
	MoveComp->InitialSpeed = 600.0f;
}

void ADefaultProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADefaultProjectile::OnImpact(const FHitResult& Hit)
{
	if (!bPendingDestroy)
	{
		PlayAudio(OnHitAudioComponent);
		DealDamage(Hit);
		HideAndDestroy(Hit);
	}
}

void ADefaultProjectile::NotifyHit(class UPrimitiveComponent* MyComponent, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation,
	FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComponent, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	
	OnImpact(Hit);
}

void ADefaultProjectile::DealDamage(const FHitResult& Hit)
{
	if (Hit.GetActor() && Hit.GetActor() != ProjectileOwner)
	{
		FDamageEvent Event = AttackHelper::CreateDamageEvent(DamageType, AttackType);

		ASnakePlayerState* State = GetOwnerPlayerState();
		AController* Controller = GetOwnerController();
		Hit.GetActor()->TakeDamage(Damage, Event, Controller, State);
	}
}