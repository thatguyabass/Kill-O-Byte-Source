// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SnakeController.h"
#include "SnakeLink.h"
#include "SnakeLinkHead.h"

#include "GameMode/BaseGameMode.h"
#include "Components/FloatyTextManager.h"

#include "Powers/Weapons/Weapon_Combo.h"
#include "Powers/Shield/Repulsion.h"
#include "Powers/Projectiles/BlastProjectile.h"
#include "Utility/Attributes.h"

const int32 ASnakeController::MaxPowerLevel = 2;
const int32 ASnakeController::NoPowerLevel = -1;

const float ASnakeController::BaseSlowModifier = 1.0f;
const float ASnakeController::BaseSpeedModifier = 1.0f;

ASnakeController::ASnakeController()
	: Super()
{
	bFireHeld = false;
	FireOffset = FVector(100.0f, 0.0f, 0.0f);
	
	PrimaryActorTick.bCanEverTick = true;
	MaxSlowModifier = 0.5f;
	MaxSpeedModifier = 0.85f;
	TeleportDistance = 800.0f;

	PowerType = EPowerType::None;
	PowerLevel = NoPowerLevel;

	StartingBodyCount = 2;

	static ConstructorHelpers::FObjectFinder<UAttributes> AttributeObject(TEXT("/Game/Blueprints/Attributes/Attributes"));
	Attributes = AttributeObject.Object;

	Health = 0;
	HealthTotal = 0;
	BaseHealthPerLink = 2;
	bDead = false;

	bLastStand = false;

	bReplicates = true;
}

void ASnakeController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	HealthPerLink = Attributes->CalculateHealth(BaseHealthPerLink);
}

void ASnakeController::CleanAndDestroy()
{
	if (Snake.Num() > 1)
	{
		CleanUpBody();
		Snake.Reset();
	}
	SetLifeSpan(2.0f);
}

void ASnakeController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SlowPowers.Num() > 0)
	{
		bool bChange = false;
		for (int32 c = 0; c < SlowPowers.Num(); c++)
		{
			FSpeedPower& Slow = SlowPowers[c];
			if (Slow.bIsActive)
			{
				Slow.Progress += DeltaTime;
				if (Slow.Progress > Slow.Duration)
				{
					Slow.bIsActive = false;
					Slow.Progress = 0.0f;

					//Flag that a slow has been removed
					bChange = true;

					SlowPowers.RemoveAt(c);
					//Reset the index for the next item.
					c--;
				}
			}
		}

		if (bChange)
		{
			ApplyMovementChange();
		}
	}

	if (SpeedPower.bIsActive)
	{
		SpeedPower.Progress += DeltaTime;
		if (SpeedPower.Progress > SpeedPower.Duration)
		{
			SpeedPower.bIsActive = false;

			//Reset the Speed back to the default values
			ApplyMovementChange();
		}
	}

	if (SpeedBoostPower.bIsActive)
	{
		SpeedBoostPower.Progress += DeltaTime;
		if (SpeedBoostPower.Progress > SpeedBoostPower.Duration)
		{
			SpeedBoostPower.bIsActive = false;

			ApplyMovementChange();
		}
	}

	if (ShieldPower.bIsActive)
	{
		ShieldPower.Progress += DeltaTime;
		if (ShieldPower.Progress > ShieldPower.Duration)
		{
			ShieldPower.bIsActive = false;
			ShieldPower.ClearFields();

			ApplyShieldChange();
		}
	}

	if (bFireHeld && PrimaryWeapon && PrimaryWeapon->CanFire())
	{
		FirePrimary();
	}

	// Check if the player has picked up a body since last tick
	if (bLastStand && Snake.Num() > 1)
	{
		bLastStand = false;
	}

	bLastStandLastTick = bLastStand;
}

void ASnakeController::SetHead(ASnakeLink* NewHead)
{
	Head = NewHead;
	Snake.Add(Head);
	Head->SetOwner(this);

	//Player has a head! How could they be dead 
	bDead = false;

	bFireHeld = false;
	bLastStand = false;

	Head->AdjustMovementSpeed();

	Health = 0;
	HealthTotal = 0;
}

ASnakeLink* ASnakeController::GetHead()
{
	return Head;
}

FTeamColorData& ASnakeController::GetTeamColorData()
{
	return ColorData;
}

void ASnakeController::SetTeamColorData(FTeamColorData InColorData)
{
	ColorData = InColorData;
}

int32 ASnakeController::GetTeamNumber()
{
	ASnakePlayerState* State = GetPlayerState();
	return State ? GetPlayerState()->GetTeamNumber() : -1;
}

void ASnakeController::SpawnBody()
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (BodyClass && CanSpawnBody())
		{
			//FTransform Trans = Snake.Last()->SpawnLocation->GetComponentTransform();

			//FVector Location = Trans.GetLocation();
			//FRotator Rotation = Trans.GetRotation().Rotator();

			//ServerSpawnBody(Location, Rotation);
		}
	}
}

void ASnakeController::AddBody(ASnakeLink* Link)
{
	if (!CanSpawnBody())
	{
		return;
	}

	if (Role < ROLE_Authority)
	{
		return;
	}

	//Add a Reference of this Snake Controller
	//Link->SnakeController = this;

	AddToTotalHealth(Link->GetHealth());

	//Set the Links Target to Follow
	Link->SetTarget(Snake.Last());

	//Teleport to the Target 
	TeleportLinkToTarget(Link);

	//Add this Link to the Snake
	Snake.Add(Link);

	//Reset the Current State of the Link to Body
	Link->SetCurrentState(ESnakeState::Body);

	//Link->UpdateTeamColors();

	//Reset the Movement States
	Link->GlobalSpeedModifier = Head->GlobalSpeedModifier;

	//Apply any Movement modifiers that the Snake may have active
	Link->AdjustMovementSpeed();

	//Apply any Shields the Snake may have active	
	if (ShieldPower.bIsActive && ShieldPower.PowerLevel == MaxPowerLevel)
	{
		SpawnRepulsionField(Link);
	}
	//Link->SetShield(ShieldPower);

	IncreaseBodyCount();
}

void ASnakeController::IncreaseBodyCount()
{
	CurrentBodyCount++;
	if (CurrentBodyCount >= MaxBodyCount)
	{
		CurrentBodyCount = MaxBodyCount;
	}
}
void ASnakeController::DecreaseBodyCount()
{
	CurrentBodyCount--;
	if (CurrentBodyCount < 0)
	{
		CurrentBodyCount = 0;
	}
}

bool ASnakeController::CanSpawnBody()
{
	return CurrentBodyCount < MaxBodyCount && CurrentBodyCount >= 0;
}

void ASnakeController::CleanUpBody()
{
	bFireHeld = false;
	for (auto Link : Snake)
	{
		//The head is destroyed before this was called
		if (!Link->IsHead())
		{
			CleanLink(Link);
			Link->HideAndDestroy();
		}
	}

	CleanUpWeapons();

	Snake.Empty();
	Head = nullptr;
	Health = 0;
	HealthTotal = 0;
}

void ASnakeController::RemoveBody(ASnakeLink* Link, ECutType Type, ASnakePlayerState* Damager)
{
	if (Role < ROLE_Authority)
	{
		ServerRemoveBody(Link, Type, Damager);
	}
	else
	{
		if (Link)
		{
			switch (Type)
			{
			case ECutType::Cut:
				CutLink(Link, Damager);
				break;
			case ECutType::Magnet:
				AdjustSnake(Link);
				break;
			}

			ScoreLinkKill(Damager);
			//FloatyTextLink(Link->GetActorLocation());

			CleanLink(Link);
			Link->HideAndDestroy();
		}
	}
}

bool ASnakeController::CheckFirstLink(ASnakeLink* Link)
{
	if (Snake.Num() <= 0)
		return false;
	else if (Snake.IsValidIndex(1) && Snake[1] == Link)
		return true;
	else
		return false;
}

void ASnakeController::TriggerRemoveBody(int32 Index, ECutType Type)
{
	RemoveBody(Snake[Index], Type, nullptr);
}

void ASnakeController::CutLink(ASnakeLink* Link, ASnakePlayerState* Damager)
{
	if (Snake.Contains(Link))
	{
		int32 LinkIndex = Snake.Find(Link);

		int32 SnakeLength = Snake.Num();

		for (int32 c = Snake.Num() - 1; c >= LinkIndex; c--)
		{
			ASnakeLink* Link = Snake[c];

			//Link->SetCurrentState(ESnakeState::Frozen);
			Link->Target = nullptr;

			Snake.RemoveAt(c);
		}

		if (SnakeLength != Snake.Num())
		{
			//Re-evaluate health
			ReevaluateHealth(SnakeLength, Damager);
		}
	}
}

void ASnakeController::AdjustSnake(ASnakeLink* Link)
{
	if (Snake.Contains(Link))
	{
		int32 LinkIndex = Snake.Find(Link);
		ASnakeLink* Target = Link->GetTarget();

		bool Last = (Snake.Last() == Link);
		Snake.Remove(Link);

		ReduceHealthTotal();

		if (!Last)
		{
			Snake[LinkIndex]->SetTarget(Target);
		}
	}
}

void ASnakeController::CleanLink(ASnakeLink* Link)
{
	if (Role < ROLE_Authority)
	{
		return;
	}
	else
	{
		//for (auto Attached : Link->AttachedProjectiles)
		//{
		//	if (Attached)
		//	{
		//		Attached->CleanProjectile();
		//	}
		//}
		//Link->AttachedProjectiles.Empty();
	
		RemoveRepulsionField(Link);

		DecreaseBodyCount();
	}
}

void ASnakeController::ApplyMovementChange()
{
	for (auto Link : Snake)
	{
		Link->AdjustMovementSpeed();
	}
}

void ASnakeController::ApplyShieldChange()
{
	for (auto Link : Snake)
	{
		//Link->SetShield(ShieldPower);
	}
}

////////////////////////////////////////////////////////////////
////// Health Methods

void ASnakeController::ReduceHealthTotal()
{
	if (Role < ROLE_Authority)
	{
		ServerReduceHealthTotal();
	}
	else
	{
		if (HealthPerLink < 0)
			return;

		HealthTotal -= HealthPerLink;
	}
}

void ASnakeController::ReduceHealth(int32 Damage, ASnakePlayerState* Damager)
{
	if (Role < ROLE_Authority)
	{
		ServerReduceHealth(Damage, Damager);
	}
	else
	{
		if (Damage < 0)
			return;

		Health -= Damage;
		
		if (Health <= 0)
		{
			Killed(Damager);
		}
	}
}

void ASnakeController::AddToTotalHealth(int32 InHealth)
{
	if (Role < ROLE_Authority)
	{
		ServerAddToTotalHealth(InHealth);
	}
	else
	{
		if (HealthPerLink < 0)
			return;

		Health += InHealth;
		HealthTotal += HealthPerLink;
	
		check(HealthTotal >= Health);
		if (Health > HealthTotal)
		{
			Health = HealthTotal;
		}
	}
}

int32 ASnakeController::GetHealth() const
{
	return Health;
}

int32 ASnakeController::GetHealthTotal() const
{
	return HealthTotal;
}

int32 ASnakeController::GetHealthPerLink() const
{
	return HealthPerLink;
}

void ASnakeController::Killed(ASnakePlayerState* Killer)
{
	if (Role < ROLE_Authority)
	{
		ServerKilled(Killer);
	}
	else
	{
		if (!IsDead())
		{
			//Turn off all powers
			DeactivatePowers();
			//Remove a repulsion field from the head
			RemoveRepulsionField(Head);

			ScoreSnakeKill(Killer);
			FloatyTextHead(Head->GetActorLocation());

			Head->DetachFromControllerPendingDestroy();
			Head->HideAndDestroy();
			bDead = true;
		}
	}
}

bool ASnakeController::IsDead()
{
	return bDead;
}

void ASnakeController::ScoreSnakeKill(ASnakePlayerState* Killer)
{
	ABaseGameMode* GameMode = GetGameMode();
	ASnakePlayerState* PlayerState = GetPlayerState();
	if (GameMode)
	{
		GameMode->SnakeKilled(Killer, PlayerState);

		if (PlayerState)
		{
			PlayerState->ReducePlayerLives();
		}
	}
}

void ASnakeController::ScoreLinkKill(ASnakePlayerState* Killer)
{
	ABaseGameMode* GameMode = GetGameMode();
	if (GameMode)
	{
		GameMode->LinkKilled(Killer);
	}
}

void ASnakeController::FloatyTextLink(FVector WorldLocation)
{
	ABaseGameMode* GameMode = GetGameMode();
	if (GameMode)
	{
		int32 Score = GameMode->GetLinkPoints();
		CreateFloatyText(Score, WorldLocation);
	}
}

void ASnakeController::FloatyTextHead(FVector WorldLocation)
{
	ABaseGameMode* GameMode = GetGameMode();
	if (GameMode)
	{
		int32 Score = GameMode->GetDeathPoints();
		CreateFloatyText(Score, WorldLocation);
	}
}

void ASnakeController::CreateFloatyText(int32 Score, FVector WorldLocation)
{
	AFloatyTextManager* FloatyText = GetFloatyText();
	if (FloatyText)
	{
		FloatyText->InitializeFloatyText(nullptr, Score, WorldLocation);
	}
}

AFloatyTextManager* ASnakeController::GetFloatyText()
{
	for (TActorIterator<AFloatyTextManager> FloatyText(GetWorld()); FloatyText; ++FloatyText)
	{
		if (*FloatyText)
		{
			return (*FloatyText);
		}
	}

	return nullptr;
}

void ASnakeController::ReevaluateHealth(int32 PreLength, ASnakePlayerState* Damager)
{
	if (Role < ROLE_Authority)
	{
		ServerReevaluateHealth(PreLength, Damager);
	}
	else
	{
		int32 NewTotal = (Snake.Num() - 1) * HealthPerLink;
		int32 NewHealth = 0;
		for (int32 c = 1; c < Snake.Num(); c++)
		{
			NewHealth += Snake[c]->GetHealth();
		}

		if (NewHealth <= 0)
		{
			//Enter Last Stand mode
			if (PreLength > 3 && !bLastStand)
			{
				bLastStand = true;
			}
			else
			{
				Killed(Damager);
			}
		}

		if(!bDead)
		{
			HealthTotal = NewTotal;
			Health = NewHealth;
		}
	}
}

bool ASnakeController::GetLastStand()
{
	return bLastStand == bLastStandLastTick;
}

////////////////////////////////////////////////////////////////
////// Power Type Methods 

void ASnakeController::SetPowerType(EPowerType Type)
{
	if (PowerType != Type)
		PowerLevel = NoPowerLevel;

	PowerType = Type;

	if (Role < ROLE_Authority)
	{
		ServerSetPowerType(Type);
	}

	HandleNewType();
}

EPowerType ASnakeController::GetPowerType()
{
	return PowerType;
}

int32 ASnakeController::GetPowerLevel()
{
	return PowerLevel;
}

void ASnakeController::HandleNewType()
{
	switch (PowerType)
	{
		case EPowerType::None: break;
		default:
		{
			PowerLevel++;
			if (PowerLevel > MaxPowerLevel)
				PowerLevel = MaxPowerLevel;
			break;
		}
	}
}

bool ASnakeController::IsPowerLevelInvalid()
{
	return (PowerLevel <= NoPowerLevel);
}

void ASnakeController::DeactivatePowers(bool bApplyMovement)
{
	if (SpeedPower.bIsActive)
	{
		SpeedPower.bIsActive = false;
	}

	if (SpeedBoostPower.bIsActive)
	{
		SpeedBoostPower.bIsActive = false;
	}

	if (SlowPowers.Num() > 0)
	{
		for(FSpeedPower Slow : SlowPowers)
		{
			if (Slow.bIsActive)
			{
				Slow.bIsActive = false;
			}
		}
		SlowPowers.Reset();
	}

	//Should we apply the movement change?
	if (bApplyMovement)
	{
		ApplyMovementChange();
	}

	if (ShieldPower.bIsActive)
	{
		ShieldPower.bIsActive = false;
	}
	ApplyShieldChange();
}

////////////////////////////////////////////////////////////////
////// Projectile Power Methods

bool ASnakeController::FireSecondaryWeapon()
{
	//Check if a Projectile has been fired
	bool bFired = false;

	if (SecondaryWeapon->CanFire())
	{
		UWorld* World = GetWorld();
		if (World)
		{
			bFired = true;

			int32 Offset = 0;
			switch (PowerType)
			{
			case EPowerType::Bomb: Offset = 0; break;
			case EPowerType::Build: Offset = 3; break;
			case EPowerType::Blast: Offset = 6; break;
			}

			/*Weapon*/
			SecondaryWeapon->Fire(FireOffset, PowerLevel + Offset);

			if (PowerType == EPowerType::Blast)
			{
				//Reset bFired, this will prevent the Power State from being overwritten.
				bFired = false;
				//Set the Power Type to the Blast Secondary. This will take over the secondary effects 
				SetPowerType(EPowerType::BlastSecondary);
			}
		}
	}

	return bFired;
}

void ASnakeController::FirePrimary()
{
	if (PrimaryWeapon && PrimaryWeapon->CanFire())
	{
		PrimaryWeapon->Fire(FireOffset);
	}

	//Flag that the Fire button is down
	bFireHeld = true;
}

void ASnakeController::FireSecondary()
{
	//Ensure the Power Level is Valid. -1 will cause out of bounds arrays
	if (IsPowerLevelInvalid())
	{
		return;
	}

	if (Role < ROLE_Authority)
	{
		ServerFireSecondary();
	}
	else
	{
		//If True, reset the power type at the end of the method. If false, skip the reset.
		//This is false when the user has activated a power that is lower then the Power already in affect. (Speed level 2 active while speed level 1 is ready to go)
		bool Activated = true;

		switch (PowerType)
		{
		case EPowerType::None: /* Do Nothing. No Power was picked up */ break;
		case EPowerType::Bomb: // Bleed into Build. 
		case EPowerType::Blast: // Bleed Into Build.
		case EPowerType::Build: Activated = FireSecondaryWeapon(); break;
		case EPowerType::BlastSecondary: Activated = ActivateBlastSecondary(); break;
		case EPowerType::Speed: Activated = ActivateSpeed(); break;
		case EPowerType::Shield: Activated = ActivateShield(); break;
		}

		if (Activated)
		{
			//Reset the Powers After Use
			SetPowerType(EPowerType::None);
		}
	}
}

void ASnakeController::FireReleased()
{
	bFireHeld = false;
}

bool ASnakeController::ActivateBlastSecondary()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(this, ABlastProjectile::StaticClass(), FoundActors);

	for (int32 c = 0; c < FoundActors.Num(); c++)
	{
		ABaseProjectile* Blast = Cast<ABlastProjectile>(FoundActors[c]);
		if (Blast && Blast->ProjectileOwner && Blast->ProjectileOwner == Head)
		{
			FHitResult Hit(ForceInit);
			Blast->OnImpact(Hit);
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////
////// Slow Modifiers Methods 

void ASnakeController::AddSlowModifier(FSpeedPower SlowPower)
{
	SlowPowers.Add(SlowPower);
	SlowPowers.Last().bIsActive = true;

	ApplyMovementChange();
}

float ASnakeController::GetSlowModifier()
{
	float SlowTotal = GetSlowModifierRaw();

	return BaseSlowModifier - SlowTotal;
}

float ASnakeController::GetSlowModifierRaw()
{
	float SlowTotal = 0.0f;

	if (SlowPowers.Num() > 0)
	{
		for (int32 c = 0; c < SlowPowers.Num(); c++)
		{
			if (SlowPowers[c].bIsActive)
			{
				SlowTotal += SlowPowers[c].SpeedModifier;
			}
		}
	}

	if (StasisPower.bIsActive)
	{
		//Add the Stasis fields slow value 
		SlowTotal += StasisPower.SpeedModifier;
	}

	//Ensure the Slow doesn't exceed the Max
	if (SlowTotal > MaxSlowModifier)
	{
		SlowTotal = MaxSlowModifier;
	}

	return SlowTotal;
}

void ASnakeController::AddStasisSlow(FSpeedPower StasisSlow)
{
	StasisPower = StasisSlow;
	StasisPower.bIsActive = true;

	ApplyMovementChange();
}

void ASnakeController::RemoveStasisSlow()
{
	StasisPower.bIsActive = false;

	ApplyMovementChange();
}

////////////////////////////////////////////////////////////////
////// Speed Modifiers Methods 

void ASnakeController::SetSpeedPower(FSpeedPower Power)
{
	SpeedPower = Power;
}

void ASnakeController::SetSpeedBoostPower(FSpeedPower Power)
{
	SpeedBoostPower = Power;

	SpeedBoostPower.bIsActive = true;

	ApplyMovementChange();
}

float ASnakeController::GetSpeedModifier() const
{
	float Speed = (SpeedPower.bIsActive ? SpeedPower.SpeedModifier : 0.0f) + (SpeedBoostPower.bIsActive ? SpeedBoostPower.SpeedModifier : 0.0f);
	if (Speed > MaxSpeedModifier)
	{
		Speed = MaxSpeedModifier;
	}

	return BaseSpeedModifier + Speed;
}

FSpeedPower& ASnakeController::GetSpeedPower()
{
	return SpeedPower;
}

bool ASnakeController::ActivateSpeed()
{
	if (!CanActivateSpeed())
	{
		return false;
	}

	//Apply Speed Stuff
	SpeedPower = SpeedPowerTiers[PowerLevel];
	SpeedPower.bIsActive = true;

	if (PowerLevel == MaxPowerLevel)
	{
		ActivateTeleport();
	}

	//Apply the Movement Increase to the Entire Snake
	ApplyMovementChange();

	return true;
}

bool ASnakeController::CanActivateSpeed() const
{
	if (SpeedPower.bIsActive)
	{
		//Cancel the new Speed if the Current Speed modifier is larger
		if (SpeedPower.SpeedModifier > SpeedPowerTiers[PowerLevel].SpeedModifier)
			return false;
	}

	return true;
}

void ASnakeController::ActivateTeleport()
{
	ASnakeLink* Link = GetHead();
	if (Link)
	{
		FVector Location = Link->GetActorLocation();
		FVector LocationOffset = Link->GetActorForwardVector() * TeleportDistance;
		FVector Target = Location + LocationOffset;
		FRotator Rotation = Link->GetActorRotation();

		Link->TeleportTo(Target, Rotation, false, false);
		//Link->OnRep_JustTeleported();

		for (int32 c = 1; c < Snake.Num(); c++)
		{
			TeleportLinkToTarget(Snake[c]);
		}
	}
}

void ASnakeController::TeleportLinkToTarget(ASnakeLink* Link)
{
	if (Link && Link->GetTarget())
	{
		const FVector Location = Link->GetActorLocation();
		FVector Target = Link->GetTarget()->GetActorLocation();
		const FRotator Rotation = Link->GetActorRotation();

		FVector Direction = Location - Target;
		Direction.Normalize();

		const FVector LocationOffset = Direction * Link->IdealDistance;

		const FVector OriginalTarget = Target;
		Target += LocationOffset;

		bool bHit = GetWorld()->EncroachingBlockingGeometry(Link, Target, Rotation);
		if (bHit)
		{
			Target = OriginalTarget;
			//Link->MultiCastInvulnerableFrames();
		}

		Link->TeleportTo(Target, Rotation, false, true);
		//Link->OnRep_JustTeleported();
	}
}

////////////////////////////////////////////////////////////////
////// Shield Methods 

bool ASnakeController::IsShieldActive()
{
	return (ShieldPower.bIsActive);
}

FShieldPower& ASnakeController::GetShieldPower()
{
	return ShieldPower;
}

bool ASnakeController::ActivateShield()
{
	if (!CanActivateShield())
	{
		return false;
	}

	// Initialize attached flag. 
	bool bFieldsAttached = false;
	TArray<ARepulsion*> Fields;
	// Check if this controller has spawned fields that are still active. Level 2 Shields will drop them in place.
	if (ShieldPower.bIsActive && ShieldPower.RepulsionFields.Num() > 0)
	{
		//Was the last power the max level?
		if (ShieldPower.PowerLevel == MaxPowerLevel)
		{
			//Flag that shields are attached and don't need to be respawned
			bFieldsAttached = true;
			//Store them
			Fields.Append(ShieldPower.RepulsionFields);
		}
		else
		{
			//Trigger the internal timers to clean them up after the duration has been reached
			ShieldPower.TriggerTimers();
		}
	}

	//Apply the New Shield
	ShieldPower = ShieldPowerTier[PowerLevel];
	ShieldPower.PowerLevel = PowerLevel;
	ShieldPower.bIsActive = true;

	// Append the fields that already exist. Needed for Garbage Collection when the Shield Expires
	ShieldPower.RepulsionFields.Append(Fields);

	if (ShieldPower.CanSpawnField() && !bFieldsAttached)
	{
		for (int32 c = 0; c < Snake.Num(); c++)
		{
			SpawnRepulsionField(Snake[c]);
		}
	}

	//Apply the New Shield to the Snake
	ApplyShieldChange();

	return true;
}

bool ASnakeController::CanActivateShield() const
{
	if (ShieldPower.bIsActive)
	{
		if (ShieldPower.PowerLevel > PowerLevel)
		{
			return false;
		}
	}

	return true;
}

void ASnakeController::SpawnRepulsionField(ASnakeLink* Link)
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	if (GetWorld())
	{
		FTransform SpawnTrans = Link->GetTransform();
		ARepulsion* Field = Cast<ARepulsion>(UGameplayStatics::BeginSpawningActorFromClass(this, ShieldPower.RepulsionFieldClass, SpawnTrans));

		Field->SetOwner(Head);
		Field->RepulsionOwner = Link;

		if (ShieldPower.PowerLevel == MaxPowerLevel)
		{
			Field->AttachRootComponentToActor(Link, NAME_None, EAttachLocation::KeepWorldPosition, false);
		}

		UGameplayStatics::FinishSpawningActor(Field, SpawnTrans);

		ShieldPower.RepulsionFields.Add(Field);
	}
}

void ASnakeController::RemoveRepulsionField(ASnakeLink* Link)
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	if (Link)
	{
		//ARepulsion* Repel = Link->ShieldPower.GetField(Link);
		//if (Repel)
		//{
		//	Repel->HideAndDestroy();
		//	ShieldPower.RepulsionFields.Remove(Repel);
		//}
	}
}

////////////////////////////////////////////////////////////////
////// Helper Methods

ABaseGameMode* ASnakeController::GetGameMode() const
{
	return GetWorld()->GetAuthGameMode() ? Cast<ABaseGameMode>(GetWorld()->GetAuthGameMode()) : nullptr;
}

ASnakePlayerState* ASnakeController::GetPlayerState() const
{
	return (Head && Head->PlayerState) ? Cast<ASnakePlayerState>(Head->PlayerState) : nullptr;
}

////////////////////////////////////////////////////////////////
////// Weapon Methods

void ASnakeController::SpawnWeapons()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (World)
	{
		PrimaryWeapon = Cast<AWeapon>(GetWorld()->SpawnActor(PrimaryWeaponClass));
		SecondaryWeapon = Cast<AWeapon_Combo>(GetWorld()->SpawnActor(SecondaryWeaponClass));

		PrimaryWeapon->SetWeaponOwner(Head);
		StoredWeapon = PrimaryWeapon;
		SecondaryWeapon->SetWeaponOwner(Head);

		// Replicate the Weapons
		EquipPrimary(PrimaryWeapon);
		EquipSecondary(SecondaryWeapon);
	}
}
void ASnakeController::EquipPrimary(AWeapon* Primary)
{
	if (Primary)
	{
		if (Role < ROLE_Authority)
		{
			ServerEquipPrimaryWeapon(Primary);
		}
		else
		{
			SetPrimary(Primary);
		}
	}
}

void ASnakeController::EquipSecondary(AWeapon_Combo* Secondary)
{
	if (Secondary)
	{
		if (Role < ROLE_Authority)
		{
			ServerEquipSecondaryWeapons(Secondary);
		}
		else
		{
			SetSecondary(Secondary);
		}
	}
}

void ASnakeController::OnRep_PrimaryWeapon()
{
	SetPrimary(PrimaryWeapon);
}

void ASnakeController::SetPrimary(AWeapon* NewWeapon)
{
	if (NewWeapon)
	{
		PrimaryWeapon = NewWeapon;
		PrimaryWeapon->SetWeaponOwner(Head);
	}
}

void ASnakeController::SetSecondary(AWeapon_Combo* Secondary)
{
	if (Secondary)
	{
		SecondaryWeapon = Secondary;
	}
}

void ASnakeController::ChangeWeapon(AWeapon* InWeapon, float Duration)
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	if (Duration > 0)
	{
		ClearTimer(WeaponReset_TimerHandle);
		GetWorld()->GetTimerManager().SetTimer(WeaponReset_TimerHandle, this, &ASnakeController::ResetPrimaryWeapon, Duration, false);
	}

	EquipPrimary(InWeapon);
}

void ASnakeController::CleanUpWeapons()
{
	if (Role == ROLE_Authority)
	{
		PrimaryWeapon->StartDestroy();
		SecondaryWeapon->StartDestroy();

		ClearTimer(WeaponReset_TimerHandle);
	}
}

void ASnakeController::ResetPrimaryWeapon()
{
	ChangeWeapon(StoredWeapon, -1);
}

////////////////////////////////////////////////////////////////
//////Server Client and NetMultiCast Methods 

void ASnakeController::ServerSpawnBody_Implementation(FVector Origin, FRotator Rotation)
{
	if (BodyClass && CanSpawnBody())
	{
		FTransform SpawnTrans(Rotation, Origin);
		ASnakeLink* NewTail = Cast<ASnakeLink>(UGameplayStatics::BeginSpawningActorFromClass(this, BodyClass, SpawnTrans, true));
		if (NewTail)
		{
			//NewTail->SetOwner(this);
			NewTail->Instigator = Instigator;
			//NewTail->SnakeController = this;
			NewTail->SetHealth(HealthPerLink);

			UGameplayStatics::FinishSpawningActor(NewTail, SpawnTrans);

			NewTail->SetTarget(Snake.Last());
			NewTail->SetCurrentState(ESnakeState::Body);
			//NewTail->UpdateTeamColors();

			for (int32 c = 0; c < Snake.Num(); c++)
			{
				NewTail->MoveIgnoreActorAdd(Snake[c]);
			}

			Snake.Add(NewTail);

			//Increase the Total Health of the Snake
			AddToTotalHealth(HealthPerLink);

			//Apply any Movement Modifiers that the Snake May have active
			NewTail->AdjustMovementSpeed();
			//Apply any Shield Effects that the Snake May have active
			if (ShieldPower.bIsActive && ShieldPower.PowerLevel == MaxPowerLevel)
			{
				SpawnRepulsionField(NewTail);
			}
			//NewTail->SetShield(ShieldPower);

			IncreaseBodyCount();
		}
	}
}

void ASnakeController::ServerRemoveBody_Implementation(ASnakeLink* Link, ECutType Type, ASnakePlayerState* Damager)
{
	RemoveBody(Link, Type, Damager);
}

void ASnakeController::ServerSetPowerType_Implementation(EPowerType Type)
{
	SetPowerType(Type);
}

void ASnakeController::ServerFireSecondary_Implementation()
{
	FireSecondary();
}

void ASnakeController::ServerReduceHealthTotal_Implementation()
{
	ReduceHealthTotal();
}

void ASnakeController::ServerReduceHealth_Implementation(int32 Damage, ASnakePlayerState* Damager)
{
	if (Damage < 0)
		return;

	ReduceHealth(Damage, Damager);
}

void ASnakeController::ServerAddToTotalHealth_Implementation(int32 InHealth)
{
	AddToTotalHealth(InHealth);
}

void ASnakeController::ServerKilled_Implementation(ASnakePlayerState* Killer)
{
	if (!IsDead())
	{
		Killed(Killer);
	}
}

void ASnakeController::ServerReevaluateHealth_Implementation(int32 PreLength, ASnakePlayerState* Damager)
{
	ReevaluateHealth(PreLength, Damager);
}

void ASnakeController::ServerEquipPrimaryWeapon_Implementation(AWeapon* Primary)
{
	EquipPrimary(Primary);
}

void ASnakeController::ServerEquipSecondaryWeapons_Implementation(AWeapon_Combo* Secondary)
{
	EquipSecondary(Secondary);
}

void ASnakeController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASnakeController, Snake);
	DOREPLIFETIME(ASnakeController, Head);

	DOREPLIFETIME(ASnakeController, Health);
	DOREPLIFETIME(ASnakeController, HealthTotal);
	DOREPLIFETIME(ASnakeController, bDead);

	DOREPLIFETIME(ASnakeController, PowerLevel);
	DOREPLIFETIME(ASnakeController, PowerType);
	DOREPLIFETIME(ASnakeController, SlowPowers);
	DOREPLIFETIME(ASnakeController, StasisPower);
	DOREPLIFETIME(ASnakeController, SpeedPower);
	DOREPLIFETIME(ASnakeController, SpeedBoostPower);
	DOREPLIFETIME(ASnakeController, ShieldPower);

	DOREPLIFETIME(ASnakeController, PrimaryWeapon);
	DOREPLIFETIME(ASnakeController, StoredWeapon);
	DOREPLIFETIME(ASnakeController, SecondaryWeapon);

	DOREPLIFETIME(ASnakeController, ColorData);

	DOREPLIFETIME(ASnakeController, bFireHeld);
	DOREPLIFETIME(ASnakeController, bLastStand);
}

void ASnakeController::ClearTimer(FTimerHandle& TimerHandle)
{
	FTimerManager& Timer = GetWorld()->GetTimerManager();
	if (Timer.IsTimerActive(TimerHandle))
	{
		Timer.ClearTimer(TimerHandle);
	}
}