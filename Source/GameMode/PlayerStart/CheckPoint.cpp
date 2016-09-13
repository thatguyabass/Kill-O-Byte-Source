// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "CheckPoint.h"
#include "SnakeCharacter/SnakeLinkHead.h"

// Sets default values
ACheckpoint::ACheckpoint(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	bStartingCheckpoint = false;

	TriggerComponent = PCIP.CreateDefaultSubobject<UBoxComponent>(this, TEXT("CheckpointTrigger"));
	TriggerComponent->AttachTo(RootComponent);
	TriggerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerComponent->SetCollisionResponseToAllChannels(ECR_Overlap);

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ACheckpoint::SetActiveCheckpoint()
{
	FlagActiveCheckpoint(true);
	OnActiveCheckpointChange.Broadcast(this);
}

void ACheckpoint::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	bActiveCheckpoint = bStartingCheckpoint ? true : false;

	TriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::BeginOverlap);
}

void ACheckpoint::BeginOverlap(class AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASnakeLinkHead* Head = Cast<ASnakeLinkHead>(Other);
	if (Head && !bActiveCheckpoint)
	{
		SetActiveCheckpoint();
	}
}

void ACheckpoint::FlagActiveCheckpoint(bool InActive)
{
	bActiveCheckpoint = InActive;
}