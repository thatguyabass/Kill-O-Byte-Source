// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SpinningTube.generated.h"

USTRUCT(BlueprintType)
struct FSpinningTubeRotation
{
	GENERATED_BODY()

	/** If true, apply custom rotation to the Yaw */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	bool bRoll;

	/** If true, apply custom rotation to the pitch */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	bool bPitch;

	/** If true, Apply custom rotation to the Yaw */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	bool bYaw;

	bool IsValid()
	{
		if (bRoll || bPitch || bYaw)
		{
			return true;
		}

		return false;
	}

	FSpinningTubeRotation()
	{
		bRoll = false;
		bPitch = false;
		bYaw = false;
	}

};

UCLASS()
class SNAKE_PROJECT_API ASpinningTube : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpinningTube();

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, Category = "Rotation")
	FRotator RotationRate;

	/** Assign a curve to apply custom rotation to the tube */
	UPROPERTY(EditAnywhere, Category = "Rotation|Custom")
	UCurveFloat* CustomRotationCurve;

	UPROPERTY(EditAnywhere, Category = "Rotation|Custom")
	float PlaybackPosition;

	/** If true, apply custom rotation to the Yaw */
	UPROPERTY(EditAnywhere, Category = "Rotation|Custom")
	FSpinningTubeRotation RotationSpecifiers;

	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintPure, Category = "Rotation")
	FRotator GetRotationRate() const;

	UFUNCTION(BlueprintPure, Category = "Rotation")
	bool UsingTimeline() const;

protected:
	FTimeline RotationTimeline;

	/** Takes in the value from the timeline and applies it to the rotations specified */
	UFUNCTION()
	void SetCustomRotationRate(float InValue);

};
