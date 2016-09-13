// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "CircleBossWall.h"
#include "Bots/CircleBoss/CircleBoss.h"
#include "Components/MoveToLocation.h"
#include "SnakeCharacter/SnakeLinkHead.h"

ACircleBossWall::ACircleBossWall(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	BoxComponent = PCIP.CreateDefaultSubobject<UBoxComponent>(this, TEXT("Box Component"));
	BoxComponent->SetCollisionProfileName("BlockAll");
	RootComponent = BoxComponent;

	OverlapComponent = PCIP.CreateDefaultSubobject<UBoxComponent>(this, TEXT("BoxOverlapComponent"));
	OverlapComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	OverlapComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	OverlapComponent->SetBoxExtent(FVector(250.0f));
	OverlapComponent->AttachTo(RootComponent);

	MeshComponent = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("CircleBossWallMeshComponent"));
	MeshComponent->AttachTo(RootComponent);

	MoveToComponent = PCIP.CreateDefaultSubobject<UMoveToLocation>(this, TEXT("WallMoveToLocationComp"));
}

void ACircleBossWall::SetWallOwner(ACircleBossAI* InOwner)
{
	if (InOwner)
	{
		WallOwner = InOwner;
		WallOwner->OnWallMovementStateChange.BindUObject(this, &ACircleBossWall::SetWallState);
	}
}

void ACircleBossWall::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &ACircleBossWall::OnBeginOverlap);
}

void ACircleBossWall::SetWallState(EWallMovementState InState)
{
	WallMovementState = InState;
}

void ACircleBossWall::NotifyHit(UPrimitiveComponent* MyComponent, AActor* Other, UPrimitiveComponent* OtherComponent, bool bSelfMoved, FVector HitLocation, 
	FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	AActor::NotifyHit(MyComponent, Other, OtherComponent, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (WallMovementState == EWallMovementState::Crush)
	{
		FHitResult Hit(ForceInit);
		OnBeginOverlap(Other, OtherComponent, 0, false, Hit);
	}
	//	ASnakeLinkHead* Link = Cast<ASnakeLinkHead>(Other);
	//	ABot* Bot = Cast<ABot>(Other);
	//	if (Link || Bot)
	//	{
	//		if (Link)
	//		{
	//			Link->OverrideInvulnerable();
	//		}

	//		FDamageEvent Event;
	//		Other->TakeDamage(250000, Event, nullptr, nullptr);
	//	}
	//}
}

void ACircleBossWall::OnBeginOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (WallMovementState == EWallMovementState::Crush)
	{
		ASnakeLinkHead* Link = Cast<ASnakeLinkHead>(Other);
		ABot* Bot = Cast<ABot>(Other);
		if (Link || Bot)
		{
			if (Link)
			{
				Link->OverrideInvulnerable();
			}

			FDamageEvent Event;
			Other->TakeDamage(250000, Event, nullptr, nullptr);
		}
	}
}

void ACircleBossWall::Hide()
{
	SetActorHiddenInGame(true);
}

void ACircleBossWall::Show()
{
	SetActorHiddenInGame(false);
}