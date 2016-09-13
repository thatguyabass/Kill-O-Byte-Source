// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ProjectileHeightTrigger.generated.h"

UCLASS()
class SNAKE_PROJECT_API AProjectileHeightTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileHeightTrigger();
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Trigger|Component")
	UBoxComponent* BoxComponent;

	virtual void PostInitializeComponents() override;
	
	UFUNCTION()
	void BeginOverlapCheck(class AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBody, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Unreliable, NetMultiCast)
	void MultiCastRotateProjectile(class ABaseProjectile* Actor, FVector_NetQuantize InRotation);
	virtual void MultiCastRotateProjectile_Implementation(class ABaseProjectile* Actor, FVector_NetQuantize InRotation);
	
};
