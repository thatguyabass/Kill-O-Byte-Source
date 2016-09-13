// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Boundary/Boundary.h"
#include "Trap.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ATrap : public ABoundary
{
	GENERATED_BODY()
	
public:
	ATrap(const class FObjectInitializer& PCIP);

	UPROPERTY(EditAnywhere, Category = "Trap")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, Category = "Trap")
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Trap")
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Trap")
	UBoxComponent* SpikeCollisionComponent;

	/** Trigger the Trap. Optional Delay before it executes */
	UFUNCTION(BlueprintCallable, Category = "Trap")
	void TriggerTrap(float InDelay = 0.0f);

	/** Executes the Trap */
	UFUNCTION(BlueprintImplementableEvent, Category = "Trap")
	void Trigger();

	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

	void NotifyHit(class UPrimitiveComponent* MyComponent, class AActor* Other, class UPrimitiveComponent* OtherComponent,
		bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	UFUNCTION()
	void OnComponentOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Return the current progress of the delay */
	float GetProgress() const;

	void DamagePawn(APawn* Pawn);

private:
	/** Delay before the trap executes */
	FTimerHandle Delay_TimerHandle;

	/** If true, the Trap has been activated with a delay */
	bool bExecute;

	/** Duration before the trap executes after being activated */
	float Delay;

	/** Time line before the trap is executed */
	float Progress;

	UFUNCTION(Reliable, NetMultiCast, WithValidation)
	void MultiCastTrigger();
	virtual void MultiCastTrigger_Implementation();
	virtual bool MultiCastTrigger_Validate() { return true; }

};
