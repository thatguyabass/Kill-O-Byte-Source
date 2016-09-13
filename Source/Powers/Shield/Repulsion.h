// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Repulsion.generated.h"

UCLASS()
class SNAKE_PROJECT_API ARepulsion : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARepulsion();

	UPROPERTY(VisibleDefaultsOnly, Category = "Repulsion|Component")
	class USphereComponent* RepulsionSphere;

	UPROPERTY(VisibleDefaultsOnly, Category = "Repulsion|Component")
	UParticleSystemComponent* ParticleComponent;

	/** Force that is applied to the projectiles */
	UPROPERTY(EditAnywhere, Category = "Repulsion|Property")
	float Force;

	virtual void PostInitializeComponents() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION()
	void OnBeginOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Start a timer until Hide And Destroy is called */
	void TriggerTimer(float Duration);

	/** Hide the Actor and Destroy */
	void HideAndDestroy();

	/** The Object that Spawned this Repulsion Field */
	AActor* RepulsionOwner;

	UFUNCTION(BlueprintCallable, Category = "Repulsion|Collision")
	void RemoveCollision();

	UFUNCTION(BlueprintCallable, Category = "Repulsion|Collision")
	void EnableCollision();

	UFUNCTION(Reliable, NetMultiCast, WithValidation)
	void MultiCastRemoveCollision();
	virtual void MultiCastRemoveCollision_Implementation();
	virtual bool MultiCastRemoveCollision_Validate() { return true; }

private:
	/** Projectiles that are currently in the Sphere */
	UPROPERTY()
	TArray<class ABaseProjectile*> Projectiles;

	/** Force Modifier. Force will be Divided by this value */
	static const int32 ForceModifier;

	/** Repel the Projectiles that enter the Sphere */
	void Repel();

	FTimerHandle TimerHandle;

public:
	/** Mark this as hidden. Turn off the Particle Effects and remove collsion */
	UFUNCTION(BlueprintCallable, Category = "Repulsion|Control")
	void Hide();

	/** Turn on the Particle Effects and Enable Collision */
	UFUNCTION(BlueprintCallable, Category = "Repulsion|Control")
	void Show();

	UPROPERTY(EditAnywhere, Category = "Repulsion|Control")
	bool bActiveAtStart;

	UFUNCTION(BlueprintPure, Category = "Repulsion|Control")
	bool IsActive() const;

protected:
	/** if true, the repulsion Field will repel projectiles*/
	bool bActive;

};
