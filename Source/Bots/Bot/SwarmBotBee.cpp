// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SwarmBotBee.h"
#include "SwarmBotHive.h"
#include "Powers/Weapons/Weapon.h"

// Sets default values
ASwarmBotBee::ASwarmBotBee()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));	
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->AttachTo(RootComponent);

	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DesctructibleComponent"));
	DestructibleComponent->SetCollisionProfileName("Des");
	DestructibleComponent->AttachTo(RootComponent);

	FireRateRange = FVector2D(0.5f, 1.0f);
	FireOffset = FVector::ZeroVector;

	DeathSFXComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("DeathSFXComp"));
	DeathSFXComponent->bAlwaysPlay = false;
	DeathSFXComponent->bAutoActivate = false;

	DeathSFXComponent->AttachTo(RootComponent);

	FireWeaponSFXComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FireWeaponComponent"));
	FireWeaponSFXComponent->bAlwaysPlay = false;
	FireWeaponSFXComponent->bAutoActivate = false;
	FireWeaponSFXComponent->AttachTo(RootComponent);

	//static ConstructorHelpers::FObjectFinder<USoundCue> WeaponSFXObject01(TEXT("/Game/Audio/Bots/Shots/BasicBot_Cue_Red"));
	//static ConstructorHelpers::FObjectFinder<USoundCue> WeaponSFXObject02(TEXT("/Game/Audio/Bots/Shots/BasicBot_Cue_Green"));
	//static ConstructorHelpers::FObjectFinder<USoundCue> WeaponSFXObject03(TEXT("/Game/Audio/Bots/Shots/BasicBot_Cue_Purple"));
	//static ConstructorHelpers::FObjectFinder<USoundCue> WeaponSFXObject04(TEXT("/Game/Audio/Bots/Shots/BasicBot_Cue_Blue"));
	//static ConstructorHelpers::FObjectFinder<USoundCue> WeaponSFXObject05(TEXT("/Game/Audio/Bots/Shots/BasicBot_Cue_White"));

	//WeaponSFX.Add(WeaponSFXObject01.Object);
	//WeaponSFX.Add(WeaponSFXObject02.Object);
	//WeaponSFX.Add(WeaponSFXObject03.Object);
	//WeaponSFX.Add(WeaponSFXObject04.Object);
	//WeaponSFX.Add(WeaponSFXObject05.Object);

	BotIndex = -1;
	DestroyTime = 2.0f;

	PrimaryColorName = "Primary Color";
	SecondaryColorName = "Secondary Color";
	FractureColorName = "Fracture Color";

	DissolveBlendName = "DissolveBlend";
	DissolveOutlineWidthName = "DissolveLineWidth";

	bDissolveInprogress = false;
	DissolveOutlineWidth = 0.05f;

	DissolveStartDelay = 2.0f;
	DissolveDuration = 2.0f;

	ActorHalfHeightName = "Actor Half Height";
	SpawnBlendName = "Blend Value";
	SpawnOriginOffsetName = "Offset";
	SpawnEffectOutlineWidthName = "Spawn Line Width";

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ASwarmBotBee::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (MeshComponent->StaticMesh)
	{
		DestructibleComponent->SetHiddenInGame(true);
		DestructibleComponent->SetCollisionProfileName("NoCollision");
	}
}

void ASwarmBotBee::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();
	SetFireSoundCue();
}

void ASwarmBotBee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDissolveInprogress)
	{
		SetDissolveBlend();
	}
}

void ASwarmBotBee::SpawnWeapon()
{
	if (WeaponClass && GetWorld())
	{
		FActorSpawnParameters Param;
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, Param);
		if (Weapon)
		{
			Weapon->SetWeaponOwner(this, BotOwner);
		}
		else
		{
			GetWorldTimerManager().SetTimer(SpawnDelay_TimerHandle, this, &ASwarmBotBee::SpawnWeapon, 0.1f, false);
		}
	}
}

void ASwarmBotBee::FireWeapon()
{
	if (Weapon && Weapon->CanFire())
	{
		PlayFireWeaponSFX();
		Weapon->Fire(FireOffset);
	}
}

void ASwarmBotBee::HideAndDestroy()
{
	if (Weapon)
	{
		Weapon->StartDestroy();
	}

	if (DeathSFXComponent)
	{
		DeathSFXComponent->Play();
	}

	UDestructibleMesh* DMesh = DestructibleComponent->GetDestructibleMesh();
	if (!DMesh)
	{
		//Hide the actor and set the life span
		SetActorHiddenInGame(true);
		SetLifeSpan(DestroyTime);
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

		GetWorldTimerManager().SetTimer(DissolveDelay_TimerHandle, this, &ASwarmBotBee::StartDissolve, DissolveStartDelay, false);
		
		SetLifeSpan(DestroyTime);
	}
}

void ASwarmBotBee::SetBotOwner(AActor* InOwner, int32 Index)
{
	BotOwner = InOwner;
	BotIndex = Index;
}

AWeapon* ASwarmBotBee::GetWeapon()
{
	return Weapon;
}

void ASwarmBotBee::SetAttackType(FAttackType NewAttackType)
{
	AttackType = NewAttackType;
	SetDMIColor();
	SetFireSoundCue();
}

FAttackType ASwarmBotBee::GetAttackType() const
{
	return AttackType;
}

void ASwarmBotBee::SetDMIColor()
{
	for (int32 c = 0; c < DMI.Num(); c++)
	{
		if (DMI[c])
		{
			DMI[c]->SetVectorParameterValue(PrimaryColorName, AttackType.ColorData.PrimaryColor);
			DMI[c]->SetVectorParameterValue(SecondaryColorName, AttackType.ColorData.SecondaryColor);
			DMI[c]->SetVectorParameterValue(FractureColorName, AttackType.ColorData.SecondaryColor);
		}
	}
}

void ASwarmBotBee::StartSpawnEffect(float ActorHalfHeight, float Offset, float OutlineWidth)
{
	for (int32 c = 0; c < DMI.Num(); c++)
	{
		DMI[c]->SetScalarParameterValue(ActorHalfHeightName, ActorHalfHeight);
		DMI[c]->SetScalarParameterValue(SpawnOriginOffsetName, Offset);
		DMI[c]->SetScalarParameterValue(SpawnEffectOutlineWidthName, OutlineWidth);
	}
}

void ASwarmBotBee::SetSpawnEffectBlend(float InBlend)
{
	for (int32 c = 0; c < DMI.Num(); c++)
	{
		DMI[c]->SetScalarParameterValue(SpawnBlendName, InBlend);
	}
}

void ASwarmBotBee::StartDissolve()
{
	if (!bDissolveInprogress)
	{
		bDissolveInprogress = true;
	}
}

void ASwarmBotBee::SetDissolveBlend()
{
	if (!bDissolveInprogress)
	{
		return;
	}

	float DeltaTime = GetWorld()->GetDeltaSeconds();

	DissolveProgress += DeltaTime;

	float Blend = DissolveProgress / DissolveDuration;
	float DissolveBlend = FMath::Lerp(1.0f, 0.0f, Blend);

	for (int32 c = 0; c < DMI.Num(); c++)
	{
		DMI[c]->SetScalarParameterValue(DissolveBlendName, DissolveBlend);
		DMI[c]->SetScalarParameterValue(DissolveOutlineWidthName, DissolveOutlineWidth);
	}

	if (DissolveProgress > DissolveDuration)
	{
		bDissolveInprogress = false;
	}
}

void ASwarmBotBee::SetFireSoundCue()
{
	if (WeaponSFX.Num() <= 0)
	{
		return;
	}

	uint8 Index = (uint8)GetAttackType().AttackMode;
	if (!WeaponSFX.IsValidIndex(Index))
	{
		Index = 0;

		FString Name = GetName();
		GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Red, Name + " Didnt have a valid FireWeaponIndex. Check the WeaponSFX Array and ensure it has the correct number of elements");
	}

	FireWeaponSFXComponent->SetSound(WeaponSFX[Index]);
}

void ASwarmBotBee::PlayFireWeaponSFX()
{
	if (FireWeaponSFXComponent)
	{
		FireWeaponSFXComponent->Play();
	}
}