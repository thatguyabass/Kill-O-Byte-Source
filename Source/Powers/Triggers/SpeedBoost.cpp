// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SpeedBoost.h"
#include "SnakeCharacter/SnakeLinkHead.h"

// Sets default values
ASpeedBoost::ASpeedBoost()
{
	EnterComponent = CreateDefaultSubobject<UBoxComponent>("EnterComponent");
	EnterComponent->SetBoxExtent(FVector(250.0f));
	EnterComponent->ShapeColor = FColor::Green;
	EnterComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	EnterComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	EnterComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = EnterComponent;

	SpeedBoost = FSpeedPower();
	SpeedBoost.Duration = 3.0f;
	SpeedBoost.SpeedModifier = 0.8f;
	SpeedBoost.bIsActive = false;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ASpeedBoost::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	EnterComponent->OnComponentBeginOverlap.AddDynamic(this, &ASpeedBoost::OnBeginEnterOverlap);
}

void ASpeedBoost::OnBeginEnterOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	ASnakeLinkHead* Head = Cast<ASnakeLinkHead>(Other);
	if (Head)
	{
		ApplySpeedBoost(Head);
	}
}

void ASpeedBoost::ApplySpeedBoost(ASnakeLinkHead* Head)
{
	if (Head)
	{
		Head->SetSpeedBoostPower(SpeedBoost);
	}
}