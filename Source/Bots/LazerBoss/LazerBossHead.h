// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bots/LazerBoss/LazerBossAI.h"
#include "Bots/Bot/BaseBoss.h"
#include "LazerBossHead.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ALazerBossHead : public ABaseBoss
{
	GENERATED_BODY()
	
public:
	ALazerBossHead();

	/** Hit Box */
	UPROPERTY(VisibleDefaultsOnly, Category = "LazerBoss|Components")
	UBoxComponent* HitBox;

	UPROPERTY(VisibleDefaultsOnly, Category = "LazerBoss|Components")
	USceneComponent* HandPlacement01;

	UPROPERTY(VisibleDefaultsOnly, Category = "LazerBoss|Components")
	USceneComponent* HandPlacement02;

	UPROPERTY(VisibleDefaultsOnly, Category = "LazerBoss|Components")
	UParticleSystemComponent* LazerChargingComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "LazerBoss|Components")
	UAudioComponent* LazerChargeAudioComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "LazerBoss|Components")
	UAudioComponent* OneOffAudioComp;

	/** Interp Speed for each hand. The higher the value, the faster they will move */
	UPROPERTY(EditAnywhere, Category = "LazerBoss|Hands")
	float HandInterpSpeed;

	UPROPERTY(EditAnywhere, Category = "LazerBoss|Hands")
	float HandInterpRotationSpeed;

	UPROPERTY(EditAnywhere, Category = "LazerBoss|Hands")
	TSubclassOf<class ALazerBossHand> RightHandClass;

	UPROPERTY(EditAnywhere, Category = "LazerBoss|Hands")
	TSubclassOf<class ALazerBossHand> LeftHandClass;

	/** Hand Actors */
	UPROPERTY(BlueprintReadOnly, Category = "LazerBoss|Hands")
	TArray<class ALazerBossHand*> Hands;

	/** USceneComponent Array. Use for Simpler Looping */
	UPROPERTY()
	TArray<USceneComponent*> HandPlacements;

	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

	/** Spawn the Hand Actors */
	void SpawnHands();

	/** Start a slam */
	UFUNCTION()
	void StartSlam(int32 Index);

	/** Trigger the Hand at Index to the Idle Animation */
	void TriggerHandIdle(int32 Index);

	/** Signal the Slam has completed */
	void SlamFinished();

	/** Get a Random Valid Index for a hand */
	int32 GetRandHandIndex();

	/** Move the hands with the head. Position them based on the HandPlacement Scene Objects */
	void MoveHands(float DeltaTime);

	void InitializeLazerState();
	void InitializeMovementState();
	void NeutralizeState();

	void PlayLazerChargeAudio();

protected:
	/** Rotate the USceneComponent to the normal rotation. Ready to slam another player! */
	UPROPERTY(EditAnywhere, Category = "LazerBoss|Hands")
	float NormalHandPitch;

	/** Rotate the USceneComponent to the stunned rotation when the attached hand is stunned */
	UPROPERTY(EditAnywhere, Category = "LazerBoss|Hands")
	float StunnedHandPitch;

	float InternalInterpSpeed;
	float InternalRotationInterpSpeed;

	/** A hand has been stunned. */
	UFUNCTION()
	void HandStunned(class ALazerBossHand* StunnedHand);

	/** Reset the Hand to a normal state */
	UFUNCTION()
	void ResetHand(class ALazerBossHand* InHand);

	/** if true, all the hands have been stunned */
	UFUNCTION(BlueprintPure, Category = "LazerBoss|Slam")
	bool AreHandsStunned();

public:
	/** Is the slam state not Idle? */
	UFUNCTION(BlueprintPure, Category = "LazerBoss|Slam")
	bool IsSlamInprogress() const;

	/** Is the slam state in a state >= then MoveDown? */
	UFUNCTION(BlueprintPure, Category = "LazerBoss|Slam")
	bool IsSlamDownInprogress() const;

public:
	/** Original Locations of the USceneComponents */
	UPROPERTY()
	TArray<FTransform> HandRelativeTransforms;

	/** */
	UPROPERTY(EditAnywhere, Category = "LazerBoss|Lazer")
	TSubclassOf<class AWeapon_Special> LazerWeaponClass;

	UPROPERTY(EditAnywhere, Category = "LazerBoss|Lazer")
	FVector LazerFireOffset;

	/** Focus point of the Lazer. */
	UPROPERTY(EditAnywhere, Category = "LazerBoss|Lazer")
	FVector LazerOrigin;

	/** Radius the hands orbit around the lazer Origin */
	UPROPERTY(EditAnywhere, Category = "LazerBoss|Lazer")
	FVector2D LazerRadiusRange;

	/** Time it takes for the hands to do one complete rotation */
	UPROPERTY(EditAnywhere, Category = "LazerBoss|Lazer")
	float LazerRotationDuration;

	/** How Fast the hands move around the lazer origin */
	UPROPERTY(EditAnywhere, Category = "LazerBoss|Lazer")
	float LazerInterpSpeed;

	/** Lerp blend when rotating around the lazer origin */
	UPROPERTY(EditAnywhere, Category = "LazerBoss|Lazer")
	float LazerInterpRotationSpeed;

	/** Time line curve that controls the movement speed scaling, the radius scaling and the sharp drop after firing */
	UPROPERTY(EditAnywhere, Category = "LazerBoss|Lazer")
	UCurveFloat* ScaleCurve;

	/** Length of the ScaleCurve Timeline */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LazerBoss|Lazer")
	float ScaleTimelineLength;

	UFUNCTION(BlueprintPure, Category = "LazerBoss|Lazer")
	bool IsLazerTimelinePlaying() const;

	UFUNCTION(BlueprintPure, Category = "LazerBoss|Lazer")
	float GetScaleTimelineProgress() const;

	/** Lowest Value the timeline will reach */
	UPROPERTY(EditAnywhere, Category = "LazerBoss|Lazer")
	float MinScaleTimelineValue;

	/** Largest value the timeline will reach */
	UPROPERTY(EditAnywhere, Category = "LazerBoss|Lazer")
	float MaxScaleTimelineValue;

	/** Time the lazer will be fired during the timeline. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LazerBoss|Lazer")
	float FireLazerAt;

	/** Rotate the UsceneComponents around the Lazer Origin. */
	UFUNCTION()
	void MoveHandsLazerSequence(float DeltaTime);

	/** End the Lazer State */
	UFUNCTION()
	void EndLazerState();

	/** Event Called when the lazer state has ended */
	DECLARE_DELEGATE(FOnLazerStateEnd);
	FOnLazerStateEnd OnLazerStateEnd;

	/** Array of attack types. Used when firing the lazer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LazerBoss|Attributes")
	TArray<class UAttackTypeDataAsset*> AttackTypeDataAssets;

	/** Current Attack Type Index */
	int32 AttackTypeIndex;

	/** Get a Random index and assign it to AttackTypeIndex */
	void GetRandAttackType();

	UFUNCTION(BlueprintImplementableEvent, Category = "LazerBoss|Attack Type")
	void OnAttackTypeChange();

	virtual FAttackType GetAttackType() override;
	
protected:
	UPROPERTY()
	class AWeapon_Special* LazerWeapon;

	/** Current Progress of the Hands as they rotate the lazer origin */
	UPROPERTY()
	float LazerRotationProgress;

	/** Rotation, Speed and Radius Scale */
	UPROPERTY()
	float LazerScale;

	UFUNCTION()
	void SetLazerScale(float Value);

	/** Spawn the Lazer weapon for the Boss */
	void SpawnLazer();

	UFUNCTION()
	void FireLazer();

	FTimeline ScaleTimeline;

public:
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void HideAndDestroy() override;

	/** Total number of complete cycles the boss will have. This value is used to calculate the Damage 
	*	Threshold which will kick the boss from the vulnerable state early
	*/
	UPROPERTY(EditAnywhere, Category = "LazerBoss|Vulnerable")
	int32 CyclePhaseCount;

	/** Reset Damage Taken */
	void ResetDamageTaken();

protected:
	/** Damage Taken this vulnerable state */
	int32 DamageTaken;

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "LazerBoss|State")
	void OnStateChange();

	/** Convert the Enum into a uint8 and transfer the value to the hands. */
	void SetBossState(ELazerBossState InBossState);

	UFUNCTION(BlueprintPure, Category = "LazerBoss|State")
	ELazerBossState GetBossState();

	virtual void StartEncounter() override;

	/** If true, the Encounter has been triggered */
	UPROPERTY()
	bool bEncounterStarted;

public:
	UFUNCTION()
	class ALazerBossAI* GetAIController();

public:
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayOneOffAudioEvent(USoundBase* BaseSound);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* GrowlSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* LaughSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* MuffledSFX;

};
