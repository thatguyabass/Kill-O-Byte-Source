// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "CircleBossAI.h"
#include "Bots/CircleBoss/CircleBoss.h"

const int32 ACircleBossAI::MaxWallMovementStateCount = 3;

ACircleBossAI::ACircleBossAI(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	BossState = ECircleBossState::Idle;
	WallStartDelay = 10.0f;
	CrushRestartDelay = 10.0f;

	CrushTime = 12.5f;

	ArmSetupDelay = 5.0f;
	RestartDelay = 10.0f;

	LazerChargeDuration = 10.0f;
	LazerFireDuration = 5.0f;
	LazerDamageDelay = 1.0f;

	ArmsTakedownDelay = 2.0f;

	bMoveWallReady = false;
	bCanFollowPlayer = false;
	bEncounterStarted = false;
}

void ACircleBossAI::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ACircleBossAI::BeginPlay()
{
	Super::BeginPlay();

	if (WallMovementCurve)
	{
		WallMovementTimeline = FTimeline();
		WallMovementTimeline.SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
		FOnTimelineFloatStatic Func;
		Func.BindUObject(this, &ACircleBossAI::SetWallTimelineProgress);
		WallMovementTimeline.AddInterpFloat(WallMovementCurve, Func);
	
		FOnTimelineEvent CrushFunc;
		CrushFunc.BindUFunction(this, "SetWallCrushState");
		WallMovementTimeline.AddEvent(CrushTime, CrushFunc);

		FOnTimelineEventStatic EndEvent;
		EndEvent.BindUObject(this, &ACircleBossAI::InternalFinishWallMovement);
		WallMovementTimeline.SetTimelineFinishedFunc(EndEvent);
	}
}

void ACircleBossAI::StartEncounter()
{
	//Prevent this from being called again
	if (bEncounterStarted)
	{
		return;
	}

	bEncounterStarted = true;

	GetWorldTimerManager().ClearTimer(WallSetup_TimerHandle);
	GetWorldTimerManager().SetTimer(WallSetup_TimerHandle, this, &ACircleBossAI::SetMoveWallReady, WallStartDelay, false);
}

void ACircleBossAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (BossState)
	{
	case ECircleBossState::Idle: IdleTick(DeltaTime); break;
	case ECircleBossState::WallInProgress: WallInProgressTick(DeltaTime); break;
	case ECircleBossState::ArmSetup: ArmsSetupTick(DeltaTime); break;
	}
}

void ACircleBossAI::IdleTick(float DeltaTime)
{
	if (bMoveWallReady)
	{
		SetState(ECircleBossState::WallSetup);
	}

	FaceTarget(0.25f);
}

void ACircleBossAI::WallInProgressTick(float DeltaTime)
{
	ACircleBoss* Boss = Cast<ACircleBoss>(GetPawn());
	if (!Boss)
	{
		return;
	}

	MoveWalls(DeltaTime);

	if (Boss->IsWallWeakPointDead() || !GetTarget())
	{
		SetState(ECircleBossState::WallTakeDown);
	}
}

void ACircleBossAI::ArmsSetupTick(float DeltaTime)
{
	if (bCanFollowPlayer)
	{
		FaceTarget(0.25f);
	}
}

void ACircleBossAI::SetState(ECircleBossState InState)
{
	if (InState != BossState)
	{
		BossState = InState;
	}
}

ECircleBossState ACircleBossAI::GetState() const
{
	return BossState;
}

void ACircleBossAI::SetWallCrushState()
{
	WallMovementState = EWallMovementState::Crush;
	OnWallMovementStateChange.ExecuteIfBound(WallMovementState);
	ACircleBoss* Boss = Cast<ACircleBoss>(GetPawn());
	if (Boss)
	{
		Boss->PlayWallCrushSFX();
	}
}

void ACircleBossAI::StartWallSetup()
{
	ACircleBoss* Boss = Cast<ACircleBoss>(GetPawn());
	if (!Boss || BossState != ECircleBossState::WallSetup)
	{
		return;
	}

	SetFocus(nullptr);
	bMoveWallReady = false;
	
	Boss->ResetWallWeakPoint();

	WallMovementTimeline.PlayFromStart();
	Boss->ResetWalls();
	Boss->MoveWallsUp();

	MoveProgress = 0.0f;
	TotalProgress = 0.0f;

	WallMovementState = EWallMovementState::Movement;
	OnWallMovementStateChange.ExecuteIfBound(WallMovementState);

	GetWorldTimerManager().ClearTimer(WallInProgress_TimerHandle);
	GetWorldTimerManager().SetTimer(WallInProgress_TimerHandle, this, &ACircleBossAI::SetMoveWallReady, Boss->GetTravelTime(), false);
	SetState(ECircleBossState::WallInProgress);
}

void ACircleBossAI::MoveWalls(float DeltaTime)
{
	ACircleBoss* Boss = Cast<ACircleBoss>(GetPawn());
	if (!Boss || !bMoveWallReady || BossState != ECircleBossState::WallInProgress)
	{
		return;
	}

	WallMovementTimeline.TickTimeline(DeltaTime);
}

void ACircleBossAI::SetWallTimelineProgress(float Value)
{
	ACircleBoss* Boss = Cast<ACircleBoss>(GetPawn());
	if (!Boss || !bMoveWallReady || BossState != ECircleBossState::WallInProgress)
	{
		return;
	}

	Boss->MoveWallsAlongSpline(Value);
}

void ACircleBossAI::InternalFinishWallMovement()
{
	FinishWallMovement();
}

void ACircleBossAI::FinishWallMovement(bool bReset)
{
	ACircleBoss* Boss = Cast<ACircleBoss>(GetPawn());
	if (!Boss || BossState != ECircleBossState::WallTakeDown)
	{
		return;
	}
	bMoveWallReady = false;

	WallMovementTimeline.Stop();

	Boss->MoveWallsDown();
	Boss->HideWallWeakPoint();

	bCanFollowPlayer = true;
	ResetAbilities(CrushRestartDelay);
}

void ACircleBossAI::ResetAbilities(float Duration)
{
	//The Walls finished thier movement
	GetWorldTimerManager().ClearTimer(WallSetup_TimerHandle);
	GetWorldTimerManager().SetTimer(WallSetup_TimerHandle, this, &ACircleBossAI::SetMoveWallReady, Duration, false);
	SetState(ECircleBossState::Idle);
}

void ACircleBossAI::TriggerArmPhase()
{
	SetState(ECircleBossState::WallTakeDown);
	FinishWallMovement(false);
	GetWorldTimerManager().ClearTimer(WallSetup_TimerHandle);

	//the Weak point was destroyed before the wall finished 
	SetState(ECircleBossState::ArmSetup);
	GetWorldTimerManager().ClearTimer(ArmSetup_TimerHandle);
	GetWorldTimerManager().SetTimer(ArmSetup_TimerHandle, this, &ACircleBossAI::StartArmSetup, ArmSetupDelay, false);
}

void ACircleBossAI::StartArmSetup()
{
	ACircleBoss* Boss = Cast<ACircleBoss>(GetPawn());
	if (!Boss)
	{
		return;
	}
	bCanFollowPlayer = false;

	Boss->EnableBossWeakPointCollision();
	Boss->StartForwardArmSequence();
}

void ACircleBossAI::StartArmTakedown()
{
	ACircleBoss* Boss = Cast<ACircleBoss>(GetPawn());
	if (!Boss)
	{
		return;
	}

	Boss->StartReverseArmSequence();

	GetWorldTimerManager().ClearTimer(ArmInProgress_TimerHandle);
	GetWorldTimerManager().SetTimer(ArmInProgress_TimerHandle, this, &ACircleBossAI::ResetAbilitiesTimer, ArmsTakedownDelay, false);
}

void ACircleBossAI::ResetAbilitiesTimer()
{
	ResetAbilities(RestartDelay);
}

void ACircleBossAI::SetArmsInPosition()
{
	// Only valid when in the Arms Setup state Prevents accidental calls out of state 
	if (BossState == ECircleBossState::ArmSetup)
	{
		SetState(ECircleBossState::ArmInProgress);
		ChargeLazer();
	}
}

float ACircleBossAI::GetLazerChargeProgress()
{
	return GetWorld() ? GetWorldTimerManager().GetTimerElapsed(LazerCharge_TimerHandle) : 0.0f;
}

void ACircleBossAI::ChargeLazer()
{
	ACircleBoss* Boss = Cast<ACircleBoss>(GetPawn());
	if (Boss)
	{
		//Start playing the particle effects 
		Boss->OnChargeLazer();
	}

	GetWorldTimerManager().ClearTimer(LazerCharge_TimerHandle);
	GetWorldTimerManager().SetTimer(LazerCharge_TimerHandle, this, &ACircleBossAI::FireLazer, LazerChargeDuration, false);
}

void ACircleBossAI::FireLazer()
{
	ACircleBoss* Boss = Cast<ACircleBoss>(GetPawn());
	if (Boss)
	{
		//Play the lazer particle effect 
		Boss->OnFireLazer();
	}

	//Apply Damage to the Collected Actors 
	ApplyLazerDamage();

	GetWorldTimerManager().SetTimer(LazerDamage_TimerHandle, this, &ACircleBossAI::ApplyLazerDamage, LazerDamageDelay, true);

	GetWorldTimerManager().ClearTimer(ArmInProgress_TimerHandle);
	GetWorldTimerManager().SetTimer(ArmInProgress_TimerHandle, this, &ACircleBossAI::ResetLazer, LazerFireDuration, false);
}

void ACircleBossAI::ResetLazer()
{
	ACircleBoss* Boss = Cast<ACircleBoss>(GetPawn());
	if (Boss)
	{
		Boss->bArmPhaseInprogress = false;
		Boss->ResetArmHealth();

		Boss->RemoveBossWeakPointCollision();

		//Stop the particle Effects 
		Boss->OnResetLazer();
	}

	GetWorldTimerManager().ClearTimer(LazerCharge_TimerHandle);
	GetWorldTimerManager().ClearTimer(LazerDamage_TimerHandle);

	StartArmTakedown();
}

void ACircleBossAI::ApplyLazerDamage()
{
	ACircleBoss* Boss = Cast<ACircleBoss>(GetPawn());
	if (Boss)
	{
		if (Boss->IsDead())
		{
			//Clear the timer if the boss is dead 
			GetWorldTimerManager().ClearTimer(LazerDamage_TimerHandle);
		}
		else
		{
			Boss->FireLazer();
		}
	}
}

void ACircleBossAI::SetMoveWallReady()
{
	bMoveWallReady = true;
}

void ACircleBossAI::SetWallSetup()
{
	SetState(ECircleBossState::WallSetup);
}

void ACircleBossAI::SetWallInProgress()
{
	SetState(ECircleBossState::WallInProgress);
}

void ACircleBossAI::SetArmInProgress()
{
	SetState(ECircleBossState::ArmInProgress);
}

void ACircleBossAI::SetTarget(AActor* InActor)
{
	Super::SetTarget(InActor);

	SetFocus(nullptr);
}

bool ACircleBossAI::CanSpawnBots()
{
	bool CanSpawn = false;

	switch (BossState)
	{
	case ECircleBossState::Idle:
	//case ECircleBossState::ArmSetup:
	case ECircleBossState::WallSetup:
	case ECircleBossState::WallTakeDown:
		CanSpawn = true;
		break;
	}

	return CanSpawn;
}

void ACircleBossAI::ClearTimers()
{
	GetWorldTimerManager().ClearTimer(ArmSetup_TimerHandle);
	GetWorldTimerManager().ClearTimer(ArmInProgress_TimerHandle);
	GetWorldTimerManager().ClearTimer(LazerCharge_TimerHandle);
}