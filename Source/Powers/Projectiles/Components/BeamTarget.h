// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BeamTarget.generated.h"

/**
*	Beam Target acts as the end point of a lazer.
*	The Target will move in its forward vector until told to stop. (bMove)
*	Target doesn't do any collision checking 
*/
UCLASS()
class SNAKE_PROJECT_API ABeamTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABeamTarget();
	
	UPROPERTY(EditAnywhere, Category = "Lazer Speed")
	float Speed;

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** Flag if the Lazer should continue moving. If false, the Target will remain in place */
	UPROPERTY()
	bool bMove;
	
	void StartDestroy();

	UFUNCTION(BlueprintCallable, Category = "Beam Target|Util")
	void SetBeamOwner(class ALazor* InOwner);

	/** Execute a Tick for the Beam Target. Called for each of the lazors damage ticks */
	UFUNCTION(BlueprintNativeEvent, Category = "Beam Target|Util")
	void ExecuteTick();
	virtual void ExecuteTick_Implementation();

	UFUNCTION(BlueprintPure, Category = "Beam Target|Util")
	class ALazor* GetBeamOwner() const;

protected:
	UPROPERTY()
	class ALazor* BeamOwner;

};
