// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "RocketTankAI.h"
#include "Bots/Bot/RocketTank.h"

ARocketTankAI::ARocketTankAI(const FObjectInitializer& PCIP)
	: ABaseAIBoss(PCIP)
{
	Phases.Add(FRocketTankPhases());
	Phases.Add(FRocketTankPhases());
	Phases.Add(FRocketTankPhases());

	bBarrageReady = false;
	bChargeReady = false;
	bEncounterStarted = false;

	ChargeLength = 10000.0f;

	bIdleDelay = false;
	IdleStateBuffer = 4.0f;

	DamageThreshold = 0.33f;

	CurrentPhaseIndex = 0;
	FaceTargetRotationRate = 20.0f;

	bConstraintZ = true;

	CurrentState = ERocketTankState::Idle;
}

void ARocketTankAI::InitializeBlackboardKeys()
{
	Super::InitializeBlackboardKeys();

	BarrageSetKey = BlackboardComponent->GetKeyID("BarrageSet");
	ChargeSetKey = BlackboardComponent->GetKeyID("ChargeSet");
}

void ARocketTankAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bEncounterStarted)
	{
		return;
	}

	switch (CurrentState)
	{
	case ERocketTankState::Idle: IdleTick(); break;
	}
}

void ARocketTankAI::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);
}

void ARocketTankAI::SetWeaponTarget(AActor* WeaponTarget)
{
	ARocketTank* Tank = Cast<ARocketTank>(GetPawn());
	AActor* Target = GetTarget();
	if (!Tank || !Target || !WeaponTarget)
	{
		return;
	}

	const FVector SearchOrigin = Target->GetActorLocation();
	FVector Location = UNavigationSystem::GetRandomReachablePointInRadius(Tank, SearchOrigin, Tank->BarrageSpread);
	if (Location != FVector::ZeroVector)
	{
		WeaponTarget->SetActorLocation(Location);
	}
}

void ARocketTankAI::StartBarrage()
{
	//Check if a barrage is already inprogress
	if (GetBarrageSet() || CurrentState != ERocketTankState::Barrage)
	{
		return;
	}

	//Get the Max Barrage count from the current Phase struct
	MaxBarrageCount = GetCurrentPhase().BarrageWaveCount;
	CurrentBarrageCount = 0;
	SetBarrageSet(true);
}

void ARocketTankAI::FireBarrage()
{
	//Check if the Barrage has been started first
	ABot* Bot = Cast<ABot>(GetPawn());
	if (!Bot || !GetBarrageSet())
	{
		return;
	}

	Bot->FireWeapon();
}

void ARocketTankAI::IncreaseBarrageWave()
{
	CurrentBarrageCount++;
	if (CurrentBarrageCount >= MaxBarrageCount)
	{
		BarrageFiredCount++;

		SetBarrageSet(false);
		SetBarrageReady(false);
		SetState(ERocketTankState::Idle);
		GetWorldTimerManager().SetTimer(BarrageCooldown_TimerHandle, this, &ARocketTankAI::SetBarrageReadyTimer, GetBarrageCooldown(), false);
	}
}

void ARocketTankAI::SetBarrageReadyTimer()
{
	SetBarrageReady(true);
}

float ARocketTankAI::GetBarrageCooldown() const
{
	return GetCurrentPhase().BarrageCooldown;
}

void ARocketTankAI::StartCharge()
{
	ARocketTank* Tank = Cast<ARocketTank>(GetPawn());
	AActor* Target = GetTarget();
	if (!Tank && !Target || CurrentState != ERocketTankState::PreCharge)
	{
		return;
	}

	FaceTarget(10.0f);

	FVector Start = Tank->GetActorLocation();
	FVector Direction = Target->GetActorLocation() - Tank->GetActorLocation();
	Direction.Z = 0;
	Direction.Normalize();
	FVector End = Target->GetActorLocation() + (Direction * ChargeLength);

	FHitResult Hit(ForceInit);
	FName TraceName = "ChargeTrace";
	FCollisionQueryParams Param(TraceName);
	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);

	FVector Location = End;

	Param.AddIgnoredComponent(Tank->HitBox);
	Param.AddIgnoredComponent(Tank->HitBox02);

	bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjParams, Param);
	//GetWorld()->DebugDrawTraceTag = TraceName;
	if (bHit)
	{
		Location = Hit.Location;
	}

	Tank->ChargeHitNormal = bHit ? Hit.ImpactNormal : FVector::ZeroVector;
	Tank->StartCharge();
	Tank->OnChargeStart(Direction, Start, Location);

	SetDestination(Location);
	SetChargeSet(true);
	SetFocus(nullptr);
}

void ARocketTankAI::FinishCharge()
{
	SetChargeSet(false);
	SetChargeReady(false);
	
	//Reset the Current Barrage Stats
	BarrageFiredCount = false;
	CurrentBarrageCount = false;

	ARocketTank* Tank = Cast<ARocketTank>(GetPawn());
	if (Tank)
	{
		Tank->FinishCharge();
		Tank->OnChargeFinished();
	}

	StartPostCharge();
	GetWorldTimerManager().ClearTimer(ChargeCooldown_TimerHandle);
	GetWorldTimerManager().SetTimer(ChargeCooldown_TimerHandle, this, &ARocketTankAI::SetChargeReadyTimer, GetChargeCooldown(), false);
}

float ARocketTankAI::GetChargeCooldown() const
{
	return GetCurrentPhase().ChargeCooldown;
}

void ARocketTankAI::SetChargeReadyTimer()
{
	if (GetCurrentPhase().bCanCharge)
	{
		SetChargeReady(true);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(ChargeCooldown_TimerHandle);
		GetWorldTimerManager().SetTimer(ChargeCooldown_TimerHandle, this, &ARocketTankAI::SetChargeReadyTimer, GetChargeCooldown(), false);
	}
}

void ARocketTankAI::StartPreCharge()
{
	SetState(ERocketTankState::PreCharge);

	GetWorldTimerManager().ClearTimer(PreChargeDuration_TimerHandle);
	GetWorldTimerManager().SetTimer(PreChargeDuration_TimerHandle, this, &ARocketTankAI::SetStateToCharge, GetPreChargeDuration(), false);
}

void ARocketTankAI::StartPostCharge()
{
	SetState(ERocketTankState::PostCharge);
	
	GetWorldTimerManager().ClearTimer(PostChargeDuration_TimerHandle);
	GetWorldTimerManager().SetTimer(PostChargeDuration_TimerHandle, this, &ARocketTankAI::SetStateToIdle, GetPostChargeDuration(), false);
}

void ARocketTankAI::DamageTaken(int32 Damage)
{
	//Only applies when in Post Charge State
	if (CurrentState != ERocketTankState::PostCharge)
	{
		return;
	}

	StoredDamage += Damage;
	
	ARocketTank* Tank = Cast<ARocketTank>(GetPawn());
	float Max = Tank->GetMaxHealth();
	if (StoredDamage > Max * DamageThreshold)
	{
		StoredDamage = 0;
		//Clear the Post Charge Timerhandle 
		GetWorldTimerManager().ClearTimer(PostChargeDuration_TimerHandle);
		
		SetStateToIdle();
	}
}

float ARocketTankAI::GetPostChargeDuration()
{
	return GetCurrentPhase().PostChargeDuration;
}

float ARocketTankAI::GetPreChargeDuration()
{
	return GetCurrentPhase().PreChargeDuration;
}

FRocketTankPhases ARocketTankAI::GetCurrentPhase() const
{
	if (Phases.IsValidIndex(CurrentPhaseIndex))
	{
		return Phases[CurrentPhaseIndex];
	}

	return FRocketTankPhases();
}

int32 ARocketTankAI::GetCurrentPhaseIndex() const
{
	return CurrentPhaseIndex;
}

void ARocketTankAI::SetPhaseIndex(int32 Index)
{
	if (Phases.IsValidIndex(Index))
	{
		CurrentPhaseIndex = Index;
	}
}

void ARocketTankAI::SetState(ERocketTankState State)
{
	if (State != CurrentState)
	{
		CurrentState = State;

		if (CurrentState == ERocketTankState::Idle)
		{
			bIdleDelay = true;
			GetWorldTimerManager().ClearTimer(IdleState_TimerHandle);
			GetWorldTimerManager().SetTimer(IdleState_TimerHandle, this, &ARocketTankAI::ResetIdleDelay, IdleStateBuffer, false);
		}
	}
}

void ARocketTankAI::ResetIdleDelay()
{
	bIdleDelay = false;
}

void ARocketTankAI::SetStateToIdle()
{
	SetState(ERocketTankState::Idle);
}

void ARocketTankAI::SetStateToCharge()
{
	SetState(ERocketTankState::Charge);
}

ERocketTankState ARocketTankAI::GetState() const
{
	return CurrentState;
}

bool ARocketTankAI::GetBarrageSet() const
{
	return BlackboardComponent ? BlackboardComponent->GetValue<UBlackboardKeyType_Bool>(BarrageSetKey) : false;
}

void ARocketTankAI::SetBarrageSet(bool InValue)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValue<UBlackboardKeyType_Bool>(BarrageSetKey, InValue);
	}
}

bool ARocketTankAI::GetBarrageReady() const
{
	return bBarrageReady;
}

void ARocketTankAI::SetBarrageReady(bool InValue)
{
	bBarrageReady = InValue;
}

bool ARocketTankAI::GetChargeSet() const
{
	return BlackboardComponent ? BlackboardComponent->GetValue<UBlackboardKeyType_Bool>(ChargeSetKey) : false;
}

void ARocketTankAI::SetChargeSet(bool InValue)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValue<UBlackboardKeyType_Bool>(ChargeSetKey, InValue);
	}
}

bool ARocketTankAI::GetChargeReady() const
{
	return bChargeReady;
}

void ARocketTankAI::SetChargeReady(bool InValue)
{
	bChargeReady = InValue;
}

void ARocketTankAI::IdleTick()
{
	FRocketTankPhases Phase = GetCurrentPhase();

	FaceTarget(FaceTargetRotationRate * GetWorld()->GetDeltaSeconds());

	if (bIdleDelay)
	{
		return;
	}

	bool FireBarrage = bBarrageReady;
	if (Phase.bCanCharge && bChargeReady)
	{
		if (BarrageFiredCount >= Phase.MinBarrageCount)
		{
			StartPreCharge();
			FireBarrage = false;
		}
	}

	if (FireBarrage)
	{
		SetState(ERocketTankState::Barrage);
	}
}

void ARocketTankAI::SetTarget(AActor* InTarget)
{
	Super::SetTarget(InTarget);

	APawn* MyPawn = GetPawn();
	if (MyPawn)
	{
		MyPawn->MoveIgnoreActorAdd(InTarget);
	}
}

void ARocketTankAI::StopActionAbilities()
{
	GetWorldTimerManager().ClearTimer(BarrageCooldown_TimerHandle);
	GetWorldTimerManager().ClearTimer(ChargeCooldown_TimerHandle);

	bBarrageReady = false;
	bChargeReady = false;

	SetState(ERocketTankState::Idle);
}

void ARocketTankAI::StartActionAbilities()
{
	GetWorldTimerManager().SetTimer(BarrageCooldown_TimerHandle, this, &ARocketTankAI::SetBarrageReadyTimer, GetBarrageCooldown(), false);
	GetWorldTimerManager().SetTimer(ChargeCooldown_TimerHandle, this, &ARocketTankAI::SetChargeReadyTimer, GetChargeCooldown(), false);
}

void ARocketTankAI::StartEncounter()
{
	if (bEncounterStarted)
	{
		return;
	}

	bEncounterStarted = true;

	GetWorldTimerManager().ClearTimer(BarrageCooldown_TimerHandle);
	GetWorldTimerManager().SetTimer(BarrageCooldown_TimerHandle, this, &ARocketTankAI::SetBarrageReadyTimer, GetBarrageCooldown(), false);

	GetWorldTimerManager().ClearTimer(ChargeCooldown_TimerHandle);
	GetWorldTimerManager().SetTimer(ChargeCooldown_TimerHandle, this, &ARocketTankAI::SetChargeReadyTimer, GetChargeCooldown(), false);
}