// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "NullifyingEMP.h"
#include "SnakeCharacter/SnakeLink.h"
#include "SnakeCharacter/SnakeLinkHead.h"
#include "Powers/AttackHelper.h"

// Sets default values
ANullifyingEMP::ANullifyingEMP()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComponent"));
	ParticleComponent->AttachTo(RootComponent);

	SFXComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EMP_SFXComponent"));
	SFXComponent->bAlwaysPlay = false;
	SFXComponent->bAutoActivate = false;
	SFXComponent->AttachTo(RootComponent);

	bDestroyed = false;

	ExecutionProgress = 0.0f;
	ExecutionDelay = 5.0f;

	DamagePercent = 0.05f;

	ExecuteRadius = 500.0f;
}

void ANullifyingEMP::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

// Called when the game starts or when spawned
void ANullifyingEMP::BeginPlay()
{
	Super::BeginPlay();

	SFXComponent->Play();
}

// Called every frame
void ANullifyingEMP::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	ExecutionProgress += DeltaTime;
	if (ExecutionProgress >= ExecutionDelay)
	{
		ExecuteEMP();
	}
}

void ANullifyingEMP::ExecuteEMP()
{
	if (bDestroyed)
	{
		return;
	}

	SFXComponent->Stop();
	if (ExplosionSFX)
	{
		SFXComponent->SetSound(ExplosionSFX);
		SFXComponent->Play();
	}

	ParticleComponent->DeactivateSystem();

	FVector Location = GetActorLocation();
	FRotator Rotation = GetActorRotation();
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionTemplate, Location, Rotation, true);

	FName ExecuteMove = FName("ExecuteEMPMove");
	FCollisionQueryParams Params(ExecuteMove, false, this);
	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjParams.AddObjectTypesToQuery(ECC_SnakeTraceChannel);

	TArray<FOverlapResult> OverlapResults;
	GetWorld()->OverlapMultiByObjectType(OverlapResults, GetActorLocation(), FQuat::Identity, ObjParams, FCollisionShape::MakeSphere(ExecuteRadius), Params);

	TArray<ASnakeLinkHead*> ValidActors;
	for (int32 c = 0; c < OverlapResults.Num(); c++)
	{
		AActor* Actor = OverlapResults[c].GetActor();
		if (Actor)
		{
			ASnakeLink* Link = Cast<ASnakeLink>(Actor);
			if (Link)
			{
				ASnakeLinkHead* Head = Link->GetHeadCast();
				bool bFound = false;
				for (int32 v = 0; v < ValidActors.Num(); v++)
				{
					if (Head == ValidActors[v])
					{
						bFound = true;
						break;
					}
				}

				if (!bFound)
				{
					ValidActors.Add(Head);
				}
			}
		}
	}

	for (int32 c = 0; c < ValidActors.Num(); c++)
	{
		//Nullify the head 
		ValidActors[c]->EnterNullifyState();

		//Damage the head
		float MaxHealth = ValidActors[c]->GetTotalHealth();
		float Damage = MaxHealth * DamagePercent;
		
		FDamageEvent DamageEvent;

		ValidActors[c]->TakeDamage(Damage, DamageEvent, nullptr, this);
	}

	HideAndDestroy();
}

void ANullifyingEMP::HideAndDestroy()
{
	if (!bDestroyed)
	{
		bDestroyed = true;
		SetActorHiddenInGame(true);
		SetLifeSpan(2.0f);
	}
}