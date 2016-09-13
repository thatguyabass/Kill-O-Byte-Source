// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "TubeTurret.h"
#include "Powers/Weapons/Weapon.h"
#include "Utility/AttackTypeDataAsset.h"
#include "Bots/Bot/Component/WeakPoint.h"

const int32 ATubeTurret::MeshPhase01 = 0;
const int32 ATubeTurret::MeshPhase02 = 1;
const int32 ATubeTurret::MeshPhase03 = 2;

ATubeTurret::ATubeTurret()
	: Super()
{
	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TubeTurretHitBox"));
	HitBox->AttachTo(RootComponent);
	HitBox->SetBoxExtent(FVector(50.0f));
	HitBox->SetCollisionProfileName("BlockAll");

	MeshChangeSFXComponent = CreateDefaultSubobject<UAudioComponent>("TubeTurretMeshChangeAudioComponent");
	MeshChangeSFXComponent->bAutoActivate = false;
	MeshChangeSFXComponent->bAlwaysPlay = false;
	MeshChangeSFXComponent->AttachTo(RootComponent);

	WeakPointPlacement = CreateDefaultSubobject<USceneComponent>(TEXT("WeakPointPlacement"));
	WeakPointPlacement->AttachTo(RootComponent);

	FireOffsetPhase02.Add(FVector::ZeroVector);
	FireOffsetPhase02.Add(FVector::ZeroVector);

	TurretState = ETubeTurretState::Initialize;
	bDead = false;

	DestroyComponentDelay = 6.0f;
}

void ATubeTurret::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SetState(ETubeTurretState::Phase01);
}

void ATubeTurret::SpawnWeapon()
{
	if (WeaponClass)
	{
		FActorSpawnParameters Param;
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, Param);
		if (Weapon)
		{
			Weapon->SetWeaponOwner(this);
		}
		else
		{
			GetWorldTimerManager().SetTimer(SpawnDelay_TimerHandle, this, &ATubeTurret::SpawnWeapon, 0.1f, false);
		}
	}
}

void ATubeTurret::FireWeapon()
{
	if (TurretState == ETubeTurretState::Phase01)
	{
		Weapon->OverrideCanFire(true);

		Super::FireWeapon();
	}
	else if (TurretState == ETubeTurretState::Phase02)
	{
		if (WeaponFireSFXPhase02)
		{
			UGameplayStatics::PlaySoundAtLocation(this, WeaponFireSFXPhase02, GetActorLocation());
		}

		for (int32 c = 0; c < WeaponsPhase2.Num(); c++)
		{
			AWeapon* W = WeaponsPhase2[c];
			if (W)
			{
				W->OverrideCanFire(true);
				W->Fire(FireOffsetPhase02[c]);
			}
		}
	}
}

float ATubeTurret::TakeDamage(float Damage, FDamageEvent const & DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Damage <= 0 || TurretState != ETubeTurretState::Phase02)
	{
		return 0.0f;
	}

	//Only reduce health if it was sent via the weakpoint 
	AWeakPoint* Point = Cast<AWeakPoint>(DamageCauser);
	if (Point)
	{
		if (!bDead)
		{
			ReduceHealth(Damage, DamageCauser);
			BotOwner->TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
		}
	}
	return 0.0f;
}

void ATubeTurret::SetHealth(int32 InHealth)
{
	MaxHealth = InHealth;
	Health = InHealth;
}

void ATubeTurret::ReduceHealth(int32 InDamage, AActor* DamageCauser)
{
	if (bDead)
	{
		return;
	}

	Health -= InDamage;
	if (Health <= 0)
	{
		Dead();
	}
}

ETubeTurretState ATubeTurret::GetState() const
{
	return TurretState;
}

void ATubeTurret::SetState(ETubeTurretState State)
{
	//Ensure the new state is higher then the current 
	if ((uint8)TurretState < (uint8)State)
	{
		TurretState = State;
		StateChanged(TurretState);

		HandleNewState();
	}
}

void ATubeTurret::HandleNewState()
{
	switch (TurretState)
	{
	case ETubeTurretState::Phase01: SwapStaticMesh(MeshPhase01); break;
	case ETubeTurretState::Phase02: BeginPhase2(); break;
	case ETubeTurretState::Dead: BeginDeadState(); break;
	}
}

void ATubeTurret::Dead()
{
	bDead = true;

	SetState(ETubeTurretState::Dead);
}

void ATubeTurret::BeginPhase2()
{
	if (Weapon)
	{
		Weapon->StartDestroy();
	}

	MeshChangeSFXComponent->Play();

	SpawnWeakPoint();
	SpawnWeaponsPhase2();
	SwapStaticMesh(MeshPhase02);
}

void ATubeTurret::SpawnWeaponsPhase2()
{
	if (WeaponClass)
	{
		FActorSpawnParameters Param;
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		for (int32 c = 0; c < 2; c++)
		{
			AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, Param);
			if (NewWeapon)
			{
				NewWeapon->SetWeaponOwner(this);
				WeaponsPhase2.Add(NewWeapon);
			}
			else
			{
				//Reset the Array and recall this method 
				WeaponsPhase2.Reset();

				GetWorldTimerManager().SetTimer(SpawnDelay_TimerHandle, this, &ATubeTurret::SpawnWeaponsPhase2, 0.1f, false);
				break;
			}

		}			

	}
}

void ATubeTurret::SpawnWeakPoint()
{
	if (WeakPointClass)
	{
		FVector Location = WeakPointPlacement->GetComponentLocation();
		FRotator Rotation = WeakPointPlacement->GetComponentRotation();

		WeakPoint = GetWorld()->SpawnActor<AWeakPoint>(WeakPointClass, Location, Rotation);
		if (WeakPoint)
		{
			WeakPoint->SetWeakPointOwner(this);
			WeakPoint->SetAttackTypeStruct(AttackType);
			WeakPoint->AttachRootComponentToActor(this, NAME_None, EAttachLocation::KeepWorldPosition);
		}
		else
		{
			GetWorldTimerManager().SetTimer(WeakPoint_TimerHandle, this, &ATubeTurret::SpawnWeakPoint, 0.1f, false);
		}
	}
}

void ATubeTurret::BeginDeadState()
{
	if (Weapon)
	{
		Weapon->StartDestroy();
	}

	for (int32 c = 0; c < WeaponsPhase2.Num(); c++)
	{
		if (WeaponsPhase2[c])
		{
			WeaponsPhase2[c]->StartDestroy();
		}
	}

	if (WeakPoint)
	{
		WeakPoint->Hide();
	}

	DeathSFXComponent->Play();

	HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	UDestructibleMesh* DMesh = DestructibleComponent->GetDestructibleMesh();
	if (!DMesh)
	{
		//Hide the actor and set the life span
		SetActorHiddenInGame(true);
	}
	else
	{
		if (MeshComponent->StaticMesh)
		{
			//Expose the Destructble Mesh and reenable its collision
			DestructibleComponent->SetHiddenInGame(false);
			DestructibleComponent->SetCollisionProfileName("Des");

			//Hide the Skeletal Mesh
			MeshComponent->SetHiddenInGame(true);
		}
		//Destroy the Destructible Component
		DestructibleComponent->ApplyRadiusDamage(500, GetActorLocation(), 500, 10, true);

		GetWorldTimerManager().SetTimer(DissolveDelay_TimerHandle, this, &ATubeTurret::StartDissolve, DissolveStartDelay, false);
		GetWorldTimerManager().SetTimer(DestroyComponent_TimerHandle, this, &ATubeTurret::DestroyDestructionComponent, DestroyComponentDelay, false);
	}
}

void ATubeTurret::DestroyDestructionComponent()
{
	DestructibleComponent->DestroyComponent();
}

void ATubeTurret::CleanDestroy()
{
	if (!bDead)
	{
		Dead();
	}

	if (WeakPoint)
	{
		WeakPoint->HideAndDestroy();
	}

	SetLifeSpan(DestroyTime);
}

void ATubeTurret::SwapStaticMesh(int32 Index)
{
	if (MeshPhases.IsValidIndex(Index))
	{
		MeshComponent->SetStaticMesh(MeshPhases[Index]);
		SetDMIColor();
	}

	if (MeshComponent->StaticMesh)
	{
		DestructibleComponent->SetHiddenInGame(true);
		DestructibleComponent->SetCollisionProfileName("NoCollision");
	}
}

void ATubeTurret::SetDMIColor()
{
	for (int32 c = 0; c < DMI.Num(); c++)
	{
		//MeshComponent->SetMaterial(c, DMI[c]);
		DMI[c]->SetVectorParameterValue(PrimaryColorName, AttackType.ColorData.PrimaryColor);
		DMI[c]->SetVectorParameterValue(SecondaryColorName, AttackType.ColorData.SecondaryColor);
	}
}

void ATubeTurret::SetAttackTypeDataAsset(UAttackTypeDataAsset* InType)
{
	if (InType)
	{
		AttackType = InType->Data;
		SetDMIColor();
	}
}