// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "ProjectileHeightTrigger.h"
#include "Powers/Projectiles/BaseProjectile.h"

// Sets default values
AProjectileHeightTrigger::AProjectileHeightTrigger()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	BoxComponent->SetBoxExtent(FVector(250.0f));
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_ProjectileChannel, ECR_Overlap);
	RootComponent = BoxComponent;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AProjectileHeightTrigger::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectileHeightTrigger::BeginOverlapCheck);
}

void AProjectileHeightTrigger::BeginOverlapCheck(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	ABaseProjectile* Projectile = Cast<ABaseProjectile>(Other);
	if (Projectile)
	{
		FRotator Rot = Projectile->GetActorRotation();
		FRotator NoPitch(0.0f, Rot.Yaw, Rot.Roll);
		MultiCastRotateProjectile(Projectile, NoPitch.Vector());
		Projectile->TriggerHeightCheck();
	}
}

void AProjectileHeightTrigger::MultiCastRotateProjectile_Implementation(ABaseProjectile* Actor, FVector_NetQuantize InRotation)
{
	if (Actor)
	{
		Actor->SetActorRotation(InRotation.Rotation());
	}
}