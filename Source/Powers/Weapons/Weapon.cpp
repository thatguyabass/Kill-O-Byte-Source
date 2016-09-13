// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "Weapon.h"
#include "SnakeCharacter/SnakeLink.h"
#include "Powers/AttackHelper.h"

const FString AWeapon::WeaponDataContext("WeaponDataTable");

// Sets default values
AWeapon::AWeapon()
{
	FireRate = 0.5f;
	FireDirection = EFireDirection::Forward;

	bCanFire = true;
	bHomingProjectiles = false;

	Damage = 1;
	Progress = 0;
	DestructionLifeSpan = 1.5f;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetTableData();
}

void AWeapon::SetWeaponOwner(AActor* InWeaponOwner, AActor* InProjectileOwner)
{
	if (WeaponOwner != InWeaponOwner)
	{
		WeaponOwner = InWeaponOwner;
		ProjectileOwner = InProjectileOwner ? InProjectileOwner : InWeaponOwner;
		SetOwner(InWeaponOwner);
	}
}

void AWeapon::Fire(FVector FireOffset)
{
	//Nothing here, override this method
}

void AWeapon::ServerFire_Implementation(const FVector& InLocation, const TArray<FRotator>& InRotations, int32 Index)
{
	//Nothing here, override this method
}

void AWeapon::ServerFireCustomMulti_Implementation(const FVector& InLocation, const TArray<FRotator>& InRotations, const TArray<TSubclassOf<ABaseProjectile>>& InProjectileClasses)
{
	//Nothing here, override thie method
}

void AWeapon::ServerFireCustom_Implementation(const FVector& InLocation, const TArray<FRotator>& InRotaitons, int32 Index, TSubclassOf<ABaseProjectile> InProjectileClass, float InFireRate, int32 InDamage)
{
	//Nothing here, override this method
}

void AWeapon::ResetCanFire()
{
	bCanFire = true;
	Progress = 0.0f;
}

FRotator AWeapon::GetInitialDirection()
{
	FRotator Direction;

	switch (FireDirection)
	{
	case EFireDirection::Forward: Direction = WeaponOwner->GetActorForwardVector().Rotation(); break;
	case EFireDirection::Back: Direction = (WeaponOwner->GetActorForwardVector() * -1).Rotation(); break;
	case EFireDirection::Right: Direction = WeaponOwner->GetActorRightVector().Rotation(); break;
	case EFireDirection::Left: Direction = (WeaponOwner->GetActorRightVector() * -1).Rotation();  break;
	}

	return Direction;
}

void AWeapon::FireWeapon(const FVector& InLocation, const FRotator& InRotation)
{
	if (bCanFire)
	{
		FTransform SpawnTransform(InRotation, InLocation);

		FVector Location = WeaponOwner->GetActorLocation();
		ABaseProjectile* Projectile = Cast<ABaseProjectile>(UGameplayStatics::BeginSpawningActorFromClass(this, ProjectileClass, SpawnTransform, false));

		Projectile->SetOwner(WeaponOwner);
		Projectile->ProjectileOwner = ProjectileOwner;
		Projectile->Instigator = Instigator;

		//Set the projectile damage
		Projectile->SetDamage(Damage);
		Projectile->SetExplosionRadius(Radius);
		Projectile->SetSlowPercent(SlowPercent);

		UGameplayStatics::FinishSpawningActor(Projectile, SpawnTransform);

		FAttackType Type = AttackHelper::FindAttackTypeInActor(ProjectileOwner);
		Projectile->SetAttackMode(Type);

		if (bHomingProjectiles)
		{
			Projectile->InitializeHoming(HomingTarget, HomingDistance);
		}

		Projectile->InitVelocity(InRotation.Vector());
	}
}

void AWeapon::FireWeapon(const FVector& InLocation, const TArray<FRotator>& InRotations, int32 Index)
{
	if (bCanFire && Index >= 0)
	{
		for (int32 c = 0; c < Index; c++)
		{
			FRotator Rot = InRotations[c];

			FireWeapon(InLocation, Rot);
		}

		bCanFire = false;
	}
}

bool AWeapon::CanFire()
{
	return bCanFire;
}

void AWeapon::OverrideCanFire(bool Override)
{
	ResetCanFire();
	bCanFire = Override;
}

void AWeapon::StartDestroy()
{
	SetLifeSpan(DestructionLifeSpan);
	bCanFire = false;
	if (OnProjectileFinishedFire.IsBound())
	{
		OnProjectileFinishedFire.Unbind();
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, bCanFire);
	DOREPLIFETIME(AWeapon, WeaponOwner);
	DOREPLIFETIME(AWeapon, ProjectileOwner);
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bCanFire)
	{
		Progress += DeltaTime;
		if (Progress > FireRate)
		{
			ResetCanFire();
		}
	}
}

bool AWeapon::ProjectileBeingFired()
{
	return false;
}

void AWeapon::SetHomingProjectile(AActor* Target, float Distance)
{
	if (Target == nullptr)
	{
		return;
	}

	bHomingProjectiles = true;
	HomingDistance = Distance;
	HomingTarget = Target->GetRootComponent();
}

void AWeapon::TerminateProjectile()
{
	//Override
}

void AWeapon::GetTableData()
{
	if (!WeaponDataTable)
	{		
		FString N = GetName();
		GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Red, "No Weapon Data Table Assigned to: " + N);
		return;
	}

	FWeaponStatDataRow* WeaponData = WeaponDataTable->FindRow<FWeaponStatDataRow>(WeaponRow, WeaponDataContext, true);

	if (WeaponData)
	{
		Damage = WeaponData->Damage;
		FireRate = WeaponData->FireRate;
		Radius = WeaponData->ExplosionRange;
		SlowPercent = WeaponData->SlowPercent;
	}
}

float AWeapon::GetFireRate() const
{
	return FireRate;
}

void AWeapon::SetFireRate(float InFireRate)
{
	FireRate = InFireRate;
}