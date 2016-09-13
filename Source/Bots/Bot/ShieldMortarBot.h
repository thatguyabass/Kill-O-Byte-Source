// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Bot/MortarBot.h"
#include "ShieldMortarBot.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API AShieldMortarBot : public AMortarBot
{
	GENERATED_BODY()
	
public:
	AShieldMortarBot();
	
	UPROPERTY(EditAnywhere, Category = "Mortar|Shield")
	TSubclassOf<class AForceField> ForceFieldClass;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category = "Mortar|Shield")
	void SpawnForceField();

	UFUNCTION(BlueprintCallable, Category = "Mortar|Shield")
	void SetShieldActive(bool Active);

	UFUNCTION(BlueprintPure, Category = "Mortar|Shield")
	bool IsShieldActive() const;

	UFUNCTION(BlueprintPure, Category = "Mortar|Shield")
	bool IsShieldShutdown() const;

	UFUNCTION(BlueprintCallable, Category = "Mortar|Shield")
	void DeployShield();

	UFUNCTION(BlueprintCallable, Category = "Mortar|Shield")
	void RemoveShield();

	UFUNCTION()
	void OnShieldShutdown();

	virtual void Dead(class ASnakePlayerState* Killer) override;

	UFUNCTION(BlueprintPure, Category = "Mortar|Shield")
	class AForceField* GetForceField() const;

protected:
	UPROPERTY()
	class AForceField* ForceField;

};
