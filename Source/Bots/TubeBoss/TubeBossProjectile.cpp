// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "TubeBossProjectile.h"

ATubeBossProjectile::ATubeBossProjectile(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	TubeOverlapComp = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("TubeOverlapComponent"));
	TubeOverlapComp->AttachTo(RootComponent);
	TubeOverlapComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TubeOverlapComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	TubeOverlapComp->SetCollisionResponseToChannel(ECC_ProjectileChannel, ECR_Overlap);
	TubeOverlapComp->SetSphereRadius(150.0f);
}

void ATubeBossProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TubeOverlapComp->OnComponentBeginOverlap.AddDynamic(this, &ATubeBossProjectile::TubeComponentBeginOverlap);
}

void ATubeBossProjectile::TubeComponentBeginOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABaseProjectile* Projectile = Cast<ABaseProjectile>(Other);
	if (Projectile && Projectile != this)
	{
		EAttackMode Mode = Projectile->GetAttackType().AttackMode;
		if (Mode != GetAttackType().AttackMode)
		{
			ATubeBossProjectile* Boss = Cast<ATubeBossProjectile>(Projectile);
			if (!Boss)
			{
				//Hack to ensure Explosion doesn't kill this projectile
				Projectile->SetExplosionRadius(1.0f);
				//Hack to ensure the bounce is killed by this projectile 
				Projectile->SetBounceCount(10);

				FHitResult Hit(ForceInit);
				Projectile->OnImpact(Hit);
			}
		}
	}
}

void ATubeBossProjectile::HideAndDestroy(const FHitResult& Hit)
{
	Super::HideAndDestroy();

	TubeOverlapComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}