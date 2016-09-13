// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "Repulsion.h"
#include "Powers/Projectiles/BaseProjectile.h"
#include "Powers/Projectiles/BuildProjectile.h"

const int32 ARepulsion::ForceModifier = 1000;

// Sets default values
ARepulsion::ARepulsion()
	: Super()
{
	RepulsionSphere = CreateDefaultSubobject<USphereComponent>("RepulsionSphere");
	RepulsionSphere->SetSphereRadius(250.0f);
	RepulsionSphere->ShapeColor = FColor::Red;
	RootComponent = RepulsionSphere;

	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	ParticleComponent->AttachTo(RootComponent);

	Force = 25.0f;

	bActiveAtStart = true;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ARepulsion::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//If true, activate the repulsion field, else hide it
	bActiveAtStart ? Show() : Hide();

	RepulsionSphere->OnComponentBeginOverlap.AddDynamic(this, &ARepulsion::OnBeginOverlap);
	RepulsionSphere->OnComponentEndOverlap.AddDynamic(this, &ARepulsion::OnEndOverlap);
}

// Called every frame
void ARepulsion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bActive)
	{
		Repel();
	}
}

void ARepulsion::Repel()
{
	if (Projectiles.Num() <= 0)
	{
		return;
	}

	for (int32 c = 0; c < Projectiles.Num(); c++)
	{
		ABaseProjectile* Projectile = Projectiles[c];
		if (Projectile->GetName() == "Invalid")
		{
			GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Red, "Invalid Projectile");
			break;
		}

		//Get the Locations of both the Projectile and the Repulsion Sphere
		FVector ProjectileLoc = Projectile->GetActorLocation();
		FVector RepelLoc = GetActorLocation();

		//Get the Direction from the Projectile to the Repulsion Sphere Center
		FVector RepelDirection = ProjectileLoc - RepelLoc;
		//Get the Vector Length
		float Length = RepelDirection.Size2D();

		//Get the difference between the Vectro length and the Sphere Radius
		float Diff = RepulsionSphere->GetScaledSphereRadius() - Length;

		//Get Force Percent
		float ForcePercent = Diff / RepulsionSphere->GetScaledSphereRadius();

		//Calculate the Force
		float RepelForce = ForcePercent * (Force / ForceModifier);
		if (RepelForce < 0)
		{
			RepelForce = 0;
			return;
		}

		//Get the Current Direction
		FVector CurrentDirection = Projectile->GetDirection();

		//Normalize and apply force to the repulsion direction
		RepelDirection.Normalize();
		RepelDirection *= RepelForce;

		//Calculate the final direction 
		FVector FinalDirection = CurrentDirection + RepelDirection;

		Projectile->SetVelocity(FinalDirection);
	}
}

void ARepulsion::OnBeginOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	ABaseProjectile* Projectile = Cast<ABaseProjectile>(Other);
	if (Projectile)
	{
		//Don't add a build projectile that is attached to a link
		ABuildProjectile* Build = Cast<ABuildProjectile>(Projectile);
		if (Build && Build->IsAttached())
		{
			return;
		}

		Projectiles.Add(Projectile);
	}
}

void ARepulsion::OnEndOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABaseProjectile* Projectile = Cast<ABaseProjectile>(Other);
	if (Projectile && Projectiles.Contains(Projectile))
	{
		Projectiles.Remove(Projectile);
	}
}

void ARepulsion::TriggerTimer(float Duration)
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ARepulsion::HideAndDestroy, Duration, false);
}

void ARepulsion::HideAndDestroy()
{
	//if (Role < ROLE_Authority)
	//{
	//	return;
	//}
	//else
	//{
	this->SetActorHiddenInGame(true);
	Hide();
	this->SetLifeSpan(2.0f);
	//}
}

void ARepulsion::MultiCastRemoveCollision_Implementation()
{
	RepulsionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARepulsion::RemoveCollision()
{
	RepulsionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARepulsion::EnableCollision()
{
	RepulsionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RepulsionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	RepulsionSphere->SetCollisionResponseToChannel(ECC_ProjectileChannel, ECR_Overlap);
	RepulsionSphere->SetCollisionResponseToChannel(ECC_PlayerProjectile, ECR_Overlap);
}

void ARepulsion::Hide()
{
	bActive = false;

	ParticleComponent->DeactivateSystem();

	RemoveCollision();
}

void ARepulsion::Show()
{
	bActive = true;

	ParticleComponent->ActivateSystem();

	EnableCollision();
}

bool ARepulsion::IsActive() const
{
	return bActive;
}