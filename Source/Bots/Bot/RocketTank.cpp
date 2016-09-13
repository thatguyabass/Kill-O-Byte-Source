// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "RocketTank.h"
#include "Powers/Weapons/Weapon.h"
#include "Powers/Weapons/Weapon_Single.h"
#include "Bots/Controller/RocketTankAI.h"
#include "Bots/Misc/RocketTankTargetDecal.h"
#include "Bots/Bot/Component/WeakPoint.h"
#include "SnakeCharacter/SnakeLinkHead.h"

const int32 ARocketTank::WeaponCount = 3;
const int32 ARocketTank::TargetCount = 18;

const int32 ARocketTank::ThirdPhaseIndex = 2;

ARocketTank::ARocketTank()
	: ABaseBoss()
{
	GetCapsuleComponent()->SetCollisionProfileName("CharacterCapsule_Pawn");
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_SnakeTraceChannel, ECR_Ignore);

	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	HitBox->AttachTo(RootComponent);
	HitBox->SetBoxExtent(FVector(250.0f));
	HitBox->SetCollisionProfileName("BlockAll");
	
	HitBox02 = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox02"));
	HitBox02->AttachTo(RootComponent);
	HitBox02->SetBoxExtent(FVector(250.0f));
	HitBox02->SetCollisionProfileName("BlockAll");

	WeaponPlacement01 = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Placement 01"));
	WeaponPlacement02 = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Placement 02"));
	WeaponPlacement03 = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Placement 03"));

	WeaponPlacement01->AttachTo(RootComponent);
	WeaponPlacement02->AttachTo(RootComponent);
	WeaponPlacement03->AttachTo(RootComponent);

	WeaponPlacementComponents.Add(WeaponPlacement01);
	WeaponPlacementComponents.Add(WeaponPlacement02);
	WeaponPlacementComponents.Add(WeaponPlacement03);

	/** 30% */
	PhaseTransitionThreshold = 0.3f;
	BarrageSpread = 2000.0f;
}

void ARocketTank::SpawnWeapon()
{
	if (WeaponClasses.Num() > 0)
	{
		for (int32 c = 0; c < WeaponCount; c++)
		{
			AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClasses[0]);
			NewWeapon->SetWeaponOwner(this);

			Weapons.Add(NewWeapon);
		}
	}

	if (TargetClass)
	{
		for (int32 c = 0; c < TargetCount; c++)
		{
			ARocketTankTargetDecal* Target = GetWorld()->SpawnActor<ARocketTankTargetDecal>(TargetClass);
			WeaponTargets.Add(Target);
		}
	}
}

void ARocketTank::SimultaneousFire()
{
	ARocketTankAI* AI = Cast<ARocketTankAI>(Controller);
	if (AI && Weapons.IsValidIndex(0) && Weapons[0]->CanFire())
	{
		//Signal that a wave has fired. 
		AI->IncreaseBarrageWave();
	}

	Super::SimultaneousFire();
}

void ARocketTank::PreProjectileSpawn(int32 WeaponIndex)
{
	if (Weapons.IsValidIndex(WeaponIndex))
	{
		Weapons[WeaponIndex]->SetHomingProjectile(GetAvailableTarget(), IdealDistance);
		Cast<AWeapon_Single>(Weapons[WeaponIndex])->Fire(WeaponPlacementComponents[WeaponIndex], GetActorRotation());
	}
}

AActor* ARocketTank::GetAvailableTarget()
{
	for (int32 c = 0; c < WeaponTargets.Num(); c++)
	{
		if (!WeaponTargets[c]->IsActive())
		{
			ARocketTankAI* AI = Cast<ARocketTankAI>(Controller);
			if (AI)
			{
				AI->SetWeaponTarget(WeaponTargets[c]);
				return WeaponTargets[c];
			}
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "All Weapon Targets are Active, but fire was called. RocketTank.cpp Line 90");
	return WeaponTargets[0];
}

float ARocketTank::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//The Rocket Tank reamins in the scene after it has died.
	//Prevent the Actor from taking any additional damage 
	if (bDead)
	{
		return 0.0f;
	}

	//Reduce the Damage if the source wasn't from the weakpoint
	AWeakPoint* WeakPoint = Cast<AWeakPoint>(DamageCauser);
	if (!WeakPoint)
	{
		Damage = 1;
	}

	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	ARocketTankAI* AI = Cast<ARocketTankAI>(Controller);
	if (AI)
	{
		AI->DamageTaken(Damage);

		if (Health < BaseHealth * PhaseTransitionThreshold && AI->GetCurrentPhaseIndex() != ThirdPhaseIndex)
		{
			AI->SetPhaseIndex(ThirdPhaseIndex);
		}
	}

	return 0.0f;
}

void ARocketTank::HideAndDestroy()
{
	if (!bDead)
	{
		bDead = true;

		for (int32 c = 0; c < Weapons.Num(); c++)
		{
			Weapons[c]->StartDestroy();
		}

		for (int32 c = 0; c < WeakPoints.Num(); c++)
		{
			WeakPoints[c]->HideAndDestroy();
		}

		GetController() ? Cast<ABaseAIController>(GetController())->StopAI() : nullptr;

		HitBox->SetCollisionProfileName("NoCollision");
		HitBox02->SetCollisionProfileName("NoCollision");

		Super::HideAndDestroy();
	}
}

void ARocketTank::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	HitBox->OnComponentBeginOverlap.AddDynamic(this, &ARocketTank::BeginOverlapCheck);
	HitBox02->OnComponentBeginOverlap.AddDynamic(this, &ARocketTank::BeginOverlapCheck);
}

void ARocketTank::BeginOverlapCheck(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASnakeLinkHead* Head = Cast<ASnakeLinkHead>(Other);
	ARocketTankAI* AI = Cast<ARocketTankAI>(Controller);
	if (Head && AI && AI->GetState() == ERocketTankState::Charge)
	{
		FDamageEvent Event;
		Head->OverrideInvulnerable();
		Head->TakeDamage(Head->GetHealth(), Event, nullptr, this);
	}
}

void ARocketTank::StopActionAbilities()
{
	ARocketTankAI* AI = Cast<ARocketTankAI>(Controller);
	if (AI)
	{
		AI->StopActionAbilities();
	}
}

void ARocketTank::StartActionAbilities()
{
	ARocketTankAI* AI = Cast<ARocketTankAI>(Controller);
	if (AI)
	{
		AI->StartActionAbilities();
	}
}

void ARocketTank::StartEncounter()
{
	ARocketTankAI* AI = Cast<ARocketTankAI>(Controller);
	if (AI)
	{
		AI->StartEncounter();
	}
}

void ARocketTank::StartCharge()
{
	HitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	HitBox02->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ARocketTank::FinishCharge()
{
	HitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	HitBox02->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}