// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "StasisField.h"
#include "Powers/Projectiles/BaseProjectile.h"
#include "SnakeCharacter/SnakeLinkHead.h"
#include "Utility/Utility.h"

// Sets default values
AStasisField::AStasisField()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	BoxComponent->SetBoxExtent(FVector(250.0f));
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComponent->SetCollisionResponseToChannel(ECC_ProjectileChannel, ECR_Overlap);
	RootComponent = BoxComponent;

	Slow.SpeedModifier = 0.5f;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AStasisField::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AStasisField::BeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AStasisField::EndOverlap);
}

void AStasisField::BeginOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Role == ROLE_Authority)
	{
		ABaseProjectile* Projectile = Cast<ABaseProjectile>(Other);
		if (Projectile)
		{
			LastModifier = Projectile->GetSpeedModifier();
			Projectile->SetSpeedModifier(Slow.SpeedModifier);
		}

		ASnakeLinkHead* Head = Cast<ASnakeLinkHead>(Other);
		if (Head)
		{
			Head->AddStasisSlowModifier(Slow);
		}
	}
}

void AStasisField::EndOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Role == ROLE_Authority)
	{
		ABaseProjectile* Projectile = Cast<ABaseProjectile>(Other);
		if (Projectile)
		{
			Projectile->SetSpeedModifier(LastModifier);
		}

		ASnakeLinkHead* Head = Cast<ASnakeLinkHead>(Other);
		if (Head)
		{
			Head->RemoveStasisSlowModifier();
		}
	}
}