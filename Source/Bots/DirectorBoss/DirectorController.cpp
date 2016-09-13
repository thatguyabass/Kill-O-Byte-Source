// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "DirectorController.h"
#include "Bots/DirectorBoss/DirectorBoss.h"

ADirectorController::ADirectorController(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	TransitionDuration = 5.0f;
	TransitionProgress = 0.0f;

	VulnerableDuration = 10.0f;
	VulnerableProgress = 0.0f;

	PostVulnerableDuration = 5.0f;
	PostVulnerableProgress = 0.0f;

	FinalHealthScale = 0.25f;
}

void ADirectorController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ADirectorController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	SetBattleState(EDirectorBattleState::Speed);
	SetDecisionState(EDirectorDecisionState::PreIdle);
}

void ADirectorController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (DecisionState)
	{
	case EDirectorDecisionState::Vulnerable: VulnerableTick(DeltaTime, false); break;
	case EDirectorDecisionState::PostVulnerable: PostVulnerableTick(DeltaTime); break;
	case EDirectorDecisionState::Transition: TransitionTick(DeltaTime); break;
	}
}

void ADirectorController::VulnerableTick(float DeltaTime, bool Override)
{
	VulnerableProgress += DeltaTime;
	if (VulnerableProgress >= VulnerableDuration || Override)
	{
		VulnerableProgress = 0.0f;

		ExecuteAbility();

		SetDecisionState(EDirectorDecisionState::PostVulnerable);
	}
}

void ADirectorController::PostVulnerableTick(float DeltaTime)
{
	PostVulnerableProgress += DeltaTime;
	if (PostVulnerableProgress >= PostVulnerableDuration)
	{
		PostVulnerableProgress = 0.0f;

		CleanupAbility();

		SetDecisionState(EDirectorDecisionState::PreIdle);
		ResetPlatform(false);
	}
}

void ADirectorController::TransitionTick(float DeltaTime)
{
	TransitionProgress += DeltaTime;
	if (TransitionProgress > TransitionDuration)
	{
		ReinitializeBossHealth();

		ADirectorBoss* Boss = Cast<ADirectorBoss>(GetPawn());
		if (Boss)
		{
			Boss->OnTransitionEnd();
		}

		TransitionProgress = 0.0f;
		SetDecisionState(EDirectorDecisionState::PreIdle);

		ResetPlatform(true);
	}
}

void ADirectorController::ExecuteAbility()
{
	ADirectorBoss* Boss = GetBoss();
	if (!Boss)
	{
		return;
	}

	Boss->ResetDamageTaken(false);

	switch (BattleState)
	{
	case EDirectorBattleState::Speed: Boss->ActivateDeathBots(); break;
	case EDirectorBattleState::Mortar: Boss->StartBarrage(); break;
	case EDirectorBattleState::Lazer: break;
	}
}

void ADirectorController::CleanupAbility()
{
	ADirectorBoss* Boss = GetBoss();
	if (!Boss)
	{
		return;
	}

	switch (BattleState)
	{
	case EDirectorBattleState::Speed: Boss->CleanupDeathBots(); break;
	case EDirectorBattleState::Mortar: Boss->FinishBarrage(); break;
	case EDirectorBattleState::Lazer: Boss->StartHeadCleanup(); break;
	}
}

EDirectorBattleState ADirectorController::GetBattleState() const
{
	return BattleState;
}

void ADirectorController::SetBattleState(EDirectorBattleState InBattleState)
{
	if (BattleState != InBattleState)
	{
		//Clean up the old state before moving into the new one
		CleanupAbility();

		BattleState = InBattleState;
		
		HandleBattleState();
	}
}

void ADirectorController::HandleBattleState()
{
	if (BattleState != EDirectorBattleState::Final)
	{
		SetDecisionState(EDirectorDecisionState::Transition);
	}

	switch (BattleState)
	{
		case EDirectorBattleState::Speed: break;
		case EDirectorBattleState::Mortar: break;
		case EDirectorBattleState::Lazer: break;
		case EDirectorBattleState::Final:
		{
			SetDecisionState(EDirectorDecisionState::Final);
			CleanupAbility();
			ReinitializeBossHealth();
			break;
		}
	}
}

bool ADirectorController::IsFinalBattleState() const
{
	return (BattleState == EDirectorBattleState::Final);
}

void ADirectorController::IncreaseBattleState()
{
	uint8 Index = (uint8)BattleState;

	Index++;
	SetBattleState(TEnumAsByte<EDirectorBattleState>(Index));
}

EDirectorDecisionState ADirectorController::GetDecisionState() const
{
	return DecisionState;
}

void ADirectorController::SetDecisionState(EDirectorDecisionState InDecisionState)
{
	if (DecisionState != InDecisionState)
	{
		DecisionState = InDecisionState;

		HandleDecisionState();
	}
}

void ADirectorController::HandleDecisionState()
{
	ADirectorBoss* Boss = Cast<ADirectorBoss>(GetPawn());
	if (!Boss)
	{
		return;
	}

	Boss->OnDecisionStateChange(DecisionState);

	switch (DecisionState)
	{
	case EDirectorDecisionState::PreIdle: Boss->StartMovementState(); break;
	case EDirectorDecisionState::Idle: break;
	case EDirectorDecisionState::Vulnerable: InitializeVulnerableState(); break;
	case EDirectorDecisionState::PostVulnerable: InitializePostVulnerableState(); break;
	case EDirectorDecisionState::Transition: CleanupAbility(); break;
	case EDirectorDecisionState::Final: Boss->InitializeFinalDecisionState(); break;
	}
}

void ADirectorController::InitializeVulnerableState()
{
	VulnerableProgress = 0.0f;

	ADirectorBoss* Boss = GetBoss();
	ensure(Boss);

	Boss->InitializeVulnerableState(BattleState);
}

void ADirectorController::InitializePostVulnerableState()
{
	PostVulnerableProgress = 0.0f;

	ADirectorBoss* Boss = GetBoss();
	if (Boss)
	{
		Boss->InitializePostVulnerableState();
	}
}

void ADirectorController::ReinitializeBossHealth()
{
	ADirectorBoss* Boss = GetBoss();
	if (!Boss)
	{
		return;
	}

	IsFinalBattleState() ? Boss->SetHealth(Boss->GetMaxHealth() * FinalHealthScale) : Boss->SetHealth(Boss->GetMaxHealth());
}

bool ADirectorController::CanDamageDirector() const
{
	return (IsVulnerable() || IsPostVulnerable() || IsFinalDecisionState());
}

bool ADirectorController::IsVulnerable() const
{
	return (DecisionState == EDirectorDecisionState::Vulnerable);
}

bool ADirectorController::IsPostVulnerable() const
{
	return (DecisionState == EDirectorDecisionState::PostVulnerable);
}

bool ADirectorController::IsTransitioning() const
{
	return (DecisionState == EDirectorDecisionState::Transition);
}

bool ADirectorController::IsFinalDecisionState() const
{
	return (DecisionState == EDirectorDecisionState::Final);
}

ADirectorBoss* ADirectorController::GetBoss()
{
	return Cast<ADirectorBoss>(GetPawn());
}

void ADirectorController::ResetPlatform(bool NextWave)
{
	ADirectorBoss* Boss = GetBoss();
	if (Boss)
	{
		Boss->ResetPlatform(NextWave);
	}
}