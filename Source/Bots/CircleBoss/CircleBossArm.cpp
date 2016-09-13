// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "CircleBossArm.h"
#include "Boundary/Boundary.h"

const int32 ACircleBossArm::StateCount = 3;

// Sets default values
ACircleBossArm::ACircleBossArm()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	Attachment01 = CreateDefaultSubobject<USceneComponent>(TEXT("Attachment01"));
	Attachment01->AttachTo(RootComponent);
	
	Attachment02 = CreateDefaultSubobject<USceneComponent>(TEXT("Attachment02"));
	Attachment02->AttachTo(Attachment01);

	UpperArmMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UpperArmMesh"));
	UpperArmMesh->AttachTo(Attachment01);

	LowerArmMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LowerArmMesh"));
	LowerArmMesh->AttachTo(Attachment02);

	DoorPlacement = CreateDefaultSubobject<USceneComponent>(TEXT("DoorPlacementComponent"));
	DoorPlacement->AttachTo(Attachment02);

	BlockerPlacement = CreateDefaultSubobject<USceneComponent>(TEXT("BlockerPlacementComponent"));
	BlockerPlacement->AttachTo(Attachment02);

	UpperArmSFXComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("UpperArmSFX"));
	UpperArmSFXComponent->bAutoActivate = false;
	UpperArmSFXComponent->bAlwaysPlay = false;
	UpperArmSFXComponent->AttachTo(RootComponent);

	LowerArmSFXComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("LowerArmSFX"));
	LowerArmSFXComponent->bAutoActivate = false;
	LowerArmSFXComponent->bAlwaysPlay = false;
	LowerArmSFXComponent->AttachTo(RootComponent);

	UpperDuration = 5.0f;
	LowerDuration = 5.0f;
	Progress = 0.0f;

	bReverse = false;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ACircleBossArm::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Door)
	{
		Door->GetRootComponent()->AttachTo(DoorPlacement);
	}

	if (Blocker)
	{
		Blocker->GetRootComponent()->AttachTo(BlockerPlacement);
	}

	SetState(EArmState::Rest);

	BaseInitialUpperRotation = GetActorRotation();
	BaseInitialLowerRotation = Attachment02->GetComponentRotation();
}

void ACircleBossArm::BeginPlay()
{
	Super::BeginPlay();

	if (Door)
	{
		Door->FeignDeath(true, false);
	}

	if (Blocker)
	{
		Blocker->FeignDeath(true, false);
	}
}

// Called every frame
void ACircleBossArm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ArmState != EArmState::Rest)
	{
		Progress += DeltaTime;
		float Blend = Progress / Duration;
		switch (ArmState)
		{
		case EArmState::Stage01:
		{
			LerpRotation(Blend, InitialUpperRotation, InternalTargetRotation, RootComponent);
			break;
		}
		case EArmState::Stage02:
		{
			LerpRotation(Blend, InitialLowerRotation, InternalTargetRotation, Attachment02);
			break;
		}
		}

		if (Progress > Duration)
		{
			if (bReverse)
			{
				DecreaseState();
			}
			else
			{
				IncreaseState();
			}
		}
	}
}

void ACircleBossArm::StartSequence()
{
	FRotator MyRotation = GetActorRotation();
	MyRotation.Roll = 0.0f;

	InitialUpperRotation = GetActorRotation();
	InitialLowerRotation = Attachment02->GetComponentRotation();

	Progress = 0.0f;

	switch (ArmState)
	{
		case EArmState::Rest: break;
		case EArmState::Stage01:
		{
			UpperArmSFXComponent->Play();

			Duration = UpperDuration;
			InternalTargetRotation = bReverse ? BaseInitialUpperRotation : UpperTargetRotation + MyRotation;
			break;
		}
		case EArmState::Stage02:
		{
			LowerArmSFXComponent->Play();

			Duration = LowerDuration;
			InternalTargetRotation = bReverse ? BaseInitialLowerRotation : LowerTargetRotation + MyRotation; 
			break;
		}
	}

}

void ACircleBossArm::LerpRotation(float Blend, FRotator Initial, FRotator Target, USceneComponent* Comp)
{
	if (Blend > 1)
	{
		Blend = 1;
	}
	FRotator Rotation = FMath::Lerp(Initial, Target, Blend);
	Comp->SetWorldRotation(Rotation);
}

void ACircleBossArm::SetState(EArmState InState)
{
	//Check the Current State
	if (ArmState == EArmState::Stage01)
	{
		FRotator MyRotation = GetActorRotation();
		MyRotation.Roll = 0.0f;
		BaseInitialLowerRotation = Attachment02->GetComponentRotation();
	}

	//Change the state 
	ArmState = InState;

	//Check current state 
	if (ArmState != EArmState::Rest)
	{
		StartSequence();
	}
	else
	{
		OnSequenceFinish.ExecuteIfBound();
		MovementSequenceFinished();
	}
}

EArmState ACircleBossArm::GetState() const
{
	return ArmState;
}

void ACircleBossArm::IncreaseState()
{
	uint8 CurrentStateValue = (uint8)ArmState;
	++CurrentStateValue;

	if (CurrentStateValue >= StateCount)
	{
		CurrentStateValue = 0;
	}

	SetState(TEnumAsByte<EArmState>(CurrentStateValue));
}

void ACircleBossArm::DecreaseState()
{
	uint8 CurrentStateValue = (uint8)ArmState;
	--CurrentStateValue;

	if (CurrentStateValue < 0)
	{
		CurrentStateValue = StateCount - 1;
	}

	SetState(TEnumAsByte<EArmState>(CurrentStateValue));
}

void ACircleBossArm::StartForwardSequence()
{
	OnSequenceStart.ExecuteIfBound();
	MovementSequenceStarted();

	if (Door)
	{
		//Activate the Doors 
		Door->ResetBoundary(true);
	}

	if (Blocker)
	{
		Blocker->ResetBoundary(true);
	}

	BaseInitialUpperRotation = GetActorRotation();

	bReverse = false;
	SetState(EArmState::Stage01);
}

void ACircleBossArm::StartReverseSequence()
{
	OnSequenceStart.ExecuteIfBound();
	MovementSequenceStarted();

	bReverse = true;
	SetState(EArmState::Stage02);

	if (Blocker)
	{
		Blocker->FeignDeath(true);
	}

	if (Door)
	{
		Door->FeignDeath(true);
	}
}

void ACircleBossArm::SetDoor(ABoundary* InDoor)
{
	if (InDoor)
	{
		Door = InDoor;
		Door->GetRootComponent()->AttachTo(DoorPlacement);
	}
}

void ACircleBossArm::SetBlocker(ABoundary* InBlocker)
{
	if (InBlocker)
	{
		Blocker = InBlocker;
		Blocker->GetRootComponent()->AttachTo(BlockerPlacement);
	}
}

void ACircleBossArm::BossDead()
{
	if (Door)
	{
		Door->FeignDeath(true);
	}

	if (Blocker)
	{
		Blocker->FeignDeath(true);
	}
}