// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SpeedBoostOneWay.h"
#include "SnakeCharacter/SnakeLinkHead.h"

ASpeedBoostOneWay::ASpeedBoostOneWay()
	: Super()
{
	RootComp = CreateDefaultSubobject<USceneComponent>("SppedBoostOneWay-RootComponent");
	RootComponent = RootComp;

	EnterComponent->AttachTo(RootComponent);
	EnterComponent->SetBoxExtent(FVector(100.0f, 250.0f, 250.0f));
	EnterComponent->AddLocalOffset(FVector(-100.0f, 0.0f, 0.0f));

	ExitComponent = CreateDefaultSubobject<UBoxComponent>("ExitComponent");
	ExitComponent->SetBoxExtent(FVector(100.0f, 250.0f, 250.0f));
	ExitComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ExitComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	ExitComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ExitComponent->ShapeColor = FColor::Red;
	ExitComponent->AttachTo(RootComponent);
	ExitComponent->AddLocalOffset(FVector(100.0f, 0.0f, 0.0f));
}

void ASpeedBoostOneWay::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ExitComponent->OnComponentBeginOverlap.AddDynamic(this, &ASpeedBoostOneWay::OnBeginExitOverlap);
	ExitComponent->OnComponentEndOverlap.AddDynamic(this, &ASpeedBoostOneWay::OnEndExitOverlap);
}

void ASpeedBoostOneWay::OnBeginEnterOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	if (!BadPlayers.Contains(Other))
	{
		Super::OnBeginEnterOverlap(Other, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	}
}

void ASpeedBoostOneWay::OnBeginExitOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	ASnakeLinkHead* Head = Cast<ASnakeLinkHead>(Other);
	if (Head)
	{
		BadPlayers.Add(Head);
	}
}

void ASpeedBoostOneWay::OnEndExitOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	if (BadPlayers.Contains(Other))
	{
		BadPlayers.Remove(Other);
	}
}