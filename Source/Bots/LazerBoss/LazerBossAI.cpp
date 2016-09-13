// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "LazerBossAI.h"
#include "Bots/LazerBoss/LazerBossHead.h"
#include "GameMode/PlayerState/SnakePlayerState.h"

ALazerBossAI::ALazerBossAI(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	FollowOffset = FVector(400.0f, 0.0f, 400.0f);
	InterpSpeed = 15.0f;

	ZInterpDeltaTime = 0.05f;
	ZInterpSpeed = 4.0f;

	LazerFollowOffset = FVector(800.0f, 0.0f, 400.0f);
	LazerFollowConstraint = 4500.0f;

	SlamProgress = 0.0f;
	SlamDuration = 15.0f;

	VulnerableDuration = 10.0f;
	VulnerableProgress = 0.0f;

	bDead = false;
	bStateSet = false;

	PreSlamDelay = 2.5f;
}

void ALazerBossAI::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ALazerBossAI::BeginPlay()
{
	Super::BeginPlay();

	SetBossState(ELazerBossState::Lazer);
}

void ALazerBossAI::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);
}

void ALazerBossAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDead || !bEncounterStarted)
	{
		return;
	}

	if (BossState != ELazerBossState::Vulnerable)
	{
		FollowTarget();
	}

	switch (BossState)
	{
	case ELazerBossState::Movement: MovementTick(DeltaTime); break;
	case ELazerBossState::Slam: break;
	case ELazerBossState::Lazer: LazerTick(DeltaTime); break;
	case ELazerBossState::Vulnerable: VulnerableTick(DeltaTime); break;
	case ELazerBossState::None: break;
	}
}

void ALazerBossAI::MovementTick(float DeltaTime)
{
	ALazerBossHead* Head = Cast<ALazerBossHead>(GetPawn());
	if (!Head)
	{
		return;
	}

	SlamProgress += DeltaTime;
	if (SlamProgress > SlamDuration)
	{
		SetBossState(ELazerBossState::Slam);
		SlamProgress = 0.0f;
	}
}

void ALazerBossAI::LazerTick(float DeltaTime)
{
	ALazerBossHead* Head = Cast<ALazerBossHead>(GetPawn());
	if (!Head)
	{
		return;
	}

	Head->MoveHandsLazerSequence(DeltaTime);
}

void ALazerBossAI::VulnerableTick(float DeltaTime)
{
	VulnerableProgress += DeltaTime;
	if (VulnerableProgress > VulnerableDuration)
	{
		VulnerableProgress = 0.0f;
		EndVulnerableState();

		ALazerBossHead* Boss = Cast<ALazerBossHead>(GetPawn());
		if (Boss)
		{
			Boss->ResetDamageTaken();
		}
	}
}

bool ALazerBossAI::FollowTarget()
{
	ALazerBossHead* Head = Cast<ALazerBossHead>(GetPawn());
	AActor* Target = GetTarget();
	if (!Head || !Target || bOverrideMovement)
	{
		return false;
	}

	if (Head->IsSlamDownInprogress())
	{
		return true;
	}

	const FRotator TargetRotation = FVector::ForwardVector.Rotation();
	FVector RotOffset = TargetRotation.RotateVector(InternalFollowOffset);
	
	FVector TargetLocation = FVector::ZeroVector;
	if (BossState == ELazerBossState::Lazer)
	{
		FVector Temp = Target->GetActorLocation();
		if (Temp.X > LazerFollowConstraint)
		{
			TargetLocation = Temp;
		}
		else
		{
			TargetLocation = FVector(LazerFollowConstraint, Temp.Y, Temp.Z);
		}
	}
	else
	{
		TargetLocation = Target->GetActorLocation();
	}

	TargetLocation += RotOffset;
	FVector MyLocation = Head->GetActorLocation();

	//Interp to the target location
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	//float Clamp = FMath::Clamp(DeltaTime, 0.05f, 0.4f);
	FVector VInterp = FMath::VInterpTo(MyLocation, TargetLocation, DeltaTime, InterpSpeed);
	VInterp.Z = FMath::FInterpTo(MyLocation.Z, TargetLocation.Z, ZInterpDeltaTime, ZInterpSpeed);
	Head->SetActorLocation(VInterp);

	bool Finished = false;
	float Dist = (TargetLocation - Head->GetActorLocation()).Size();
	if (Dist < 50.0f)
	{
		Finished = true;
	}

	return Finished;
}

void ALazerBossAI::SetTarget(AActor* InActor)
{
	Super::SetTarget(InActor);

	SetFocus(nullptr);
}

void ALazerBossAI::SetBossState(ELazerBossState InState)
{
	BossState = InState;

	HandleStateChange();
}

ELazerBossState ALazerBossAI::GetBossState() const
{
	return BossState;
}

void ALazerBossAI::HandleStateChange()
{
	ALazerBossHead* Head = Cast<ALazerBossHead>(GetPawn());
	if (!Head)
	{
		return;
	}

	bStateSet = true;

	Head->OnStateChange();
	Head->GetCharacterMovement()->GravityScale = 0.0f;

	switch (BossState)
	{
	case ELazerBossState::Movement: 
		InternalFollowOffset = FollowOffset;
		Head->InitializeMovementState();
		break;
	
	case ELazerBossState::Slam:
	{
		SlamIndex = Head->GetRandHandIndex();
		InternalFollowOffset.Y = Head->HandPlacements[SlamIndex]->GetRelativeTransform().GetLocation().Y;

		TriggerHandIdle();

		GetWorldTimerManager().SetTimer(Slam_TimerHandle, this, &ALazerBossAI::StartSlam, PreSlamDelay, false);
		break;
	}

	case ELazerBossState::Lazer:
		InternalFollowOffset = LazerFollowOffset;
		Head->InitializeLazerState();
		break;

	case ELazerBossState::Vulnerable:
		Head->GetCharacterMovement()->GravityScale = 1.0f;
		VulnerableProgress = 0.0f;
		break;

	case ELazerBossState::PostLazer:
		SetBossState(ELazerBossState::Movement);
		break;

	case ELazerBossState::None:
		Head->NeutralizeState();
		break;
	}

	Head->SetBossState(BossState);
}

void ALazerBossAI::StartSlam()
{
	ALazerBossHead* Head = Cast<ALazerBossHead>(GetPawn());
	if (!Head)
	{
		return;
	}

	Head->StartSlam(SlamIndex);
}

void ALazerBossAI::TriggerHandIdle()
{
	ALazerBossHead* Head = Cast<ALazerBossHead>(GetPawn());
	if (!Head)
	{
		return;
	}

	Head->TriggerHandIdle(SlamIndex);
}

void ALazerBossAI::EndVulnerableState()
{
	SetBossState(ELazerBossState::Lazer);
}

void ALazerBossAI::BotDead(ASnakePlayerState* Killer)
{
	bDead = true;
}

void ALazerBossAI::StartEncounter()
{
	if (bEncounterStarted)
	{
		return;
	}

	bEncounterStarted = true;

	if (BossState == ELazerBossState::Lazer)
	{
		ALazerBossHead* Head = Cast<ALazerBossHead>(GetPawn());
		if (Head)
		{
			Head->PlayLazerChargeAudio();
		}
	}
}