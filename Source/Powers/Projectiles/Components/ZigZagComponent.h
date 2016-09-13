// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "ZigZagComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SNAKE_PROJECT_API UZigZagComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UZigZagComponent();

	// Called when the game starts
	virtual void InitializeComponent() override;
	
	/** How long In Seconds before the next directional change */
	UPROPERTY(EditAnywhere, Category = "ZigZag|Property")
	float Duration;

	UPROPERTY(EditAnywhere, Category = "ZigZag|Property")
	float DirectionOffset;

protected:
	/** Projectile this component is attacehd to */
	class ABaseProjectile* Projectile;

	/** Is this the First Zag? */
	bool bFirst;

	void InitialDirection();

	/** Change teh Zig of the Projectile */
	void ChangeDirection();

	FTimerHandle ZigZag_TimerHandle;
		
};
