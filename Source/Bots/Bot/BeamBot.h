// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Bot/Bot.h"
#include "BeamBot.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ABeamBot : public ABot
{
	GENERATED_BODY()
	
public:
	ABeamBot();

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UParticleSystemComponent* PreFireParticle;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	class UAudioComponent* ChargeSFXComponent;

	virtual void PostInitializeComponents() override;

	/** Fire the Attached Weapon, change the state to FireInprogress and start the cooldown */
	virtual void FireWeapon() override;
	
	UFUNCTION(BlueprintPure, Category = "BeamBot|Weapon")
	class AWeapon_Special* GetWeapon() const;

	/** Fire the Attached Weapon, change the state to FireInprogress and start the cooldown */
	void FireWeaponInternal();

	/** Dont use the Bot Audio for this projectile. Audio is attached to the lazer projectile */
	virtual void PlayFireWeaponSFX() override;

public:
	UPROPERTY(EditAnywhere, Category = "BeamBot|Shield")
	TSubclassOf<class ARepulsion> RepulsionClass;

	UFUNCTION(BlueprintCallable, Category = "BeamBot|Shield")
	void SpawnShield();

	/** Turn the shield on, and enable the collision */
	UFUNCTION(BlueprintCallable, Category = "BeamBot|Shield")
	void ActivateShield();

	/** Turn the Shield off, and remove the collision */
	UFUNCTION(BlueprintCallable, Category = "BeamBot|Shield")
	void DeactivateShield();

protected:
	/** Shield Reference */
	UPROPERTY()
	class ARepulsion* RepulsionField;

	virtual void Dead(class ASnakePlayerState* Killer) override;

};
