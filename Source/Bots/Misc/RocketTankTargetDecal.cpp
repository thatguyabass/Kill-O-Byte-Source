// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "RocketTankTargetDecal.h"
#include "Powers/Projectiles/BaseProjectile.h"

ARocketTankTargetDecal::ARocketTankTargetDecal(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	SceneRootComponent = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("Scene Root Component"));
	RootComponent = SceneRootComponent;

	BoxComponent = PCIP.CreateDefaultSubobject<UBoxComponent>(this, TEXT("BoxComp"));
	BoxComponent->AttachTo(RootComponent);

	DecalComponent = PCIP.CreateDefaultSubobject<UDecalComponent>(this, TEXT("DecalComp"));
	DecalComponent->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	DecalComponent->AttachTo(RootComponent);

	bProjectileEntered = false;

	DestroyDistance = 50.0f;

	PrimaryActorTick.bCanEverTick = true;
}


void ARocketTankTargetDecal::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ARocketTankTargetDecal::BeginOverlap);
	
	if (!Projectile)
	{
		Hide();
	}
}

void ARocketTankTargetDecal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bProjectileEntered)
	{
		FVector MyLocation = GetActorLocation();
		FVector ProjectileLocation = Projectile->GetActorLocation();

		float Distance = (ProjectileLocation - MyLocation).Size();
		if (Distance < DestroyDistance)
		{
			FHitResult Hit(ForceInit);
			Projectile->OnImpact(Hit);
			Hide();
		}
	}
}

void ARocketTankTargetDecal::BeginOverlap(class AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABaseProjectile* Project = Cast<ABaseProjectile>(Other);
	if (Project && Projectile && Project == Projectile)
	{
		bProjectileEntered = true;
		//Hide();
	}
}

void ARocketTankTargetDecal::Hide()
{
	bIsActive = false;
	bProjectileEntered = false;
	Projectile = nullptr;
	SetActorHiddenInGame(true);
}

void ARocketTankTargetDecal::Show(ABaseProjectile* InProjectile)
{
	if (InProjectile)
	{
		Projectile = InProjectile;
		SetActorHiddenInGame(false);
		bIsActive = true;
	}
}

bool ARocketTankTargetDecal::IsActive() const
{
	return bIsActive;
}