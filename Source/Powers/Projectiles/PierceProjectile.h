// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Powers/Projectiles/DefaultProjectile.h"
#include "PierceProjectile.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API APierceProjectile : public ADefaultProjectile
{
	GENERATED_BODY()
	
public:
	APierceProjectile(const class FObjectInitializer& PCIP);
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Pierce|Components")
	USphereComponent* PierceOverlapComponent;

	/** Total number of times this projectile can pass through a target */
	UPROPERTY(EditAnywhere, Category = "Projectile|Pierce")
	int32 MaxPenetration;

	virtual void PostInitializeComponents() override;

	virtual void OnImpact(const FHitResult& Hit) override;

protected:
	/** number of time this projectile has penetrated a target */
	int32 PenetrationCount;

	/** How much the Actor's scale is affected each penetration */
	float ScaleIncrement;

	/** List of actors this projectile has overlaped */
	UPROPERTY()
	TArray<AActor*> OverlapedActors;

	/** Go through the Overlap List to try and find this actor
	*	If Found in list, return true.
	*	If not found, add to list and return false
	*
	*	@return Return true if actor found in list
	*/
	bool FindInList(AActor* Other);

	/** Scale down the damage based on the number of times it has penetrated */
	virtual void DealDamage(const FHitResult& Hit) override;

	/** Overlap before the actor runs into the blocking volume. 
	*	Ensures the actor being hit isn't the projectile owner Or is based on the Object Type WorldStatic. 
	*	
	*/
	UFUNCTION()
	void PierceOverlapCheck(class AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
};
