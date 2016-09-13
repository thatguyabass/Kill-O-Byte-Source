// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"
#include "Powers/Projectiles/BaseProjectile.h"
#include "BuildProjectile.generated.h"

class ASnakeLink;

UCLASS()
class SNAKE_PROJECT_API ABuildProjectile : public ABaseProjectile
{
	GENERATED_BODY()
	
public:
	ABuildProjectile(const class FObjectInitializer& PCIP);

	UPROPERTY(EditAnywhere, Category = "BuildProjectile|Slow")
	FSpeedPower Slow;

	/** Loop through each attached projectile and damage the link appropriatly */
	UFUNCTION(BlueprintCallable, Category = BuildProjectile)
	void DamageAttachedLink();

	/** Reduce the Health of hte link this projectile is attached too */
	UFUNCTION(BlueprintCallable, Category = "BuildProjectile|ReduceHealth")
	void ReduceLinkHealth(AActor* ActorToDamage);

	/** Clean this projectile of any attached projectiles */
	void CleanProjectile();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay();
	virtual void Tick(float DelatTime);

	/** When this projectile is overlapping another actor */
	UFUNCTION()
	void OnBeginOverlap(class AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** When this projectile collides with a blocking object */
	virtual void OnImpact(const FHitResult& HitResult) override;

	virtual void NotifyHit(class UPrimitiveComponent* MyComponent, class AActor* Other, class UPrimitiveComponent* OtherComponent, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	/** if true, this projectile is attached to a Link */
	bool IsAttached();

private:
	/** Other Build projectiles that have attached to this one */
	UPROPERTY(Replicated)
	TArray<ABuildProjectile*> AttachedProjectiles;

	/** Link the projectile is attached to */
	UPROPERTY(Replicated)
	ASnakeLink* AttachedToLink;

	/** Attach this to the Link */
	void Attach(ASnakeLink* Link);

	UFUNCTION(Reliable, NetMultiCast, WithValidation)
	void MultiCastAttachToRootComponent(AActor* Other);
	virtual void MultiCastAttachToRootComponent_Implementation(AActor* Other);
	virtual bool MultiCastAttachToRootComponent_Validate(AActor* Other) { return true; }

};
