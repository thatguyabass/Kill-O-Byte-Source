// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "DestructibleTrigger.h"
#include "Powers/AttackHelper.h"
#include "Utility/AttackTypeDataAsset.h"

// Sets default values
ADestructibleTrigger::ADestructibleTrigger()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->AttachTo(RootComponent);
	//Remove any Mesh Collision
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	MaxHealth = 10;
	bDead = false;

	PrimaryColorName = "Primary Color";

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADestructibleTrigger::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	SetHealth(MaxHealth);
}

float ADestructibleTrigger::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageAmount <= 0.0f)
	{
		return 0.0f;
	}

	int32 DamageTaken = DamageAmount;
	AttackHelper::CalculateDamage(DamageTaken, DamageEvent, GetAttackType());

	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::SanitizeFloat(DamageTaken));

	// Check if the state is a valid player state. If false, a bot must have fired the projectile. 
	ASnakePlayerState* State = Cast<ASnakePlayerState>(DamageCauser);
	if (State)
	{
		ReduceHealth(DamageTaken);
	}

	return 0.0f;
}

FAttackType ADestructibleTrigger::GetAttackType()
{
	return AttackTypeDataAsset ? AttackTypeDataAsset->Data : FAttackType();
}

void ADestructibleTrigger::ReduceHealth(float DamageAmount)
{
	if (DamageAmount < 0)
	{
		return;
	}

	Health -= DamageAmount;

	if (Health <= 0)
	{
		HideAndDestroy();
	}
}

void ADestructibleTrigger::SetHealth(int32 InHealth)
{
	if (InHealth <= 0)
	{
		return;
	}

	Health = InHealth;
	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}
}

void ADestructibleTrigger::HideAndDestroy()
{
	if (!bDead)
	{
		//Call the Blueprint Event to trigger. Also remvoe any attached Collision objects 
		Trigger();

		SetActorHiddenInGame(true);
		SetLifeSpan(2.0f);

		bDead = true;
	}
}

void ADestructibleTrigger::CreateAndSetDMI()
{
	DMI.Empty();

	if (MeshComponent && MeshComponent->GetNumMaterials() > 0)
	{
		for (int32 c = 0; c < MeshComponent->GetNumMaterials(); c++)
		{
			DMI.Add(MeshComponent->CreateAndSetMaterialInstanceDynamic(c));
		}
	}

	SetMaterialColor();
}

void ADestructibleTrigger::SetMaterialColor()
{
	if (DMI.Num() > 0)
	{
		for (int32 c = 0; c < DMI.Num(); c++)
		{
			DMI[c]->SetVectorParameterValue(PrimaryColorName, GetAttackType().ColorData.PrimaryColor);
		}
	}
}