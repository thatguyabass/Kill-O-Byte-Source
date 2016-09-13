// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/Controller/BaseAIBoss.h"
#include "LazerBossAI.generated.h"

UENUM(BlueprintType)
enum class ELazerBossState : uint8
{
	Movement,
	Slam,
	Lazer,
	Vulnerable,
	PostLazer,
	None
};

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ALazerBossAI : public ABaseAIBoss
{
	GENERATED_BODY()
	
public:
	ALazerBossAI(const class FObjectInitializer& PCIP);

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Possess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

	void MovementTick(float DeltaTime);
	void LazerTick(float DeltaTime);
	void VulnerableTick(float DeltaTime);

public:
	UPROPERTY(EditAnywhere, Category = "LazerBoss|Movement")
	FVector FollowOffset;

	UPROPERTY(EditAnywhere, Category = "LazerBoss|Movement")
	float InterpSpeed;

	UPROPERTY(EditAnywhere, Category = "LazerBoss|Movement")
	float ZInterpDeltaTime;

	UPROPERTY(EditAnywhere, Category = "LazerBoss|Movement")
	float ZInterpSpeed;

	UPROPERTY(EditAnywhere, Category = "LazerBoss|Movement")
	FVector LazerFollowOffset;

	UPROPERTY(EditAnywhere, Category = "LazerBoss|Movement")
	float LazerFollowConstraint;

	/** Follow the Target, offset based on the Offset Value. Ignore height and only use X/Y */
	UFUNCTION(BlueprintCallable, Category = "LazerBoss|Movement")
	bool FollowTarget();

	/** If true, Follow Target will not execute. Custom movement will be exepected */
	UPROPERTY(EditAnywhere, Category = "LazerBoss|Movement")
	bool bOverrideMovement;

protected:
	/** Internal Offset, modify this to change the Follow Offset */
	FVector InternalFollowOffset;

	/** Hand index that is performing the slam */
	int32 SlamIndex;
	
	/** Time before a slam will occur. */
	UPROPERTY(EditAnywhere, Category = "LazerBoss|Slam")
	float SlamDuration;

	/** Progress till the next slam */
	float SlamProgress;

	UPROPERTY(EditAnywhere, Category = "LazerBoss|Slam")
	float PreSlamDelay;

	void StartSlam();

	/** Trigger the hand at Slam Index to transition to the idle animation */
	void TriggerHandIdle();

	FTimerHandle Slam_TimerHandle;

public:
	/** How long is the Boss Vulnerable for? */
	UPROPERTY(EditAnywhere, Category = "LazerBoss|Vulnerable")
	float VulnerableDuration;

	/** Progress to the next state */
	void EndVulnerableState();

protected:
	/** Elapsed time the Boss has been vulnerable */
	UPROPERTY()
	float VulnerableProgress;

public:
	UFUNCTION(BlueprintCallable, Category = "LazerBoss|State")
	void SetBossState(ELazerBossState InState);

	UFUNCTION(BlueprintCallable, Category = "LazerBoss|State")
	ELazerBossState GetBossState() const;

protected:
	ELazerBossState BossState;

	bool bStateSet;

	UFUNCTION()
	void HandleStateChange();

public:
	virtual void SetTarget(AActor* InActor) override;

	virtual void StartEncounter() override;

	void StopEncounter();

	virtual void BotDead(class ASnakePlayerState* Killer) override;

protected:
	/** Pawn Dead Flag */
	bool bDead;

};
