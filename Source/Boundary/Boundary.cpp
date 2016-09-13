// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "Boundary.h"
#include "SnakeCharacter/SnakeLinkHead.h"
#include "Bots/Bot/Bot.h"
#include "GameMode/BaseGameMode.h"
#include "GameMode/PlayerState/SnakePlayerState.h"
#include "Components/FloatyTextManager.h"

ABoundary::ABoundary(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	bKillZone = false;
	bDead = false;
	bLazerDamageOnly = false;
	bHideOnDeath = false;

	AlphaParameterName = "Alpha Param";
	GlowParameterName = "Glow Param";
	GlowValue = 10.0f;

	PrimaryActorTick.bCanEverTick = true;
}

void ABoundary::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Health = BaseHealth;
}

void ABoundary::BeginPlay()
{
	Super::BeginPlay();

	if (HitCurve)
	{
		HitTimeline = FTimeline();
		FOnTimelineFloatStatic Func;

		Func.BindUObject(this, &ABoundary::SetShieldAlpha);
		HitTimeline.AddInterpFloat(HitCurve, Func);
		HitTimeline.SetTimelineLength(HitTimelineLength);
	}

	if (ShutdownCurve)
	{
		ShutdownTimeline = FTimeline();
		FOnTimelineFloatStatic Func;

		Func.BindUObject(this, &ABoundary::SetShieldAlpha);
		ShutdownTimeline.AddInterpFloat(ShutdownCurve, Func);
		ShutdownTimeline.SetTimelineLength(ShutdownTimelineLength);
	}
}

void ABoundary::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HitTimeline.IsPlaying())
	{
		HitTimeline.TickTimeline(DeltaTime);
	}

	if (ShutdownTimeline.IsPlaying())
	{
		ShutdownTimeline.TickTimeline(DeltaTime);
	}
}

void ABoundary::NotifyHit(class UPrimitiveComponent* MyComponent, class AActor* Other, class UPrimitiveComponent* OtherComponent,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComponent, Other, OtherComponent, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	
	if (bKillZone)
	{
		OnBeginOverlap(Other, OtherComponent, 0, false, Hit);
	}
}

void ABoundary::OnBeginOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bKillZone)
	{
		ASnakeLinkHead* Link = Cast<ASnakeLinkHead>(Other);
		ABot* Bot = Cast<ABot>(Other);
		if (Link || Bot)
		{
			if (Link)
			{
				Link->OverrideInvulnerable();
			}
			else if (Bot)
			{
				Bot->bOverrideInvulnerable = true;
			}

			FDamageEvent Event;
			Other->TakeDamage(100000, Event, nullptr, nullptr);
		}
	}
}

float ABoundary::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bCanTakeDamage && !bDead && !bLazerDamageOnly)
	{
		ReduceHealth(DamageAmount, DamageCauser);
	}
	else if (bCanTakeDamage && !bDead && bLazerDamageOnly)
	{
		LazerDamageCheck(DamageAmount, DamageCauser);
	}

	return 0.0f;
}

void ABoundary::LazerDamageCheck(float DamageAmount, AActor* DamageCauser)
{
	if (!DamageCauser)
	{
		return;
	}

	ASnakeLinkHead* Head = Cast<ASnakeLinkHead>(DamageCauser);
	if (Head && Head->LazerBeingFired())
	{
		ReduceHealth(DamageAmount, DamageCauser);
	}
}

void ABoundary::ReduceHealth(int32 Damage, AActor* DamageCauser)
{
	if (Damage <= 0.0f)
	{
		return;
	}

	Health -= Damage;

	if (HitCurve)
	{
		HitTimeline.PlayFromStart();
	}

	if (Health <= 0.0f)
	{
		if (bHideOnDeath)
		{
			FeignDeath(true);
		}
		else
		{
			DestroyBoundary(DamageCauser);
		}
	}
}

void ABoundary::DestroyBoundary(AActor* Destroyer)
{
	ASnakePlayerState* State = Cast<ASnakePlayerState>(Destroyer);
	if(State)
	{
		Killer = State;
		State->ScoreMisc(Points);
	}

	HideAndDestroy();
}

void ABoundary::HideAndDestroy()
{
	FeignDeath();

	this->SetLifeSpan(2.0f);
}

bool ABoundary::IsDead() const
{
	return bDead;
}

ASnakePlayerState* ABoundary::GetKiller() const
{
	return Killer;
}

AFloatyTextManager* ABoundary::GetFloatyText()
{
	for (TActorIterator<AFloatyTextManager> FloatyText(GetWorld()); FloatyText; ++FloatyText)
	{
		if (*FloatyText)
		{
			return (*FloatyText);
		}
	}

	return nullptr;
}

void ABoundary::HideActor()
{
	HideCollision();
	SetActorHiddenInGame(true);
}

void ABoundary::FeignDeath(bool bPlayTimeline, bool bPlayAudio)
{
	if ((bPlayTimeline && ShutdownCurve) && !bDead)
	{
		ShutdownTimeline.PlayFromStart();
		HideCollision();
	}
	else
	{
		HideActor();
	}

	bDead = true;

	OnBoundaryDestroyed(bPlayAudio);
}

void ABoundary::ResetBoundary(bool bPlayTimeline, bool bPlayAudio)
{
	if (bPlayTimeline && ShutdownCurve)
	{
		ShutdownTimeline.ReverseFromEnd();
	}

	Health = BaseHealth;

	bDead = false;
	EnableCollision();
	SetActorHiddenInGame(false);

	OnBoundaryReset(bPlayAudio);
}

void ABoundary::SetShieldAlpha(float Alpha)
{
	if (DMI)
	{
		DMI->SetScalarParameterValue(AlphaParameterName, Alpha);
	}
}

void ABoundary::SetShieldGlow(float Glow)
{
	if (DMI)
	{
		DMI->SetScalarParameterValue(GlowParameterName, Glow);
	}
}