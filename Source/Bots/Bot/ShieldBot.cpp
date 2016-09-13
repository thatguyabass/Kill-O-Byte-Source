// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "ShieldBot.h"
#include "Bots/Controller/ShieldBotAI.h"
#include "Powers/Shield/ForceField.h"
#include "Utility/AttackTypeDataAsset.h"

AShieldBot::AShieldBot()
	: Super()
{
	SafeZoneComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SafeZoneComponent"));
	SafeZoneComponent->SetCapsuleHalfHeight(100.0f);
	SafeZoneComponent->SetCapsuleRadius(50.0f);
	SafeZoneComponent->AttachTo(RootComponent);

	DeployedSpeedScale = 0.4f;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AShieldBot::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	BaseMovementSpeed = GetCharacterMovement()->MaxWalkSpeed;

	DisableZoneCollision();

	SafeZoneComponent->OnComponentBeginOverlap.AddDynamic(this, &AShieldBot::OnComponentBeginOverlap);
	SafeZoneComponent->OnComponentEndOverlap.AddDynamic(this, &AShieldBot::OnComponentEndOverlap);
}

void AShieldBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ForceField)
	{
		FRotator Rotation = ForceField->GetActorRotation();
		
		Rotation.Pitch = 0.0f;
		Rotation.Roll = 0.0f;

		ForceField->SetActorRotation(Rotation);
	}
}

void AShieldBot::ActivateNullifyingEMP(AActor* Target)
{
	if (!Target || !NullEMPClass)
	{
		return;
	}

	FVector TargetLocation = Target->GetActorLocation();
	FRotator Rotation = FRotator::ZeroRotator;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<ANullifyingEMP>(NullEMPClass, TargetLocation, Rotation, Params);
}

void AShieldBot::DeployShield()
{
	if (ForceField && !ForceField->bIsActive)
	{
		EnableZoneCollision();
		ForceField->ActivateForceField();
		ApplyShieldMovementChange(DeployedSpeedScale);

		GetCharacterMovement()->bOrientRotationToMovement = false;
		AShieldBotAI* AI = Cast<AShieldBotAI>(Controller);
		if (AI)
		{
			AI->RemoveFocus();
		}
	}
}

void AShieldBot::RemoveShield()
{
	if (ForceField && ForceField->bIsActive)
	{
		DisableZoneCollision();
		ForceField->IsShutdown() ? ForceField->bIsActive = false : ForceField->DeactivateForceField();
		ApplyShieldMovementChange();

		GetCharacterMovement()->bOrientRotationToMovement = true;
		AShieldBotAI* AI = Cast<AShieldBotAI>(Controller);
		if (AI)
		{
			AI->FindFocus();
		}
	}
}

void AShieldBot::OnShieldRestart()
{
	AShieldBotAI* AI = Cast<AShieldBotAI>(Controller);
	if (AI)
	{
		AI->SetRetreat(false);
	}
}

void AShieldBot::OnShieldShutdown()
{
	AShieldBotAI* AI = Cast<AShieldBotAI>(Controller);
	if (AI)
	{
		RemoveShield();
		AI->SetRetreat(true);
	}
}

bool AShieldBot::IsShieldShutdown() const
{
	return ForceField->IsShutdown();
}

void AShieldBot::ApplyShieldMovementChange(float InScale)
{
	float Speed = BaseMovementSpeed * InScale;
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void AShieldBot::SpawnWeapon()
{
	if (ForceFieldClass)
	{
		const FVector Location = GetActorLocation();
		const FRotator Rotation = GetActorRotation();

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ForceField = GetWorld()->SpawnActor<AForceField>(ForceFieldClass, Location, Rotation, Params);
		ForceField->SetShieldOwner(this);
		ForceField->SetAttackType(AttackTypeDataAsset->Data);
		ForceField->bCanRegen = true;
		ForceField->bIsActive = false;

		ForceField->OnRestart.BindUObject(this, &AShieldBot::OnShieldRestart);
		ForceField->OnShutdown.BindUObject(this, &AShieldBot::OnShieldShutdown);
	}
}

void AShieldBot::Dead(ASnakePlayerState* Killer)
{
	ABot::Dead(Killer);

	//Disable the Zone Collision to ensure no additional bots get added to the safe zone before this bot is destroyed
	DisableZoneCollision();
	if (ForceField)
	{
		ForceField->HideAndDestroy(false);
	}

	for (int32 c = 0; c < SafeBots.Num(); c++)
	{
		SafeBots[c]->bCanBeDamaged = true;
	}
	SafeBots.Empty();
}

void AShieldBot::OnComponentBeginOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bDead)
	{
		return;
	}

	ABot* Bot = Cast<ABot>(Other);
	//Do not add this bot to the list. 
	if (!Bot || Other == this)
	{
		return;
	}

	bool bFound = false;
	for (int32 c = 0; c < SafeBots.Num(); c++)
	{
		if (Bot == SafeBots[c])
		{
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		//Bots within the safe zone are immune to damage 
		Bot->bCanBeDamaged = false;
		SafeBots.Add(Bot);
	}
}

void AShieldBot::OnComponentEndOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABot* Bot = Cast<ABot>(Other);
	if (!Bot || Other == this)
	{
		return;
	}

	int32 Find = SafeBots.Find(Bot);
	if (SafeBots.IsValidIndex(Find))
	{
		Bot->bCanBeDamaged = true;
		SafeBots.Remove(Bot);
	}
}

void AShieldBot::EnableZoneCollision()
{
	SafeZoneComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SafeZoneComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SafeZoneComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AShieldBot::DisableZoneCollision()
{
	SafeZoneComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SafeZoneComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
}