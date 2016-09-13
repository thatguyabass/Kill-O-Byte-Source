// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RocketTankTargetDecal.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ARocketTankTargetDecal : public AActor
{
	GENERATED_BODY()
	
public:
	ARocketTankTargetDecal(const class FObjectInitializer& PCIP);

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* SceneRootComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Decal")
	UDecalComponent* DecalComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Box Component")
	UBoxComponent* BoxComponent;

	UFUNCTION()
	void BeginOverlap(class AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Set actor to hidden and null out projectile */
	void Hide();

	/** make this actor visible. Only valid while projectile is NOT null */
	void Show(class ABaseProjectile* InProjectile);

	/** Is this target actor active? */
	bool IsActive() const;

protected:
	UPROPERTY()
	class ABaseProjectile* Projectile;

	UPROPERTY()
	bool bIsActive;

	UPROPERTY()
	bool bProjectileEntered;

	UPROPERTY(EditAnywhere, Category = "Control")
	float DestroyDistance;

};
