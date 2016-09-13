// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SeekerBot.h"
#include "Bots/Controller/BaseAIController.h"
#include "SnakeCharacter/SnakePlayer.h"
#include "Utility/DataTables/StatData.h"

ASeekerBot::ASeekerBot()
	: ABot()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> WeaponDataTableObject(TEXT("/Game/Blueprints/Attributes/Stats_WeaponStats"));
	WeaponDataTable = WeaponDataTableObject.Object;

	SizeParameterName = "Particle Size Param";
}

void ASeekerBot::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	FString WeaponContext = "SeekerWeaponDataContext";
	FWeaponStatDataRow* WeaponData = WeaponDataTable->FindRow<FWeaponStatDataRow>(DataTableRowName, WeaponContext, true);

	if (WeaponData)
	{
		Damage = WeaponData->Damage;
		ExplosionRadius = WeaponData->ExplosionRange;
	}
}

void ASeekerBot::SpawnDeathVFX()
{
	if (DeathVFX)
	{
		FVector Location = GetActorLocation();
		FRotator Rotation = GetActorRotation();

		UParticleSystemComponent* VFX = UGameplayStatics::SpawnEmitterAtLocation(this, DeathVFX, Location, Rotation, true);

		if (VFX)
		{
			FVector SizeVector = FVector(ExplosionRadius, 0.0f, 0.0f);

			FParticleSysParam SizeParam;
			SizeParam.ParamType = EParticleSysParamType::PSPT_Vector;
			SizeParam.Vector = SizeVector;
			SizeParam.Name = SizeParameterName;
			VFX->InstanceParameters.Add(SizeParam);
		}
	}

	PlayFireWeaponSFX();
	DeathSFXComponent->SetSound(nullptr);
}