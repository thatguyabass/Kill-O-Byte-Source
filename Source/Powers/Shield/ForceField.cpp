// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "ForceField.h"
#include "Powers/AttackHelper.h"
#include "Utility/Utility.h"
#include "Utility/AttackTypeDataAsset.h"
#include "Utility/DataTables/StatData.h"

const FString AForceField::ForceFieldContext("ForceFieldDataTable");

// Sets default values
AForceField::AForceField()
{
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = RootComp;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->AttachTo(RootComponent);
	MeshComponent->bCanEverAffectNavigation = false;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAlwaysPlay = false;
	AudioComponent->bAutoActivate = false;
	AudioComponent->AttachTo(RootComponent);

	HitAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("HitAudioComponent"));
	HitAudioComponent->bAlwaysPlay = false;
	HitAudioComponent->bAutoActivate = false;
	HitAudioComponent->AttachTo(RootComponent);

	static ConstructorHelpers::FObjectFinder<UDataTable> ForceFieldDataTableObject(TEXT("/Game/Blueprints/Attributes/Stats_ForceFields"));
	ForceFieldDataTable = ForceFieldDataTableObject.Object;

	EnableMeshCollision_Implementation();

	bUsePrimaryColor = true;
	PrimaryColorParameterName = "Primary Color";
	AlphaParameterName = "Alpha Param";
	GlowParameterName = "Glow Param";
	GlowValue = 10.0f;

	ForceFieldRowName = "Player";

	MaxHealth = 15;
	Health = 0;
	bCanRegen = false;
	DamageDelay = 5.0f;
	HealthRegen = 1;

	RegenTickRate = 0.5f;
	bRegenTick = false;

	bIsActive = true;
	bShutdown = false;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AForceField::BeginPlay()
{
	Super::BeginPlay();
	
	if (HitCurve)
	{
		HitTimeline = FTimeline();
		FOnTimelineFloatStatic Func;

		Func.BindUObject(this, &AForceField::SetShieldAlpha);
		HitTimeline.AddInterpFloat(HitCurve, Func);
		HitTimeline.SetTimelineLength(HitTimelineLength);
	}

	if (ShutdownCurve)
	{
		ShutdownTimeline = FTimeline();
		FOnTimelineFloatStatic Func;

		Func.BindUObject(this, &AForceField::SetShieldAlpha);
		ShutdownTimeline.AddInterpFloat(ShutdownCurve, Func);
		ShutdownTimeline.SetTimelineLength(ShutdownTimelineLength);
	}

	if (GlowCurve)
	{
		GlowTimeline = FTimeline();
		FOnTimelineFloatStatic Func;

		Func.BindUObject(this, &AForceField::SetShieldGlow);
		GlowTimeline.AddInterpFloat(GlowCurve, Func);
		GlowTimeline.SetTimelineLength(GlowTimelineLength);
	}

	if (bCanRegen)
	{
		// Regen Tick.
		GetWorldTimerManager().SetTimer(RegenTick_TimerHandle, this, &AForceField::RegenHealth, RegenTickRate, true);
	}

	SetHealth(MaxHealth);
}

void AForceField::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	FForceFieldDataRow* ShieldData = ForceFieldDataTable->FindRow<FForceFieldDataRow>(ForceFieldRowName, ForceFieldContext, true);
	if(ShieldData)
	{
		MaxHealth = ShieldData->ShieldHealth;
		RegenTickRate = ShieldData->RegenTickRate;
		HealthRegen = ShieldData->HealthPerTick;
	}
	else
	{
		FString N = GetName();
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, "Bot Health Data Table Missing on: " + N);
	}

	if (MeshComponent)
	{
		DMI = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);
	}
	SetAttackType(GetAttackType());

	if (!bIsActive)
	{
		Hide();
	}
}

// Called every frame
void AForceField::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (HitTimeline.IsPlaying())
	{
		HitTimeline.TickTimeline(DeltaTime);
	}

	if (ShutdownTimeline.IsPlaying())
	{
		ShutdownTimeline.TickTimeline(DeltaTime);
	}

	if (GlowTimeline.IsPlaying())
	{
		GlowTimeline.TickTimeline(DeltaTime);
	}
}

int32 AForceField::GetTeamNumber() const
{
	ASnakeLinkHead* Head = Cast<ASnakeLinkHead>(ShieldOwner);
	if (Head)
	{
		return Head->GetTeamNumber();
	}

	ABot* Bot = Cast<ABot>(ShieldOwner);
	if (Bot)
	{
		return Bot->TeamNumber;
	}

	return -1;
}

void AForceField::RegenHealth()
{
	if (!bRegenTick || !bCanRegen)
	{
		return;
	}

	GlowTimeline.PlayFromStart();

	AddHealth(HealthRegen);
	if (Health >= MaxHealth)
	{
		PlaySFX(RegenSFX);
		StopRegen();
	}
}

void AForceField::ActivateForceField()
{
	if (!bIsActive && !bShutdown)
	{
		bIsActive = true;
		ShowWithTimeline();
	}
}

void AForceField::DeactivateForceField()
{
	if (bIsActive && !bShutdown)
	{
		bIsActive = false;
		DisableMeshCollision();
		HideWithTimeline();
	}
}

void AForceField::SetShieldOwner(AActor* InOwner)
{
	ShieldOwner = InOwner;

	if (InOwner)
	{
		AttachRootComponentToActor(InOwner, NAME_None, EAttachLocation::KeepWorldPosition, false);

		APawn* InPawn = Cast<APawn>(InOwner);
		if(InPawn)
		{
			InPawn->MoveIgnoreActorAdd(this);
		}
	}
}

AActor* AForceField::GetShieldOwner() const
{
	return ShieldOwner;
}

void AForceField::SetAttackType(FAttackType InAttackType)
{
	if (DMI)
	{
		AttackType = InAttackType;
		const FLinearColor Color = bUsePrimaryColor ? AttackType.ColorData.PrimaryColor : AttackType.ColorData.SecondaryColor;
		DMI->SetVectorParameterValue(PrimaryColorParameterName, Color);
		SetShieldGlow(GlowValue);
	}
}

FAttackType AForceField::GetAttackType()
{
	if (AttackTypeDataAsset == nullptr)
	{
		return AttackType;
	}
	else
	{
		return AttackTypeDataAsset->Data;
	}
}

void AForceField::SetShieldAlpha(float Alpha)
{
	if (DMI)
	{
		DMI->SetScalarParameterValue(AlphaParameterName, Alpha);
	}
}

void AForceField::SetShieldGlow(float Glow)
{
	if (DMI)
	{
		DMI->SetScalarParameterValue(GlowParameterName, Glow);
	}
}

float AForceField::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	ASnakePlayerState* State = GetPlayerState();
	if (State && DamageCauser && State == DamageCauser)
	{
		return 0.0f;
	}

	int32 DamageTaken = DamageAmount;
	ASnakeLinkHead* Head = Cast<ASnakeLinkHead>(ShieldOwner);
	if (Head)
	{
		//If the attack types are the same, deal reduced damage. Opposite of the Bots AND anywhere else 
		AttackHelper::CalculateDamage(DamageTaken, DamageEvent, AttackType, true);
	}
	else
	{
		//If the attack types are the same, deal full damage
		AttackHelper::CalculateDamage(DamageTaken, DamageEvent, AttackType);
	}

	ReduceHealth(DamageTaken);

	return 0.0f;
}

float AForceField::ReduceHealth(int32 Damage)
{
	if (Damage < 0)
	{
		return 0.0f;
	}

	float Diff = 0.0f;

	HitAudioComponent->Play();

	Health -= Damage;
	if (Health < 0)
	{
		Diff = Health * -1;
		PlaySFX(ShutdownSFX);
		Shutdown(true);
	}
	else
	{
		HitTimeline.PlayFromStart();

		if (bCanRegen)
		{
			StopRegen();
			GetWorldTimerManager().SetTimer(Regen_TimerHandle, this, &AForceField::StartRegen, DamageDelay, false);
		}
	}

	return Diff;
}

void AForceField::SetHealth(int32 Value)
{
	Health = Value;
	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}

	ShutdownTimeline.ReverseFromEnd();
}

void AForceField::AddHealth(int32 Value)
{
	if (Value <= 0)
	{
		return;
	}

	Health += Value;
	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}
}

int32 AForceField::GetHealth() const
{
	return Health;
}

int32 AForceField::GetMaxHealth() const
{
	return MaxHealth;
}

void AForceField::StartRegen()
{
	bRegenTick = true;
}
void AForceField::StopRegen()
{
	bRegenTick = false;
	GetWorldTimerManager().ClearTimer(Regen_TimerHandle);
	GlowTimeline.Stop();
}

void AForceField::Shutdown(bool PlayTimeline)
{
	if (!bShutdown)
	{
		bShutdown = true; 
		StopRegen();

		OnShutdown.ExecuteIfBound();

		DisableMeshCollision();
		if (bIsActive && PlayTimeline)
		{
			HideWithTimeline();
		}
		else
		{
			Hide();
		}

		if (RechargeTime > 0.0f)
		{
			GetWorldTimerManager().SetTimer(Recharge_TimerHandle, this, &AForceField::InternalRestart, RechargeTime, false);
		}
	}
}

void AForceField::HideAndDestroy(bool PlayTimeline)
{
	Shutdown(PlayTimeline);

	SetLifeSpan(2.0f);
}

void AForceField::Hide()
{
	AudioComponent->Stop();

	SetActorHiddenInGame(true);
	DisableMeshCollision();
}

void AForceField::HideWithTimeline()
{
	AudioComponent->FadeOut(ShutdownTimelineLength, 0.0f);

	ShutdownTimeline.PlayFromStart();
	GetWorldTimerManager().SetTimer(Hide_TimerHandle, this, &AForceField::Hide, ShutdownTimelineLength, false);
}

void AForceField::Show()
{
	SetActorHiddenInGame(false);
	EnableMeshCollision();
}

void AForceField::ShowWithTimeline()
{
	AudioComponent->FadeIn(ShutdownTimelineLength, 1.0f, 0.0f);

	GetWorldTimerManager().ClearTimer(Hide_TimerHandle);
	ShutdownTimeline.ReverseFromEnd();
	Show();
}

void AForceField::InternalRestart()
{
	PlaySFX(RegenSFX);
	Restart(true);
}

void AForceField::Restart(bool Reset)
{
	OnRestart.ExecuteIfBound();
	
	if (Reset)
	{
		SetHealth(MaxHealth);
	}

	if (bIsActive)
	{
		ShowWithTimeline();
	}

	bShutdown = false;
}

void AForceField::EnableMeshCollision_Implementation()
{
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
	MeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Overlap);
}

void AForceField::DisableMeshCollision_Implementation()
{
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
}

bool AForceField::IsShutdown() const
{
	return bShutdown;
}

ASnakePlayerState* AForceField::GetPlayerState()
{
	ACharacter* Character = Cast<ACharacter>(ShieldOwner);
	return Character ? Cast<ASnakePlayerState>(Character->PlayerState) : nullptr;
}

void AForceField::PlaySFX(USoundBase* InSFX)
{
	if (InSFX)
	{
		UGameplayStatics::PlaySound2D(this, InSFX);
	}
}