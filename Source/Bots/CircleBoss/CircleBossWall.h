// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/CircleBoss/CircleBossAI.h"
#include "Boundary/Boundary.h"
#include "CircleBossWall.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ACircleBossWall : public ABoundary
{
	GENERATED_BODY()
	
public:
	ACircleBossWall(const class FObjectInitializer& PCIP);
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UBoxComponent* BoxComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UBoxComponent* OverlapComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	class UMoveToLocation* MoveToComponent;

	/** Set teh AI controller that controls this walls */
	void SetWallOwner(ACircleBossAI* InOwner);

	virtual void PostInitializeComponents() override;

	virtual void NotifyHit(class UPrimitiveComponent* MyComponent, class AActor* Other, class UPrimitiveComponent* OtherComponent,
		bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	/** Mark this actor as hidden in game */
	void Hide();

	/** Mark this actor as visible in game */
	void Show();

protected:
	ACircleBossAI* WallOwner;

	/** Current State of movement for this wall  */
	EWallMovementState WallMovementState;

	/** Set the Wall Movement State */
	void SetWallState(EWallMovementState InState);

	UFUNCTION()
	void OnBeginOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
