// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "SineComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SNAKE_PROJECT_API USineComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USineComponent();

	UPROPERTY(EditAnywhere, Category = "Sine|Property")
	float StartingElapsed;

	UPROPERTY(EditANywhere, Category = "Sine|Property")
	float Oscillation;

	UPROPERTY(EditAnywhere, Category = "Sine|Property")
	float Offset;

	// Called when the game starts
	virtual void InitializeComponent() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

protected:
	class ABaseProjectile* Projectile;

	/** 
	* Stored the Right and Forward vector. The sine wave will be based off these values, becaus the vector directions for the projectile changes based on the velocity.
	* If the Live Vectors are called it will either run straight (Forward) or spin in a circle (Right).
	*/
	FVector StoredRight;
	FVector StoredForward;

	float Elapsed;	
	
};
