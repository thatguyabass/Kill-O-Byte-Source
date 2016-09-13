// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Controller/BaseAIController.h"
#include "TurretAI.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ATurretAI : public ABaseAIController
{
	GENERATED_BODY()
	
public:
	ATurretAI(const class FObjectInitializer& PCIP);
	
	virtual void Possess(APawn* InPawn) override;

	virtual void InitializeBlackboardKeys() override;

	virtual void Tick(float DeltaTime) override;

	virtual void FindTarget() override;

	UFUNCTION(BlueprintCallable, Category = "Turret AI")
	void GetNextWaypoint();

	/** In True, Clamp the rotation between the min and max rotation clamp */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret AI|Stationary")
	bool bClampRotation;

	/** Smallest Value the actor rotation can be. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret AI|Stationary")
	float MinRotationClamp;
	
	/** Largest Value the actor rotation can be. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret AI|Stationary")
	float MaxRotationClamp;

protected:
	int32 WaitKey;

	/** Prevent the Rotation from surpassing the min and max clamp */
	void ClampRotation();
	void Clamp(float& Value, const float BaseValue);

	/** Initial Rotation upon spawn */
	FRotator BaseRotation;

};
