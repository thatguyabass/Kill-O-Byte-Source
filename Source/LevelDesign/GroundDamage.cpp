// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "GroundDamage.h"
#include "SnakeCharacter/SnakeLink.h"
#include "Bots/Bot/Bot.h"
#include "Powers/AttackHelper.h"
#include "Powers/Projectiles/BaseProjectile.h"
#include "Utility/AttackTypeDataAsset.h"

// Sets default values
AGroundDamage::AGroundDamage()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	OverlapComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComponent"));
	OverlapComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapComponent->SetCollisionResponseToChannel(ECC_SnakeTraceChannel, ECR_Overlap);
	OverlapComponent->SetCollisionResponseToChannel(ECC_ProjectileChannel, ECR_Overlap);
	OverlapComponent->SetBoxExtent(FVector(250.0f));
	OverlapComponent->AttachTo(RootComponent);

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->AttachTo(RootComponent);

	static ConstructorHelpers::FClassFinder<UDamageType> _DamageTypeClass(TEXT("/Game/Blueprints/Powers/Projectiles/BP_DamageType_AttackMode"));
	DamageTypeClass = _DamageTypeClass.Class;

	PrimaryDecalColorName = "Primary Color";
	AlphaParameterName = "Alpha";
	GlowParameterName = "Glow";
	AttackTypeIndex = 0;

	ColorLerpDuration = 1.5f;
	ColorLerpProgress = 0.0f;
	bColorLerpInprogress = false;

	InitialColor = FLinearColor::White;
	NewColor = FLinearColor::White;

	GlowValue = 25.0f;

	FadeDuration = 1.5f;
	FadeProgress = 0.0f;
	bActive = true;

	bFadeInprogress = false;

	SameColorDamagePercent = 0.1f;
	DamagePercent = 0.05f;
	DamageFrequency = 0.125f;
	DamageProgress = 0.0f;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AGroundDamage::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &AGroundDamage::OnBeginOverlap);
	OverlapComponent->OnComponentEndOverlap.AddDynamic(this, &AGroundDamage::OnEndOverlap);

	bActive = true;

	SetDMIColor();
}

// Called when the game starts or when spawned
void AGroundDamage::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGroundDamage::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	DamageProgress += DeltaTime;
	if ((DamageProgress > DamageFrequency) && !bColorLerpInprogress)
	{
		DamageProgress = 0.0f;
		DamageOverlapedActors();
	}

	if (bFadeInprogress)
	{
		FadeProgress += DeltaTime;

		float Blend = FadeProgress / FadeDuration;
		if (!bActive)
		{
			Blend = 1 - Blend;
		}

		float Alpha = FMath::Lerp(0.0f, 1.0f, Blend);

		if (DMI)
		{
			DMI->SetScalarParameterValue(AlphaParameterName, Alpha);
		}

		if (FadeProgress > FadeDuration)
		{
			FadeProgress = 0.0f;
			bFadeInprogress = false;
		}
	}

	if (bColorLerpInprogress && bActive)
	{
		ColorLerpProgress += DeltaTime;

		float Blend = ColorLerpProgress / ColorLerpDuration;
		FLinearColor Color = FMath::Lerp(InitialColor, NewColor, Blend);
		if (DMI)
		{
			DMI->SetVectorParameterValue(PrimaryDecalColorName, Color);
		}

		if (ColorLerpProgress > ColorLerpDuration)
		{
			ColorLerpProgress = 0.0f;
			bColorLerpInprogress = false;

			if (GetOverlappedPlayer())
			{
				OnPlayerEnteredOverlap();
			}
		}
	}
}

void AGroundDamage::DamageOverlapedActors()
{
	if (!bActive)
	{
		return;
	}

	for (int32 c = 0; c < ActorsInOverlap.Num(); c++)
	{
		FGroundDamageContainer& Cont = ActorsInOverlap[c];
		if (!Cont.bCanBeDamaged)
		{
			Cont.CountDamageTick();
			continue;
		}

		AActor* Actor = Cont.InActor;
		int32 Damage = 0;
		FAttackType Other;
		FAttackType AttackType = AttackTypeDataAssets[AttackTypeIndex]->Data;
		
		ASnakeLinkHead* Link = Cast<ASnakeLinkHead>(Actor);
		if (Link && !Link->IsDead())
		{
			Other = Link->GetAttackType();

			if (Link->IsShieldButtonHeld())
			{
				FAttackType Shield = Link->GetForceFieldAttackType();
				if (Shield.AttackMode == AttackType.AttackMode)
				{
					//Stop the Damage Process becuase the force field is active and on the same attack mode 
					continue;
				}
			}

			int32 MaxHealth = Link->GetTotalHealth();
			Damage = DamagePercent * MaxHealth; 
		}

		FDamageEvent Event = AttackHelper::CreateDamageEvent(DamageTypeClass, AttackType);
		AttackHelper::CalculateDamage(Damage, Event, Other, true, SameColorDamagePercent);

		Actor->TakeDamage(Damage, Event, nullptr, this);
	}
}

void AGroundDamage::OnBeginOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweeptResult)
{
	if (!bActive)
	{
		return;
	}

	bool bCanEnter = false;
	ASnakeLink* Link = Cast<ASnakeLink>(Other);
	if (Link)
	{
		ASnakeLink* Head = Link->GetHead();
		if (Head)
		{
			bool bFound = FindActorInOverlap(Head);

			if (!bFound)
			{
				bCanEnter = true;
			}
		}
	}

	if (bCanEnter)
	{
		FGroundDamageContainer Cont(Other);
		ActorsInOverlap.Add(Cont);
		OnPlayerEnteredOverlap();
	}
}

void AGroundDamage::OnEndOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Other)
	{
		return;
	}

	for (int32 c = 0; c < ActorsInOverlap.Num(); c++)
	{
		if (Other == ActorsInOverlap[c].InActor)
		{
			OnPlayerExitedOverlap();
			ActorsInOverlap.RemoveAt(c);
			break;
		}
	}
}

void AGroundDamage::SetAttackType(UAttackTypeDataAsset* InAttackDataAsset)
{
	if (!InAttackDataAsset)
	{
		return;
	}

	AttackTypeDataAssets.Empty();

	AttackTypeDataAssets.Add(InAttackDataAsset);
}

void AGroundDamage::SetDMIColor()
{
	if (DMI && AttackTypeDataAssets.Num() > 0)
	{
		if (!bManualSet)
		{
			AttackTypeIndex = FMath::RandRange(0, AttackTypeDataAssets.Num() - 1);
		}

		if (AttackTypeDataAssets.IsValidIndex(AttackTypeIndex) && AttackTypeDataAssets[AttackTypeIndex])
		{
			DMI->SetVectorParameterValue(PrimaryDecalColorName, AttackTypeDataAssets[AttackTypeIndex]->GetPrimaryColor());
			DMI->SetScalarParameterValue(GlowParameterName, GlowValue);
		}

		OnAttackTypeChange();
	}
}

bool AGroundDamage::FindActorInOverlap(AActor* InActor)
{
	if (!InActor)
	{
		return false;
	}

	bool bFound = false;

	for (int32 c = 0; c < ActorsInOverlap.Num(); c++)
	{
		if (InActor == ActorsInOverlap[c].InActor)
		{
			bFound = true;
			break;
		}
	}

	return bFound;
}

void AGroundDamage::ShowAndActivate()
{
	if (!bActive)
	{
		bActive = true;
		bFadeInprogress = true;
	}
}

void AGroundDamage::Hide()
{
	if (bActive)
	{
		bActive = false;
		bFadeInprogress = true;
	}
}

void AGroundDamage::HideAndDestroy()
{
	Hide();

	SetLifeSpan(FadeDuration + 1.0f);
}

void AGroundDamage::StartColorLerp(int32 InIndex)
{
	if (!AttackTypeDataAssets.IsValidIndex(InIndex))
	{
		return;
	}

	bManualSet = true;
	InitialColor = AttackTypeDataAssets[AttackTypeIndex]->GetPrimaryColor();
	NewColor = AttackTypeDataAssets[InIndex]->GetPrimaryColor();

	bColorLerpInprogress = true;

	for (int32 c = 0; c < ActorsInOverlap.Num(); c++)
	{
		//Reset the tick count so players dont take damage from the new color right on color change.
		ActorsInOverlap[c].ResetTickCount();
	}

	OnPlayerExitedOverlap();

	SetAttackTypeIndex(InIndex);
}

void AGroundDamage::SetAttackTypeIndex(int32 InIndex)
{
	if (bManualSet)
	{
		AttackTypeIndex = InIndex;
	}
}

int32 AGroundDamage::GetAttackTypeIndex() const
{
	return AttackTypeIndex;
}

FAttackType AGroundDamage::GetAttackType()
{
	return AttackTypeDataAssets.IsValidIndex(AttackTypeIndex) ? AttackTypeDataAssets[AttackTypeIndex]->Data : FAttackType();
}

ASnakeLink* AGroundDamage::GetOverlappedPlayer()
{
	for (int32 c = 0; c < ActorsInOverlap.Num(); c++)
	{
		ASnakeLink* Link = Cast<ASnakeLink>(ActorsInOverlap[c].InActor);
		if (Link)
		{
			return Link;
		}
	}

	return nullptr;
}

void FGroundDamageContainer::CountDamageTick()
{
	if (!IsValid())
	{
		return;
	}

	TickCount++;
	if (TickCount >= TickCountTotal)
	{
		bCanBeDamaged = true;
	}
}

bool FGroundDamageContainer::IsValid() const
{
	return InActor != nullptr;
}

bool AGroundDamage::CompareAttackTypes(FAttackType InAttackType)
{
	return AttackTypeDataAssets.IsValidIndex(AttackTypeIndex) ? (AttackTypeDataAssets[AttackTypeIndex]->Data.AttackMode == InAttackType.AttackMode) : false;
}