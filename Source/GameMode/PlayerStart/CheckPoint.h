// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerStart.h"
#include "CheckPoint.generated.h"

UCLASS()
class SNAKE_PROJECT_API ACheckpoint : public APlayerStart
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckpoint(const class FObjectInitializer& PCIP);

	/** Box Trigger to mark this check point as active. */
	UPROPERTY(VisibleDefaultsOnly, Category = "Checkpoint Trigger")
	UBoxComponent* TriggerComponent;

	/** if true, this is where the character will spawn into the map. */
	UPROPERTY(EditAnywhere, Category = "Checkpoint")
	bool bStartingCheckpoint;

	/** Set the Current Checkpoints Activity. If true, Event triggeres. */
	UFUNCTION(BlueprintCallable, Category = "CheckPoint")
	void SetActiveCheckpoint();

	//DECLARE_DELEGATE_OneParam(FOnActiveCheckpointChange, ACheckpoint*);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActiveCheckpointChange, class ACheckpoint*, Checkpoint);

	UPROPERTY(BlueprintAssignable, Category = "Custom Event")
	FOnActiveCheckpointChange OnActiveCheckpointChange;

	virtual void PostInitializeComponents() override;

	/** Set the Active Checkpoint Flag */
	UFUNCTION()
	void FlagActiveCheckpoint(bool IsActive);

protected:
	UFUNCTION()
	void BeginOverlap(class AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY()
	bool bActiveCheckpoint;

};
