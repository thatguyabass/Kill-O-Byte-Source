// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "Bot.h"
#include "Bots/Controller/BaseAIController.h"
#include "Bots/BotManager.h"
#include "Powers/Weapons/Weapon.h"
#include "Powers/AttackHelper.h"
#include "GameMode/GameMode_Arcade.h"
#include "GameMode/PlayerState/SnakePlayerState.h"
#include "Components/FloatyTextManager.h"
#include "Utility/Attributes.h"
#include "Utility/AttackTypeDataAsset.h"
#include "Utility/LootTable.h"
#include "Runtime/Engine/Classes/Sound/SoundCue.h"

const float ABot::BaseSlowModifier = 1.0f;

const FString ABot::DataTableContext("BotStatDataTable");

// Sets default values
ABot::ABot()
	: Super()
{
	AIControllerClass = ABaseAIController::StaticClass();

	static ConstructorHelpers::FObjectFinder<UAttackTypeDataAsset> AttackTypeDataAssetObject(TEXT("/Game/Blueprints/Attributes/White"));
	AttackTypeDataAsset = AttackTypeDataAssetObject.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> StatDataAssetObject(TEXT("/Game/Blueprints/Attributes/Stats_BotHealth"));
	StatDataTable = StatDataAssetObject.Object;

	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DestructibleComponent"));
	DestructibleComponent->SetCollisionProfileName("Des");
	DestructibleComponent->AttachTo(RootComponent);

	SpawnEffectVFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BotSpawnEffectVFX"));
	SpawnEffectVFX->bAutoActivate = false;
	SpawnEffectVFX->AttachTo(RootComponent);

	DeathSFXComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("DeathSFXComp"));
	DeathSFXComponent->bAlwaysPlay = false;
	DeathSFXComponent->bAutoActivate = false;
	
	//static ConstructorHelpers::FObjectFinder<USoundCue> DeathSFXCueObject(TEXT("/Game/Audio/Bots/Deaths/BotDeath_Cue"));
	//DeathSFXComponent->SetSound(DeathSFXCueObject.Object);

	DeathSFXComponent->AttachTo(RootComponent);

	AmbientSFXComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AmbientSFXComp"));
	AmbientSFXComponent->bAlwaysPlay = false;
	AmbientSFXComponent->bAutoActivate = false;
	AmbientSFXComponent->AttachTo(RootComponent);

	FireWeaponSFXComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FireWeaponComponent"));
	FireWeaponSFXComponent->bAlwaysPlay = false;
	FireWeaponSFXComponent->bAutoActivate = false;
	FireWeaponSFXComponent->AttachTo(RootComponent);

	SpawnSFXComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SpawnSFXComponent"));
	SpawnSFXComponent->bAlwaysPlay = false;
	SpawnSFXComponent->bAutoActivate = false;
	SpawnSFXComponent->AttachTo(RootComponent);

	static ConstructorHelpers::FObjectFinder<USoundCue> SpawnSFXCueObject(TEXT("/Game/Audio/Bots/BotAmbience/BotSpawn_Cue"));
	SpawnSFXComponent->SetSound(SpawnSFXCueObject.Object);

	bPlaySpawnSFX = true;
	bPlayFireSFX = true;

	TeamNumber = 5;
	SearchRadius = 1000.0f;

	MaxSlowModifier = 0.5f;

	PrimaryColorName = "Primary Color";
	SecondaryColorName = "Secondary Color";
	FractureColorName = "Fracture Color";

	DissolveBlendName = "DissolveBlend";
	DissolveOutlineWidthName = "DissolveLineWidth";

	bDissolveInprogress = false;
	DissolveOutlineWidth = 0.05f;

	DissolveStartDelay = 2.0f;
	DissolveDuration = 2.0f;

	bSpawnEffectDebug = false;
	ActorHalfHeightName = "Actor Half Height";
	SpawnBlendName = "Blend Value";
	SpawnOriginOffsetName = "Offset";
	SpawnEffectOutlineWidthName = "Spawn Line Width";

	bSpawnEffectInprogress = false;
	SpawnEffectDuration = 1.0f;
	SpawnEffectProgress = 0.0f;
	ActorHalfHeight = 60.0f;
	SpawnOffset = 0.0f;
	SpawnEffectOutlineWidth = 0.1f;

	BaseHealth = 10;
	Health = 0;
	bDead = false;
	DestroyTime = 5.0f;

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Debug 
	bShowBotDamage = false;

	Points = 1;

	bForceDamageOverride = false;

	bReplicates = true;
	bReplicateMovement = true;
}

void ABot::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	FStatDataRow* StatData = StatDataTable->FindRow<FStatDataRow>(DataTableRowName, DataTableContext, false);

	if (StatData)
	{
		BaseHealth = StatData->Health;
		Health = BaseHealth;
		BaseMovementSpeed = GetCharacterMovement()->MaxWalkSpeed;
	}
	else
	{
		FString N = GetName();
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, "Bot Health Data Table Missing on: " + N);
	}

	for (int32 c = 0; c < DMI.Num(); c++)
	{
		DMI[c]->SetScalarParameterValue(DissolveBlendName, 1.0f);
		DMI[c]->SetScalarParameterValue(DissolveOutlineWidthName, DissolveOutlineWidth);
	}

	SpawnWeapon();

	if (GetMesh()->SkeletalMesh)
	{
		DestructibleComponent->SetHiddenInGame(true);
		DestructibleComponent->SetCollisionProfileName("NoCollision");
	}

	//SetFireWeaponSoundCue();
}

void ABot::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	bSpawnEffectDebug ? SetSpawnEffectBlend(0.5f) : SetSpawnEffectBlend(1.0f);
}

void ABot::BeginPlay()
{
	Super::BeginPlay();

	if (bPlaySpawnVFX)
	{
		StartSpawnEffect();
	}

	GetWorldTimerManager().SetTimer(Ambient_TimerHandle, this, &ABot::StartAmbientSFX, 2.0f, false);
}

void ABot::SpawnWeapon()
{
	if (WeaponClass)
	{
		Weapon = Cast<AWeapon>(GetWorld()->SpawnActor(WeaponClass));
		Weapon->SetWeaponOwner(this);

		PostSpawnWeapon();
	}
}

void ABot::PostSpawnWeapon()
{

}

AWeapon* ABot::GetWeapon()
{
	return Weapon;
}

void ABot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SlowModifiers.Num() > 0)
	{
		bool Change = false;
		for (int32 c = 0; c < SlowModifiers.Num(); c++)
		{
			SlowModifiers[c].Progress += DeltaTime;
			if (SlowModifiers[c].Progress > SlowModifiers[c].Duration)
			{
				SlowModifiers[c].bIsActive = false;
				SlowModifiers.RemoveAt(c);
				c--;

				Change = true;
			}
		}

		if (Change)
		{
			ApplyMovementChange();
		}
	}

	if (bDissolveInprogress)
	{
		SetDissolveBlend();
	}

	if (bSpawnEffectInprogress)
	{
		SpawnEffectTick();
	}
}

void ABot::GetPointsOfInterest(TArray<APointOfInterest*>& Out)
{
	Out.Empty();

	if (PointOfInterestGroup.IsValid())
	{
		for (int32 c = 0; c < PointOfInterestGroup.Points.Num(); c++)
		{
			Out.Add(PointOfInterestGroup.Points[c]);
		}
	}
	else
	{
		for (TActorIterator<APointOfInterest> Interest(GetWorld()); Interest; ++Interest)
		{
			if (*Interest)
			{
				Out.Add(*Interest);
			}
		}
	}
}

void ABot::SetPointOfInterestGroup(FPointOfInterestGroup& Group)
{
	if (Group.IsValid())
	{
		PointOfInterestGroup = Group;
	}
}

void ABot::FireWeapon()
{
	if (Weapon && Weapon->CanFire())
	{
		PlayFireWeaponSFX();
		Weapon->Fire(FireOffset);
	}
}

float ABot::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bDead || Damage <= 0.0f || !bCanBeDamaged)
	{
		return 0.0f;
	}

	TargetInstigator(EventInstigator);

	int32 DamageTaken = Damage;
	if (!bForceDamageOverride)
	{
		AttackHelper::CalculateDamage(DamageTaken, DamageEvent, GetAttackType());
	}

	if (bShowBotDamage)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, "Bot Damage Taken: " + FString::SanitizeFloat(DamageTaken));
	}

	ASnakePlayerState* State = Cast<ASnakePlayerState>(DamageCauser);
	ReduceHealth(DamageTaken, State);

	//Force Damage override set to false
	//This will ensure that this bot will take normal damage unless set elsewhere. 
	bForceDamageOverride = false;

	return 0.0f;
}

void ABot::TargetInstigator(AController* Instigator)
{
	if (Instigator)
	{
		APawn* Pawn = Instigator->GetPawn();
		ABaseAIController* AI = Cast<ABaseAIController>(GetController());
		if (Pawn && AI)
		{
			SearchRadius = 10000.0f;
			AI->SetTarget(Pawn);
		}
	}
}

void ABot::ReduceHealth(int32 Damage, ASnakePlayerState* DamageCauser)
{
	//Return if this bot is invulnerable
	if (bInvulnerable && !bOverrideInvulnerable)
	{
		return;
	}

	if (Damage <= 0.0f)
	{
		Damage = 0.0f;
	}

	Health -= Damage;
	if (Health <= 0)
	{
		Dead(DamageCauser);
	}

	bOverrideInvulnerable = false;
}

void ABot::Dead(ASnakePlayerState* Killer)
{
	ABaseGameMode* GameMode = GetGameMode();
	if (GameMode)
	{
		GameMode->BotKilled(Points, Killer);

		AGameMode_Arcade* Arcade = Cast<AGameMode_Arcade>(GameMode);
		if (Arcade)
		{
			Arcade->AddBotKill((uint8)BotClass);
		}
	}

	ABaseAIController* Control = Cast<ABaseAIController>(Controller);
	if (Control)
	{
		Control->BotDead(Killer);
	}

	if (DeathSFXComponent)
	{
		DeathSFXComponent->Play();
	}

	//if (FloatyText)
	//{
	//	FloatyText->InitializeFloatyText(nullptr, Points, GetActorLocation());
	//}

	SpawnLoot();

	GetWorldTimerManager().SetTimer(DissolveDelay_TimerHandle, this, &ABot::StartDissolve, DissolveStartDelay, false);

	OnBotDead();

	HideAndDestroy();
}

bool ABot::IsDead()
{
	return bDead;
}

void ABot::DestroyBot()
{
	if (!bDead)
	{
		Dead(nullptr);
	}
}

void ABot::HideAndDestroy()
{
	bDead = true;

	if (Weapon)
	{
		Weapon->StartDestroy();
	}

	AmbientSFXComponent->Stop();

	GetController() ? Cast<ABaseAIController>(GetController())->StopAI() : nullptr;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UDestructibleMesh* DMesh = DestructibleComponent->GetDestructibleMesh();
	if (!DMesh)
	{
		//Hide the actor and set the life span
		SetActorHiddenInGame(true);
		SetLifeSpan(DestroyTime);
	}
	else
	{
		if (GetMesh()->SkeletalMesh)
		{
			//Expose the Destructble Mesh and reenable its collision
			DestructibleComponent->SetHiddenInGame(false);
			DestructibleComponent->SetCollisionProfileName("Des");

			//Hide the Skeletal Mesh
			GetMesh()->SetHiddenInGame(true);
		}

		//Destroy the Destructible Component
		DestructibleComponent->ApplyRadiusDamage(500, GetActorLocation(), 500, 10, true);
		OnHideAndDestroy(DestroyTime);
	}
}

void ABot::OnHideAndDestroy_Implementation(float InLifeSpan)
{
	SetLifeSpan(InLifeSpan);
}

void ABot::ApplySlow(FSpeedPower Slow)
{
	if (CanApplySlow())
	{
		Slow.bIsActive = true;
		SlowModifiers.Add(Slow);

		ApplyMovementChange();
	}
}

bool ABot::IsSlowed() const
{
	return SlowModifiers.Num() > 0;
}

float ABot::GetSlowModifier(bool Raw)
{
	float Slow = 0.0f;
	for (int32 c = 0; c < SlowModifiers.Num(); c++)
	{
		Slow += SlowModifiers[c].SpeedModifier;
	}

	if (Slow > MaxSlowModifier)
	{
		Slow = MaxSlowModifier;
	}

	return Raw ? Slow : BaseSlowModifier - Slow;
}

bool ABot::CanApplySlow()
{
	return GetSlowModifier(true) < MaxSlowModifier;
}

void ABot::ApplyMovementChange()
{
	float Slow = GetSlowModifier();
	float Movement = BaseMovementSpeed * Slow;

	GetCharacterMovement()->MaxWalkSpeed = Movement;
}

ABaseGameMode* ABot::GetGameMode()
{
	return GetWorld()->GetAuthGameMode() ? Cast<ABaseGameMode>(GetWorld()->GetAuthGameMode()) : nullptr;
}

void ABot::ServerReduceHealth_Implementation(int32 Damage, ASnakePlayerState* DamageCauser)
{
	ReduceHealth(Damage, DamageCauser);
}

void ABot::MultiCastRemoveCollision_Implementation()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

AFloatyTextManager* ABot::GetFloatyText()
{
	for (TActorIterator<AFloatyTextManager> Iter(GetWorld()); Iter; ++Iter)
	{
		if (*Iter)
		{
			return (*Iter);
		}
	}

	return nullptr;
}

FAttackType ABot::GetAttackType()
{
	if (ensure(AttackTypeDataAsset) == false)
	{
		return FAttackType(EAttackMode::Attack05_White);
	}

	return AttackTypeDataAsset->Data;
}

void ABot::SetDMIColor()
{
	if (DMI.Num() > 0)
	{
		for (int32 c = 0; c < DMI.Num(); c++)
		{
			if (DMI[c])
			{
				DMI[c]->SetVectorParameterValue(PrimaryColorName, GetAttackType().ColorData.PrimaryColor);
				DMI[c]->SetVectorParameterValue(SecondaryColorName, GetAttackType().ColorData.SecondaryColor);
				DMI[c]->SetVectorParameterValue(FractureColorName, GetAttackType().ColorData.PrimaryColor);
			}
		}
	}
}

void ABot::StartDissolve()
{
	if (!bDissolveInprogress)
	{
		bDissolveInprogress = true;
	}
}

void ABot::SetDissolveBlend()
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

void ABot::StartSpawnEffect()
{
	SpawnEffectVFX->ActivateSystem();

	if (bPlaySpawnSFX)
	{
		SpawnSFXComponent->Play();
	}

	SetSpawnEffectBlend(0.0f);

	bSpawnEffectInprogress = true;
	SpawnEffectProgress = 0.0f;

	SpawnEffectInitialRelativeLocation = SpawnEffectVFX->GetRelativeTransform().GetLocation();

	for (int32 c = 0; c < DMI.Num(); c++)
	{
		DMI[c]->SetScalarParameterValue(ActorHalfHeightName, ActorHalfHeight);
		DMI[c]->SetScalarParameterValue(SpawnOriginOffsetName, SpawnOffset);
		DMI[c]->SetScalarParameterValue(SpawnEffectOutlineWidthName, SpawnEffectOutlineWidth);
	}
}

void ABot::SetSpawnEffectBlend(float Blend)
{
	for (int32 c = 0; c < DMI.Num(); c++)
	{
		DMI[c]->SetScalarParameterValue(SpawnBlendName, Blend);
	}
}

void ABot::SpawnEffectTick()
{
	if (!bSpawnEffectInprogress)
	{
		return;
	}

	float DeltaTime = GetWorld()->GetDeltaSeconds();
	SpawnEffectProgress += DeltaTime;

	if (SpawnEffectProgress >= SpawnEffectDuration)
	{
		FinishSpawnEffect();
	}
	else
	{
		float Blend = SpawnEffectProgress / SpawnEffectDuration;
		float SpawnBlend = FMath::Lerp(0.0f, 1.0f, Blend);
		SetSpawnEffectBlend(SpawnBlend);

		FVector Start = SpawnEffectInitialRelativeLocation + FVector(0.0f, 0.0f, ActorHalfHeight);
		FVector Finish = SpawnEffectInitialRelativeLocation - FVector(0.0f, 0.0f, ActorHalfHeight);

		FVector RelativeLocation = FMath::Lerp(Start, Finish, Blend);
		SpawnEffectVFX->SetRelativeLocation(RelativeLocation);
	}

	bOverrideInvulnerable = false;
}

void ABot::FinishSpawnEffect()
{
	SpawnEffectVFX->DeactivateSystem();

	SetSpawnEffectBlend(1.1f);

	SpawnEffectProgress = 0.0f;
	bSpawnEffectInprogress = false;
}

void ABot::SpawnDeathVFX()
{
	if (DeathVFX)
	{
		const FVector MyLocation = GetActorLocation();
		const FRotator MyRotation = GetActorRotation();
		UGameplayStatics::SpawnEmitterAtLocation(this, DeathVFX, MyLocation, MyRotation, true);
	}
}

void ABot::SpawnLoot()
{
	if (LootTable)
	{
		TSubclassOf<AActor> Item;
		bool bCanDrop = LootTable->DropItem(Item);
		if (bCanDrop)
		{
			FActorSpawnParameters Params;
			//Params.bNoCollisionFail = false;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AActor* Spawned = GetWorld()->SpawnActor<AActor>(Item, GetActorLocation(), GetActorRotation(), Params);
			Spawned->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
	}
}

int32 ABot::GetMaxHealth() const
{
	return BaseHealth;
}

void ABot::SetHealth(int32 InHealth)
{
	if (InHealth <= 0)
	{
		return;
	}

	Health = InHealth;
	if (Health >= BaseHealth)
	{
		Health = BaseHealth;
	}
}

int32 ABot::GetCurrentHealth() const
{
	return Health;
}

void ABot::ShowBotDamage()
{
	bShowBotDamage = !bShowBotDamage;
}

void ABot::PlayFireWeaponSFX()
{
	if (FireWeaponSFXComponent && bPlayFireSFX)
	{
		FireWeaponSFXComponent->Play();
	}
}

void ABot::StartAmbientSFX()
{
	if (AmbientSFXComponent && AmbientSFXComponent->Sound)
	{
		AmbientSFXComponent->Play();
	}
}