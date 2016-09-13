// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "DestructibleTrigger.generated.h"

UCLASS()
class SNAKE_PROJECT_API ADestructibleTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADestructibleTrigger();

	// Called when the game starts or when spawned
	virtual void PostInitializeComponents() override;

	/** Root Component */
	UPROPERTY(VisibleDefaultsOnly, Category = "Trigger|Components")
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Trigger|Components")
	UStaticMeshComponent* MeshComponent;

	/** Attack Type of this actor */
	UPROPERTY(EditAnywhere, Category = "Trigger|Properties")
	class UAttackTypeDataAsset* AttackTypeDataAsset;

	/** Total health this actor can have */
	UPROPERTY(EditAnywhere, Category = "Trigger|Properties")
	float MaxHealth;

	/** Take Damage Override */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	struct FAttackType GetAttackType();

	UFUNCTION(BlueprintImplementableEvent, Category = "Trigger|Event")
	void Trigger();

protected:
	/** Reduce the Triggers Health */
	void ReduceHealth(float DamageAmount);
	
	/** Set the Triggers Health */
	void SetHealth(int32 InHealth);

	/* Triggers Current Health */
	int32 Health;

	/** Hide and Destroy this actor after a delay */
	void HideAndDestroy();

	/** Track if Hide And Destroy has been called */
	bool bDead;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Trigger|Material")
	TArray<UMaterialInstanceDynamic*> DMI;

	/** Use the Attack Data Asset to get the correct color for this actor */
	UFUNCTION(BlueprintCallable, Category = "Trigger|Material")
	void SetMaterialColor();

	UFUNCTION(BlueprintCallable, Category = "Trigger|Material")
	void CreateAndSetDMI();

public:
	UPROPERTY(EditAnywhere, Category = "Trigger|Material")
	FName PrimaryColorName;

};
