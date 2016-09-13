// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "DeathSpeedBot.h"
#include "SnakeCharacter/SnakeLink.h"
#include "SnakeCharacter/SnakeLinkHead.h"

ADeathSpeedBot::ADeathSpeedBot()
	: Super()
{
	OverlapComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxOverlapComponent"));
	OverlapComponent->SetBoxExtent(FVector(250.0f));
	OverlapComponent->AttachTo(RootComponent);

	BotState = EDeathSpeedState::Charging;
}

void ADeathSpeedBot::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &ADeathSpeedBot::OnComponentBeginOverlap);
}


void ADeathSpeedBot::OnComponentBeginOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASnakeLink* Link = Cast<ASnakeLink>(Other);
	if (!Link || BotState != EDeathSpeedState::Moving)
	{
		return;
	}
	
	//Cast<ASnakeLinkHead>(Link->GetHead())->OverrideInvulnerable();
	FDamageEvent DamageEvent;

	Link->TakeDamage(Link->GetTotalHealth() * 3, DamageEvent, Controller, this);
}

float ADeathSpeedBot::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return 0.0f;
}

EDeathSpeedState ADeathSpeedBot::GetBotState()
{
	return BotState;
}

void ADeathSpeedBot::SetBotState(EDeathSpeedState InBotState)
{
	if (BotState != InBotState)
	{
		BotState = InBotState;
	}
}