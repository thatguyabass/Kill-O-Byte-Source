// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Bot/Bot.h"
#include "ShieldBot.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AShieldBot : public ABot
{
	GENERATED_BODY()
	
public:
	AShieldBot();

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UCapsuleComponent* SafeZoneComponent;

	UPROPERTY(EditAnywhere, Category = "ShieldBot|Properties")
	TSubclassOf<class AForceField> ForceFieldClass;

	UPROPERTY(EditAnywhere, Category = "ShieldBot|Properties")
	TSubclassOf<class ANullifyingEMP> NullEMPClass;

	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

	void ActivateNullifyingEMP(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "ShieldBot|Method")
	void DeployShield();

	UFUNCTION(BlueprintCallable, Category = "ShieldBot|Method")
	void RemoveShield();

	/** The shield has been restarted */
	void OnShieldRestart();

	/** the shield has been shutdown. */
	void OnShieldShutdown();

	/** Getter to check the shield state */
	bool IsShieldShutdown() const;

	/** Spawn the Shield class instead of the weapon */
	virtual void SpawnWeapon() override;
	
	virtual void Dead(class ASnakePlayerState* Killer) override;

protected:
	UPROPERTY()
	class AForceField* ForceField;
	
public:
	UPROPERTY(EditAnywhere, Category = "ShieldBot|Properties")
	float DeployedSpeedScale;

	/** Modify the movement speed based on in scale */
	void ApplyShieldMovementChange(float InScale = 1.0f);

protected:
	/** Initial Movement speed. Which all movement modifications are based */
	float BaseMovementSpeed;
	
public:
	UFUNCTION()
	void OnComponentBeginOverlap(class AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(class AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Enable to collision on the Safe Zone Component */
	void EnableZoneCollision();

	/** Disable the collision on the safe zone component */
	void DisableZoneCollision();

protected:
	UPROPERTY()
	TArray<ABot*> SafeBots;

};
