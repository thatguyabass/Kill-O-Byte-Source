// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "TrapTrigger.h"
#include "Trap.h"
#include "SnakeCharacter/SnakeLinkHead.h"

// Sets default values
ATrapTrigger::ATrapTrigger()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	RootComponent = BoxComponent;

	AudioSingleComponent = CreateDefaultSubobject<UAudioComponent>("TrapSingleAudioComponent");
	AudioSingleComponent->bAutoActivate = false;
	AudioSingleComponent->bAlwaysPlay = false;
	AudioSingleComponent->AttachTo(RootComponent);

	AudioLoopComponent = CreateDefaultSubobject<UAudioComponent>("TrapLoopAudioComponent");
	AudioLoopComponent->bAutoActivate = false;
	AudioLoopComponent->bAlwaysPlay = false;
	AudioLoopComponent->AttachTo(RootComponent);

	FuzeComp = CreateDefaultSubobject<UParticleSystemComponent>("FuzeParticleComponent");
	FuzeComp->AttachTo(RootComponent);

	Spline = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	Spline->AttachTo(RootComponent);

	Delay = 0.0f;

	bReplicates = true;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ATrapTrigger::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ATrapTrigger::BeginOverlapCheck);
	Spline->Duration = Delay;
}

void ATrapTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FuzeComp && FuzeComp->IsActive())
	{
		Progress += DeltaTime;
		const FVector WorldLocation = Spline->GetWorldLocationAtTime(Progress, true);
		FuzeComp->SetWorldLocation(WorldLocation);
		
		if (Progress > Delay)
		{
			Progress = 0.0f;
			FuzeComp->SetWorldLocation(Spline->GetWorldLocationAtTime(0.0f));
			FuzeComp->DeactivateSystem();

			AudioLoopComponent->Stop();
		}
	}
}

void ATrapTrigger::BeginOverlapCheck(class AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASnakeLinkHead* Head = Cast<ASnakeLinkHead>(Other);
	if (Head)
	{
		if (bMultiActivate)
		{
			Trigger(Other);
		}
		else if (!bHasBeenActivated)
		{
			Trigger(Other);
		}
	}
}

void ATrapTrigger::Trigger(AActor* Actor)
{
	if (Progress > 0.0f)
	{
		return;
	}

	bHasBeenActivated = true;

	EventTriggeredStart();

	if (Delay > 0.0f)
	{
		AudioLoopComponent->Play();

		GetWorldTimerManager().ClearTimer(EventTrigger_TimerHandle);
		GetWorldTimerManager().SetTimer(EventTrigger_TimerHandle, this, &ATrapTrigger::ExecuteTriggerEvent, Delay, false);

		FVector Location = Spline->GetWorldLocationAtTime(0.0f, true);
		FuzeComp->ActivateSystem();
	}
	else
	{
		EventTriggered();
	}
}

void ATrapTrigger::ExecuteTriggerEvent()
{
	EventTriggered();
}

void ATrapTrigger::Reset()
{
	bHasBeenActivated = false;
	ResetTrigger();
}

bool ATrapTrigger::HasBeenActivated() const
{
	return bHasBeenActivated;
}

void ATrapTrigger::PlaySingleSFX(USoundBase* InSFX)
{
	if (InSFX)
	{
		AudioSingleComponent->SetSound(InSFX);
		AudioSingleComponent->Play();
	}
}