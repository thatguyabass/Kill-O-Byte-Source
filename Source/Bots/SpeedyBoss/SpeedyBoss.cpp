// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SpeedyBoss.h"
#include "Bots/POIGroup.h"
#include "Bots/Bot/Component/WeakPoint.h"
#include "Bots/SpeedyBoss/SpeedyBossAI.h"
#include "Powers/Weapons/Weapon.h"
#include "Powers/Weapons/Weapon_Multi.h"

ASpeedyBoss::ASpeedyBoss()
	:	Super()
{
	WeaponPlacement = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponPlacement"));
	WeaponPlacement->AttachTo(RootComponent);

	SecondaryWeaponPlacement = CreateDefaultSubobject<USceneComponent>(TEXT("SecondaryWeaponPlacement"));
	SecondaryWeaponPlacement->AttachTo(RootComponent);

	WeakPointPlacement = CreateDefaultSubobject<USceneComponent>(TEXT("WeakPointPlacement"));
	WeakPointPlacement->AttachTo(RootComponent);

	bSlowImmunity = false;
	MaxSlowDuration = 10.0f;

	OrbitRadius = 250.0f;
	OrbitDuration = 10.0f;

	RotationRate = 50.0f;
	RotationRateScale = 1.0f;

	DestroyTime = 7.5f;

	FireMode = EFireMode::Sequential;
}

void ASpeedyBoss::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Initialize the Points of Interest for this Bot
	if (BossPointOfInterestGroup.IsValid())
	{
		SetPointOfInterestGroup(BossPointOfInterestGroup);
	}
}

void ASpeedyBoss::SpawnWeapon()
{
	if (WeaponClasses.Num() > 0)
	{
		for (int32 c = 0; c < WeaponClasses.Num(); c++)
		{
			if (WeaponClasses[c])
			{
				AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClasses[c]);
				NewWeapon->SetWeaponOwner(this, this);
				Weapons.Add(NewWeapon);
			}
		}
	}

	if (SecondaryWeaponClass)
	{
		SecondaryWeapon = GetWorld()->SpawnActor<AWeapon>(SecondaryWeaponClass);
		SecondaryWeapon->SetWeaponOwner(this, this);
	}
}

void ASpeedyBoss::PreProjectileSpawn(int32 WeaponIndex)
{
	if (Weapons.IsValidIndex(WeaponIndex))
	{
		Cast<AWeapon_Multi>(Weapons[WeaponIndex])->Fire(WeaponPlacement);
	}
}

void ASpeedyBoss::OverrideWeaponFireRate(float InFireRate)
{
	for (int32 c = 0; c < Weapons.Num(); c++)
	{
		Weapons[c]->SetFireRate(InFireRate);
	}
}

void ASpeedyBoss::FireSecondaryWeapon()
{
	if (SecondaryWeapon && SecondaryWeapon->CanFire())
	{
		PlayFireWeaponSFX();
		Cast<AWeapon_Multi>(SecondaryWeapon)->Fire(SecondaryWeaponPlacement);
	}
}

void ASpeedyBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDead)
	{
		return;
	}

	ASpeedyBossAI* AI = Cast<ASpeedyBossAI>(Controller);
	if (AI)
	{
		AActor* Target = AI->GetTarget();
		if (Target)
		{
			FVector TargetLocation = Target->GetActorLocation();
			FVector MyLocation = GetActorLocation();

			FVector Direction = TargetLocation - MyLocation;
			Direction.Normalize();

			WeaponPlacement->SetWorldRotation(Direction.Rotation());
		}
	}

	if (WeakPointPlacement)
	{
		OrbitProgress += (DeltaTime * GetSlowModifier());
		if (OrbitProgress > OrbitDuration)
		{
			OrbitProgress = 0.0f;
			OrbitAmount = FRotator::ZeroRotator;
		}

		FRotator RotationDirection = FRotator(0.0f, 1.0f, 0.0f);
		OrbitAmount = RotationDirection * (360.0f * (OrbitProgress / OrbitDuration));

		FVector Direction = FVector::ForwardVector;
		FVector Offset = OrbitAmount.RotateVector(Direction * OrbitRadius);
		FVector Location = Offset + GetActorLocation();

		WeakPointPlacement->SetWorldLocationAndRotation(Location, OrbitAmount);
		FTransform Trans = WeakPointPlacement->GetComponentTransform();

		if (WeakPoints.Num() > 0)
		{
			WeakPoints[0]->SetActorLocationAndRotation(Location, OrbitAmount);
		}

		FVector Dir = WeakPointPlacement->GetComponentLocation() - GetActorLocation();
		SecondaryWeaponPlacement->SetWorldRotation(Dir.Rotation());
	}

	if (DestructibleComponent)
	{
		FRotator CompRotation = DestructibleComponent->GetRelativeTransform().GetRotation().Rotator();

		CompRotation.Yaw += (RotationRate * RotationRateScale) * DeltaTime;

		DestructibleComponent->SetRelativeRotation(CompRotation);
	}
}

float ASpeedyBoss::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bDead || Damage <= 0.0f)
	{
		return 0.0f;
	}

	//Do not take any damage if the encounter hasn't been started 
	ASpeedyBossAI* AI = Cast<ASpeedyBossAI>(Controller);
	if (AI && !AI->GetEncounterStarted())
	{
		return 0.0f;
	}

	AWeakPoint* WeakPoint = Cast<AWeakPoint>(DamageCauser);
	if (WeakPoint)
	{
		Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

		if (!bDead)
		{
			AI->DamageTaken(Damage);
		}
	}

	return 0.0f;
}

void ASpeedyBoss::ApplySlow(FSpeedPower Slow)
{
	if (CanApplySlow() && !GetSlowImmunity())
	{
		Super::ApplySlow(Slow);
	}
	else if(!GetSlowImmunity())
	{
		SlowModifiers.Reset();

		FSpeedPower TempPower;
		TempPower.bIsActive = true;
		TempPower.Duration = MaxSlowDuration;
		TempPower.SpeedModifier = MaxSlowModifier;

		SlowModifiers.Add(TempPower);
		ApplyMovementChange();

		SetSlowImmunity(true);
	}
}

bool ASpeedyBoss::GetSlowImmunity() const
{
	return bSlowImmunity;
}

void ASpeedyBoss::SetSlowImmunity(bool InImmunity)
{
	bSlowImmunity = InImmunity;

	if (!bSlowImmunity)
	{
		RemoveSlow();
	}
}

void ASpeedyBoss::RemoveSlow()
{
	if (SlowModifiers.Num() > 0)
	{
		SlowModifiers.Reset();

		ApplyMovementChange();
	}
}

void ASpeedyBoss::StartEncounter()
{
	ASpeedyBossAI* AI = Cast<ASpeedyBossAI>(Controller);
	if (AI)
	{
		AI->StartEncounter();
	}
}

void ASpeedyBoss::HideAndDestroy()
{
	bDead = true;

	for (int32 c = 0; c < Weapons.Num(); c++)
	{
		if (Weapons[c])
		{
			Weapons[c]->StartDestroy();
		}
	}

	for (int32 c = 0; c < WeakPoints.Num(); c++)
	{
		WeakPoints[c]->HideAndDestroy();
	}

	GetController() ? Cast<ABaseAIController>(GetController())->StopAI() : nullptr;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (DestructibleComponent && DestructibleComponent->GetDestructibleMesh())
	{
		DestructibleComponent->ApplyRadiusDamage(500, GetActorLocation(), 500, 10, true);
	}

	GetWorldTimerManager().SetTimer(DissolveDelay_TimerHandle, this, &ASpeedyBoss::StartDissolve, DissolveStartDelay, false);

	OnHideAndDestroy(DestroyTime);
}

void ASpeedyBoss::ApplyMovementChange()
{
	Super::ApplyMovementChange();

	RotationRateScale = GetSlowModifier();
}