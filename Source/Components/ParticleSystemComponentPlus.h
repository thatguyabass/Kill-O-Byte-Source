// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Particles/ParticleSystemComponent.h"
#include "ParticleSystemComponentPlus.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Rendering, Common), meta = (BlueprintSpawnableComponent))
class SNAKE_PROJECT_API UParticleSystemComponentPlus : public UParticleSystemComponent
{
	GENERATED_BODY()
	
public:
	//UParticleSystemComponentPlus(const class FObjectInitializer& PCIP);

	/** Activate the particle system */
	UFUNCTION(BlueprintCallable, Category = "ParticleSystem|Helper")
	void ActivateParticleSystem(bool bFlagAsJustAttached = false);

	/** Deactivate the particle system without killing the existing particles. They should fade out after thier lifespan has expired */
	UFUNCTION(BlueprintCallable, Category = "ParticleSystem|Helper")
	void DeactivateParticleSystem();
	
};
