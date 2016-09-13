// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "WeakPoint.h"
#include "Utility/AttackTypeDataAsset.h"
#include "Powers/AttackHelper.h"
#include "Utility/DataTables/StatData.h"

const FString AWeakPoint::WeakPointDataTableContext("WeakPointDataTableContext");

// Sets default values for this component's properties
AWeakPoint::AWeakPoint()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	HitBox->AttachTo(RootComponent);
	HitBox->bCanEverAffectNavigation = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	MeshComponent->AttachTo(RootComponent);
	MeshComponent->bCanEverAffectNavigation = false;

	HitSFXComp = CreateDefaultSubobject<UAudioComponent>(TEXT("HitSFX"));
	HitSFXComp->bAutoActivate = false;
	HitSFXComp->bAlwaysPlay = false;
	HitSFXComp->AttachTo(RootComponent);

	DestroyedSFXComp = CreateDefaultSubobject<UAudioComponent>(TEXT("DestroyedSFX"));
	DestroyedSFXComp->bAutoActivate = false;
	DestroyedSFXComp->bAlwaysPlay = false;
	DestroyedSFXComp->AttachTo(RootComponent);

	DamageModifier = 5.0f;

	static ConstructorHelpers::FObjectFinder<UAttackTypeDataAsset> AttackTypeDataAssetObject(TEXT("/Game/Blueprints/Attributes/White"));
	AttackTypeDataAsset = AttackTypeDataAssetObject.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> WeakPointDataTableObject(TEXT("/Game/Blueprints/Attributes/Stats_BotHealth"));
	WeakPointDataTable = WeakPointDataTableObject.Object;

	bCanTakeDamage = false;
	MaxHealth = 250;

	PrimaryColorName = "Primary Color";
	SecondaryColorName = "Secondary Color";

	PrimaryActorTick.bCanEverTick = true;
}

void AWeakPoint::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	FStatDataRow* WeakPointData = WeakPointDataTable->FindRow<FStatDataRow>(WeakPointDataRow, WeakPointDataTableContext, true);
	if (WeakPointData)
	{
		MaxHealth = WeakPointData->Health;
	}

	//DMI = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);
	//SetDMIColor();

	SetHealth(MaxHealth);
}

void AWeakPoint::SetHealth(int32 InHealth)
{
	Health = InHealth;
}

int32 AWeakPoint::GetHealth()
{
	return Health;
}

int32 AWeakPoint::GetMaxHealth()
{
	return MaxHealth;
}

void AWeakPoint::ReduceHealth(int32 InDamage)
{
	if (bDead || InDamage <= 0)
	{
		return;
	}

	Health -= InDamage;
	if (Health <= 0)
	{
		Dead();
	}
}

bool AWeakPoint::IsDead() const
{
	return bDead;
}

void AWeakPoint::Dead()
{
	bDead = true;

	Hide();
}

void AWeakPoint::SetDeathFlag(bool InFlag)
{
	bDead = InFlag;
}

float AWeakPoint::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (WeakPointOwner)
	{
		float NewDamage = Damage * DamageModifier;
		WeakPointOwner->TakeDamage(NewDamage, DamageEvent, EventInstigator, this);

		PlayHitSFX();
		OnWeakPointTakeDamage.ExecuteIfBound();
	}
	else if (bCanTakeDamage && AttackTypeDataAsset)
	{

		int32 DamageTaken = Damage;
		AttackHelper::CalculateDamage(DamageTaken, DamageEvent, AttackTypeDataAsset->Data);

		ReduceHealth(DamageTaken);

		if (FragmentCount > 0)
		{
			StoredDamage += DamageTaken;
			float Threshold = (1.0f / (float)FragmentCount) * MaxHealth;
			while (StoredDamage >= Threshold)
			{
				StoredDamage -= FMath::CeilToInt(Threshold);

				Fragment();
			}
		}

		PlayHitSFX();
		OnWeakPointTakeDamage.ExecuteIfBound();
	}

	return 0.0f;
}

void AWeakPoint::ResetWeakPoint()
{
	if (bCanTakeDamage)
	{
		StoredDamage = 0;
		SetHealth(MaxHealth);
	}

	bDead = false;

	Show();
}

void AWeakPoint::SetWeakPointOwner(AActor* InOwner)
{
	if (InOwner)
	{
		WeakPointOwner = InOwner;
	}
}

void AWeakPoint::HideAndDestroy()
{
	if (!bDead)
	{
		bDead = true;
		Hide();
		WeakPointOwner = nullptr;
		SetLifeSpan(2.0f);
	}
}

void AWeakPoint::Hide()
{
	OnDestroyed.ExecuteIfBound(this);
	OnHide();

	SetActorHiddenInGame(true);
	RemoveCollision();
}

void AWeakPoint::RemoveCollision()
{
	HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AWeakPoint::Show()
{
	OnShow();
	SetActorHiddenInGame(false);
	HitBox->SetCollisionProfileName("BlockAll");
}

void AWeakPoint::SetDMIColor()
{
	if (DMI)
	{
		DMI->SetVectorParameterValue(PrimaryColorName, AttackTypeDataAsset->Data.ColorData.PrimaryColor);
	}
}

void AWeakPoint::SetAttackType(UAttackTypeDataAsset* InType)
{
	if (InType)
	{
		AttackTypeDataAsset = InType;

		OnAttackTypeChange();
		//SetDMIColor();
	}
}

void AWeakPoint::SetAttackTypeStruct(FAttackType InType)
{
	AttackTypeDataAsset->Data = InType;

	OnAttackTypeChange();
}

uint8 AWeakPoint::GetAttackMode()
{
	return AttackTypeDataAsset ? (uint8)AttackTypeDataAsset->Data.AttackMode : -1;
}

void AWeakPoint::PlayDestroySFX()
{
	DestroyedSFXComp->Play();
}

void AWeakPoint::PlayHitSFX()
{
	if (HitSFXCues.Num() <= 0)
	{
		return;
	}

	int32 Rand = FMath::FRandRange(0, HitSFXCues.Num() - 1);
	if (HitSFXCues.IsValidIndex(Rand))
	{
		HitSFXComp->SetSound(HitSFXCues[Rand]);
		HitSFXComp->Play();
	}
}