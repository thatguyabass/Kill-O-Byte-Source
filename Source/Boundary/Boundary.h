// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Attributes.h"
#include "GameFramework/Actor.h"
#include "Boundary.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ABoundary : public AActor
{
	GENERATED_BODY()
	
public:
	ABoundary(const class FObjectInitializer& PCIP);

	/** Act as a Kill Zone? If the player hits this zone, they will die. */
	UPROPERTY(EditAnywhere, Category = "Attributes")
	bool bKillZone;

	/** Health modifier */
	UPROPERTY(EditAnywhere, Category = "Attributes")
	int32 BaseHealth;

	/** Damage flag to check if the boundary should take damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	bool bCanTakeDamage;

	/** if true, only the lazer will deal damage */
	UPROPERTY(EditAnywhere, Category = "Attributes")
	bool bLazerDamageOnly;

	/** If true, the actor will be hidden opposed to destroyed */
	UPROPERTY(EditAnywhere, Category = "Attributes")
	bool bHideOnDeath;

	/** How many points this boundary is worth when destroyed */
	UPROPERTY(EditAnywhere, Category = "Attributes")
	int32 Points;

	/** Call the blueprint to Hide collision attached to this actor */
	UFUNCTION(BlueprintImplementableEvent, Category = "Hide And Destroy")
	void HideCollision();

	UFUNCTION(BlueprintImplementableEvent, Category = "Hide And Destroy")
	void EnableCollision();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Collision")
	void OnBeginOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void NotifyHit(class UPrimitiveComponent* MyComponent, class AActor* other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	/** Override the base take damage method */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	/** bDead getter. Is this boundary Dead? */
	bool IsDead() const;

	/** Return the Killers player state. Note this can be null */
	class ASnakePlayerState* GetKiller() const;
	
protected:

	/** Current Health after the health calculation */
	int32 Health;

	/** Reduce the Health of the Boundary */
	void ReduceHealth(int32 Damage, AActor* DamageCauser);

	/** Add score to the Destroyer and call HideAndDestroy */
	void DestroyBoundary(AActor* Destroyer);

	/** Hide the actor and destroy it after a short delay */
	void HideAndDestroy();

	/** Flag if the boundaries health drops below 0. Only applies to boundries that can be damaged */
	bool bDead;

	/** Player state of the player that killed this boundary */
	class ASnakePlayerState* Killer;

	/** Find the Floaty Text Manager in the world */
	class AFloatyTextManager* GetFloatyText();

public:
	/** Hide this actor and remove the collision */
	UFUNCTION(BlueprintCallable, Category = "Method")
	void HideActor();

	/** Blueprint Event when the boundary is Destroyed AND fiegning death */
	UFUNCTION(BlueprintImplementableEvent, Category = "Method")
	void OnBoundaryDestroyed(bool bPlayAudio = true);

	UFUNCTION(BlueprintImplementableEvent, Category = "Method")
	void OnBoundaryReset(bool bPlayAudio = true);

	/** Mark as dead, Hide this actor and Remove the collision */
	UFUNCTION(BlueprintCallable, Category = "Method")
	void FeignDeath(bool bPlayTimeline = false, bool bPlayAudio = true);

	/** Reset this actor the the default state */
	UFUNCTION(BlueprintCallable, Category = "Method")
	void ResetBoundary(bool bPlayTimeline = false, bool bPlayAudio = true);

protected:
	void LazerDamageCheck(float Damage, AActor* DamageCauser);

public:
	UPROPERTY(EditAnywhere, Category = "Dynamic Material")
	FName PrimaryColorParameterName;

	UPROPERTY(EditAnywhere, Category = "Dynamic Material")
	FName AlphaParameterName;

	UPROPERTY(EditAnywhere, Category = "Dynamic Material")
	FName GlowParameterName;

	UPROPERTY(EditAnywhere, Category = "Dynamic Material")
	float GlowValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Timeline")
	UCurveFloat* HitCurve;

	UPROPERTY(EditAnywhere, Category = "Hit Timeline")
	float HitTimelineLength;

	UPROPERTY(EditAnywhere, Category = "Shutdown Timeline")
	UCurveFloat* ShutdownCurve;

	UPROPERTY(EditAnywhere, Category = "Shutdown Timeline")
	float ShutdownTimelineLength;

	/** Set the Alpha of the Shield's Dynamic Material Instance */
	UFUNCTION(BlueprintCallable, Category = "Timeline")
	void SetShieldAlpha(float Alpha);

	/** Set the glow of the shield's Dynamic Material Instance */
	UFUNCTION(BlueprintCallable, Category = "Timeline")
		void SetShieldGlow(float Glow);

protected:
	/** Timelines */
	FTimeline HitTimeline;
	FTimeline ShutdownTimeline;
	FTimeline GlowTimeline;

	/** Material attached to this Actor */
	UPROPERTY(BlueprintReadWrite, Category = "Material")
	UMaterialInstanceDynamic* DMI;

};
