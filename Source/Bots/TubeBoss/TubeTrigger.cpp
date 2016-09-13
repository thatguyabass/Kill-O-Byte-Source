// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "TubeTrigger.h"
#include "Bots/Turret/TubeTurret.h"

// Sets default values
ATubeTrigger::ATubeTrigger()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Overlap Box Component"));
	RootComponent = BoxComponent;
	BoxComponent->SetBoxExtent(FVector(250.0f));
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ATubeTrigger::FireTurretsInOverlap()
{
	TArray<AActor*> OverlapOut;
	BoxComponent->GetOverlappingActors(OverlapOut, ATubeTurret::StaticClass());

	for (int32 c = 0; c < OverlapOut.Num(); c++)
	{
		ATubeTurret* Turret = Cast<ATubeTurret>(OverlapOut[c]);
		if (Turret)
		{
			Turret->FireWeapon();
		}
	}
}