// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SpinningTube.h"


// Sets default values
ASpinningTube::ASpinningTube()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	RotationRate = FRotator::ZeroRotator;
	PlaybackPosition = 0.0f;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ASpinningTube::BeginPlay()
{
	Super::BeginPlay();

	if (CustomRotationCurve)
	{
		RotationTimeline = FTimeline();
		FOnTimelineFloatStatic Func;

		Func.BindUObject(this, &ASpinningTube::SetCustomRotationRate);
		RotationTimeline.AddInterpFloat(CustomRotationCurve, Func);
		RotationTimeline.SetLooping(true);

		RotationTimeline.SetPlaybackPosition(PlaybackPosition, false);
		RotationTimeline.Play();
	}
}

// Called every frame
void ASpinningTube::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (CustomRotationCurve)
	{
		RotationTimeline.TickTimeline(DeltaTime);
	}
	else
	{
		float Roll = RotationRate.Roll * DeltaTime;
		float Pitch = RotationRate.Pitch * DeltaTime;
		float Yaw = RotationRate.Yaw * DeltaTime;

		AddActorWorldRotation(FRotator(Pitch, Yaw, Roll));
	}
}

void ASpinningTube::SetCustomRotationRate(float InValue)
{
	FRotator Rotation = FRotator::ZeroRotator;
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	if (!RotationSpecifiers.IsValid())
	{
		FString N = GetName();
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "No Rotation specified for Tube: " + N);
		return;
	}

	if (RotationSpecifiers.bRoll)
	{
		RotationRate.Roll = InValue;
		Rotation.Roll = InValue * DeltaTime;
	}
	if (RotationSpecifiers.bPitch)
	{
		RotationRate.Pitch = InValue;
		Rotation.Pitch = InValue * DeltaTime;
	}
	if (RotationSpecifiers.bYaw)
	{
		RotationRate.Yaw = InValue;
		Rotation.Yaw = InValue * DeltaTime;
	}

	AddActorWorldRotation(Rotation);
}

FRotator ASpinningTube::GetRotationRate() const
{
	return RotationRate;
}

bool ASpinningTube::UsingTimeline() const
{
	return CustomRotationCurve ? true : false;
}