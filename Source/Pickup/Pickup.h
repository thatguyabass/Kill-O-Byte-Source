// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

/**
 * 
 */

UCLASS()
class SNAKE_PROJECT_API APickup : public AActor
{
	GENERATED_BODY()

public:
	APickup(const class FObjectInitializer& PCIP);

	virtual void BeginPlay() override;
	 
	UPROPERTY(VisibleDefaultsOnly, Category = Pickup)
	class USphereComponent* RootSphereComp;

	/** Check if an actor has overlaped this pickup */
	virtual void NotifyActorBeginOverlap(AActor* Other) override;

	/** [Server + Client] Link that overlaped this pickup */
	void PickedUp(class ASnakeLink* Link);

	/** Give this power to the link */
	virtual void GiveTo(class ASnakeLink* Link);

	UFUNCTION(BlueprintImplementableEvent, Category = "OnCollected")
	void OnCollected();

	/** Cleanup after being picked up */
	virtual void PostPickedUp();

	/** Hide and Destroy this object */
	void HideAndDestroy();

	/** Hide this object */
	void Hide();

	/** Respawn */
	void Respawn();

	UFUNCTION(Unreliable, NetMultiCast, WithValidation)
	void MultiCastRemoveCollision();
	virtual void MultiCastRemoveCollision_Implementation();
	virtual bool MultiCastRemoveCollision_Validate() { return true; }

	UFUNCTION(Reliable, NetMultiCast, WithValidation)
	void MultiCastAddCollision();
	virtual void MultiCastAddCollision_Implementation();
	virtual bool MultiCastAddCollision_Validate() { return true; }

	UPROPERTY(EditAnywhere, Category = Pickup)
	int32 Score;

	/** Flag to determine if this pickup has been collected. If true, this has been collectd */
	UPROPERTY(ReplicatedUsing=OnRep_Collected)
	bool bCollected;

	UPROPERTY(EditAnywhere, Category = "Pickup|Respawn")
	bool bCanRespawn;

	UPROPERTY(EditAnywhere, Category = "Pickup|Respawn")
	float RespawnTime;

	/** bCollected Replication Function */
	UFUNCTION()
	virtual void OnRep_Collected(bool bWasCollected);

	UFUNCTION(BlueprintCallable, Category = Pickup)
	void RotateMesh(UStaticMeshComponent* Mesh, const FRotator& Rotation, float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = Pickup)
	void RotateBaseActor(const FRotator& Rotaiton, float DelatTime);

	/** Timer Handle */
	FTimerHandle TimerHandle;

public:
	/** If true, the pick up will fade out and destroy itself after a duration */
	UPROPERTY(EditAnywhere, Category = "Pickup|Fade Out")
	bool bCanFadeOut;

	UPROPERTY(EditAnywhere, Category = "Pickup|Fade Out")
	float PickupExpireDuration;

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup|Fade Out")
	void OnFadeOut();

protected:
	/** Fade out if the player hasn't picked it up before the pick up expires */
	void FadeOut();

	FTimerHandle FadeOut_TimerHandle;


};
