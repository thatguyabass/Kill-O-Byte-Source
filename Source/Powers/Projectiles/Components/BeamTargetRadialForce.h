// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Powers/Projectiles/Components/BeamTarget.h"
#include "BeamTargetRadialForce.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ABeamTargetRadialForce : public ABeamTarget
{
	GENERATED_BODY()
	
public:
	ABeamTargetRadialForce();

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	URadialForceComponent* ForceComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* OverlapComponent;

	virtual void PostInitializeComponents() override;
	virtual void ExecuteTick_Implementation() override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "BeamTarget|RadialForce")
	void OnBeamTargetOverlap(AActor* Other, class ALazor* ThisBeamOwner);

	UFUNCTION()
	void OnBeginOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
