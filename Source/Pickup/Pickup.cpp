// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "Pickup.h"
#include "Spawner/CollectionSpawner.h"
#include "Snake_ProjectGameMode.h"
#include "SnakeCharacter/SnakeLink.h"

APickup::APickup(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	RootSphereComp = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("RootSphereComp"));
	RootComponent = RootSphereComp;
	RootSphereComp->SetSphereRadius(100.0f);
	RootSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RootSphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootSphereComp->SetCollisionResponseToChannel(ECC_PowerUpChannel, ECR_Overlap);

	Score = 100;

	bCollected = false;
	bCanRespawn = false;

	bCanFadeOut = false;
	PickupExpireDuration = 10.0f;

	RespawnTime = -1;

	NetPriority = 2.0f;
	bReplicates = true;
}

void APickup::BeginPlay()
{
	Super::BeginPlay();

	if (bCanFadeOut && PickupExpireDuration > 0.0f)
	{
		GetWorldTimerManager().SetTimer(FadeOut_TimerHandle, this, &APickup::FadeOut, PickupExpireDuration, false);
	}
}

void APickup::NotifyActorBeginOverlap(AActor* Other)
{
	Super::NotifyActorBeginOverlap(Other);
	PickedUp(Cast<ASnakeLink>(Other));
}

void APickup::PickedUp(ASnakeLink* Link)
{
	//if (Role < ROLE_Authority)
		//return;

	if (!bCollected && Link)
	{
		ASnakePlayerState* State = Cast<ASnakePlayerState>(Link->PlayerState);
		if (State)
		{
			State->ScorePickup(Score);
		}

		GiveTo(Link);
		PostPickedUp();
	}
}

void APickup::GiveTo(ASnakeLink* Link)
{
	//Nothing Here
}

void APickup::PostPickedUp()
{
	if (bCanRespawn)
	{
		//Hide this actor till it can be respawned
		Hide();
		if (RespawnTime >= 0)
		{
			GetWorldTimerManager().SetTimer(TimerHandle, this, &APickup::Respawn, RespawnTime, false);
		}
	}
	else
	{
		HideAndDestroy();
	}
}

void APickup::HideAndDestroy()
{
	Hide();
	this->SetLifeSpan(2.0f);
}

void APickup::Hide()
{
	this->SetActorHiddenInGame(true);
	RootSphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootSphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//MultiCastRemoveCollision();
}

void APickup::Respawn()
{
	//if (Role < ROLE_Authority)
	//	return;

	bCollected = false;
	OnRep_Collected(bCollected);

	this->SetActorHiddenInGame(false);
	//MultiCastAddCollision();
	RootSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RootSphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootSphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	TArray<AActor*> Actors;
	RootSphereComp->GetOverlappingActors(Actors, ASnakeLink::StaticClass());
	if (Actors.Num() > 0)
	{
		for (int32 c = 0; c < Actors.Num(); c++)
		{
			ASnakeLink* Link = Cast<ASnakeLink>(Actors[c]);
			if (Link)
			{
				GiveTo(Link);
				PostPickedUp();
				break;
			}
		}
	}
}

void APickup::FadeOut()
{
	OnFadeOut();
	//Expand this function to flicker when close to expiring 
	HideAndDestroy();
}

void APickup::RotateMesh(UStaticMeshComponent* Mesh, const FRotator& Rotation, float DeltaTime)
{
	if (Mesh)
	{
		FRotator MeshRot = Mesh->GetRelativeTransform().GetRotation().Rotator();

		FRotator Rot = Rotation * DeltaTime;

		FRotator FinalRot = MeshRot.Add(Rot.Pitch, Rot.Yaw, Rot.Roll);
		Mesh->SetRelativeRotation(FinalRot);
	}
}

void APickup::RotateBaseActor(const FRotator& Rotation, float DeltaTime)
{
	FRotator Rot = GetActorRotation();
	FRotator RotAdd = Rotation * DeltaTime;

	FRotator FinalRot = Rot.Add(RotAdd.Pitch, RotAdd.Yaw, RotAdd.Roll);
	SetActorRotation(FinalRot);
}

void APickup::MultiCastRemoveCollision_Implementation()
{
	RootSphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootSphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APickup::MultiCastAddCollision_Implementation()
{
	RootSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RootSphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootSphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void APickup::OnRep_Collected(bool bWasCollected)
{
	bCollected = bWasCollected;
}

void APickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickup, bCollected);
}