// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "Trap.h"
#include "SnakeCharacter/SnakeLink.h"
#include "Powers/AttackHelper.h"

ATrap::ATrap(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	bReplicates = true;

	RootSceneComponent = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	SkeletalMeshComponent = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("SkeletalMesh"));
	SkeletalMeshComponent->AttachTo(RootComponent);

	SpikeCollisionComponent = PCIP.CreateDefaultSubobject<UBoxComponent>(this, TEXT("SpikeCollisionComponent"));
	SpikeCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SpikeCollisionComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	SpikeCollisionComponent->SetBoxExtent(FVector(250.0f));
	SpikeCollisionComponent->AttachTo(SkeletalMeshComponent, TEXT("joint1"));

	bExecute = false;
	Progress = 0.0f;
	Delay = 0.0f;

	PrimaryActorTick.bCanEverTick = true;
}

void ATrap::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SpikeCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ATrap::OnComponentOverlap);
}

void ATrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority)
	{
		if (bExecute)
		{
			Progress += DeltaTime;
			if (Progress >= Delay)
			{
				//MultiCastTrigger();
				Trigger();
				Progress = 0.0f;
				bExecute = false;
			}
		}
	}
}

void ATrap::TriggerTrap(float InDelay)
{
	if (InDelay > 0.0f)
	{
		Delay = InDelay;
		bExecute = true;
	}
	else
	{
		Trigger();
	}
}

void ATrap::NotifyHit(class UPrimitiveComponent* MyComponent, class AActor* Other, class UPrimitiveComponent* OtherComponent,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComponent, Other, OtherComponent, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	DamagePawn(Cast<APawn>(Other));
}

void ATrap::OnComponentOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DamagePawn(Cast<APawn>(Other));
}

void ATrap::DamagePawn(APawn* Pawn)
{
	if (Pawn)
	{
		FDamageEvent Event = AttackHelper::CreateDamageEvent(DamageType, FAttackType(EAttackMode::Attack05_White));

		ASnakeLink* Link = Cast<ASnakeLink>(Pawn);
		if (Link)
		{
			//Ensure the player can't survive if they are immortal
			Link->GetHeadCast()->OverrideInvulnerable();
		}

		Pawn->TakeDamage(50000, Event, nullptr, nullptr);
	}
}

void ATrap::MultiCastTrigger_Implementation()
{
	Trigger();
}

float ATrap::GetProgress() const
{
	return Progress;
}