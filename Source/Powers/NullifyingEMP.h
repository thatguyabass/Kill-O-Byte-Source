// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "NullifyingEMP.generated.h"

UCLASS()
class SNAKE_PROJECT_API ANullifyingEMP : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANullifyingEMP();

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UParticleSystemComponent* ParticleComponent;

	/** Audio Component for the EMP */
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UAudioComponent* SFXComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	UParticleSystem* ExplosionTemplate;

	UPROPERTY(EditAnywhere, Category = "Components")
	USoundBase* ExplosionSFX;

	/** If this is changed, the particle effect will also need to be adjusted */
	UPROPERTY(EditAnywhere, Category = "Null|Properties")
	float ExecutionDelay;

	UPROPERTY(EditAnywhere, Category = "Null|Properties")
	float ExecuteRadius;

	UPROPERTY(EditAnywhere, Category = "Null|Properties")
	float DamagePercent;

	virtual void PostInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** Execute the Effect */
	void ExecuteEMP();

	/** Hide and Destroy the Actor after a short delay */
	UFUNCTION(BlueprintCallable, Category = "Null|Helper")
	void HideAndDestroy();

protected:
	UPROPERTY()
	float ExecutionProgress;

	bool bDestroyed;
	
};
