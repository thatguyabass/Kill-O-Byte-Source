// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"
#include "Bots/Bot/Bot.h"
#include "SwarmBotHive.generated.h"

UCLASS()
class SNAKE_PROJECT_API ASwarmBotHive : public ABot
{
	GENERATED_BODY()
	
public:
	ASwarmBotHive();
	
	/** List of actors that are attached to this actor. Attached Via Blueprint Contructor */
	UPROPERTY(BlueprintReadWrite, Category = "Swarm Actors")
	TArray<class ASwarmBotBee*> AttachedActors;

	/** Scene Components attached to this Actor. Set via Blueprint Constructor */
	UPROPERTY(BlueprintReadWrite, Category = "Swarm Actors")
	TArray<USceneComponent*> TargetSceneComponents;

	UPROPERTY(EditAnywhere, Category = "Swarm Properties")
	EFireMode FireMode;

	/** If true, attached actors will be destroyed From the End of the Array when this Bot takes damage 
	*	Use HealthThreshold to determine the actors health
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swarm Properties")
	bool bDestroyOnDamage;

	/** Percent of Max Health before a attached actor is removed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swarm Properties")
	float HealthThreshold;

	virtual void BeginPlay() override;

	/** Fire from all the Bots Attached to this Bot */
	virtual void FireWeapon() override;

	/** Get the Scene Component for index */
	USceneComponent* GetSceneTarget(int32 Index);

protected:
	/** Total Damage Taken. */
	int32 CurrentDamageTaken;

	/** Remove one of the bots when damaged */
	virtual void ReduceHealth(int32 Damage, class ASnakePlayerState* Killer) override;

	/** Check if the DamageThreshold is greater then the Healththreshold *
	*	Remove an actor if true
	*/
	void CheckAttached(int32 Damage);

	/** Remove any Visible Bots when killed */
	virtual void Dead(class ASnakePlayerState* Killer) override;

	FTimerHandle Fire_TimerHandle;

	/** Current Fire index */
	int32 FireIndex;

	/** Flag to increment the Fire Index */
	bool bNextIndex;

	void SetNextFireIndex();

protected:
	virtual void StartSpawnEffect() override;

	virtual void SetSpawnEffectBlend(float Blend = 0.0f) override;

};
