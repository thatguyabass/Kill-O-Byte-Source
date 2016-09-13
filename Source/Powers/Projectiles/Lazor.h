// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Lazor.generated.h"

UENUM(BlueprintType)
enum class ELazerTraceMethod : uint8
{
	LeftRight,
	UpDown,
	Both
};

UCLASS()
class SNAKE_PROJECT_API ALazor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALazor();

	/** Root component */
	UPROPERTY(VisibleDefaultsOnly, Category = "Lazer Target")
	USceneComponent* RootComp;

	/** Beam Particle Effect */
	UPROPERTY(VisibleDefaultsOnly, Category = "LAZORBEAM!")
	UParticleSystemComponent* ParticleSystemComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Audio")
	UAudioComponent* AudioComponent;

	UPROPERTY(EditAnywhere, Category = "Lazer|Material")
	UMaterial* LazerMaterial;

	UPROPERTY(EditAnywhere, Category = "Lazer|Material")
	FName PrimaryColorName;

	UPROPERTY(EditAnywhere, Category = "Lazer|Material")
	FName SecondaryColorName;

	/** Beam End point. The actor that does the movement */
	UPROPERTY(EditAnywhere, Category = "Lazer Target")
	TSubclassOf<class ABeamTarget> BeamTargetClass;

	UPROPERTY(EditAnywhere, Category = "Lazer Target")
	ELazerTraceMethod LazerTraceMethod;

	/** Offset for the line traces from the center of the actor. Applied to the Left and Right of the actor */
	UPROPERTY(EditAnywhere, Category = "Lazer Target")
	float LeftRightTraceOffset;

	/** Offset for the line traces from the center of the actor. Applied to Top and Bottom of the actor */
	UPROPERTY(EditAnywhere, Category = "Lazer Target")
	float UpDownTraceOffset;

	/** if true, the line traces will be drawn */
	UPROPERTY(EditAnywhere, Category = "Lazer Target")
	bool bShowLineTrace;

	UFUNCTION(BlueprintPure, Category = "Lazer Properties")
	float GetRemainingDuration();

	/** How long will the Lazer be visible for? */
	UPROPERTY(BlueprintReadOnly, Category = "Lazer Properties")
	float Duration;

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void SetLazerOwner(AActor* InOwner, FVector FireOffset, AActor* InProjectileOwner = nullptr);

	void SetAttackType(const struct FAttackType InAttackType);

	/** Is this lazer Finished Shooting */
	bool IsFinished() const;

	/** Dim the beam and Destroy the actor after a few seconds */
	void HideAndDestroy();

	UFUNCTION()
	class ABeamTarget* GetBeamTarget();

protected:
	/** Actor that owns this Lazer. This is normally the actor that fired it */
	UPROPERTY()
	AActor* LazerOwner;

	/** The Actor that fired this projectile */
	UPROPERTY()
	AActor* ProjectileOwner;

	struct FAttackType AttackType;

	/** Relative Offset from the Lazer Owner */
	FVector OwnerOffset;

	/** The end of the beam. BeamTarget moves independent from this base class */
	UPROPERTY()
	class ABeamTarget* BeamTarget;

	/** If true, the lazer is finished shooting */
	bool bFinished;

	/** Line Trace from the center and from the sides of the particle effect to check for any collisions */
	void TrackLazer(float DeltaTime);

	/** Get the Correct offset for each line trace */
	FVector GetOffset(int32 Index);

	/** Can this particle system fade out? */
	bool bCanFadeOut;

	/** Face the Particle Effect out as it is being destroyed */
	void FadeOut(float Alpha);

	/** Timer For Fade Timeline Start */
	FTimerHandle Fade_TimerHandle;

	/** Timer for HideAndDestroy */
	FTimerHandle Destroy_TimerHandle;

public:
	UPROPERTY(EditAnywhere, Category = "Lazer|Fade Out Timeline")
	UCurveFloat* FadeOutCurve;

	UPROPERTY(EditAnywhere, Category = "Lazer|Fade Out Timeline")
	float FadeOutTimelineLength;

	/** Clear the Timer Handles, and start the Fade out timeline */
	void TerminateLazer();

protected:
	FTimeline FadeOutTimeline;

	void StartFadeOutTimeline();

public:
	UPROPERTY(EditAnywhere, Category = "Lazer|Damage")
	TSubclassOf<UDamageType> DamageTypeClass;

	UFUNCTION()
	void SetDamage(int32 InDamage);

	/** Damage Curve, Used with Damage to calculate the ammount of damage that should be delt to targets */
	UPROPERTY(EditAnywhere, Category = "Lazer|Damage")
	UCurveFloat* DamageCurve;

	UPROPERTY(EditAnywhere, Category = "Lazer|Damage")
	float DamageFrequency;

	UPROPERTY(EditAnywhere, Category = "Lazer|Damage")
	FVector DamageBoxExtent;

	/** If true, Actors on the same team can damaged themselves */
	UPROPERTY(EditAnywhere, Category = "Lazer|Damage")
	bool bFriendlyFire;

	/** If true, God mode will be ignored and the damage will still be applied */
	bool bForceDamageOverride;

protected:
	/** Base Damage Value, All damage calculations will use this as the default damage */
	UPROPERTY(BlueprintReadWrite, Category = "Lazer|Damage")
	int32 Damage;

	FTimeline DamageTimeline;

	/** Damage to apply to targets */
	int32 ScaledDamage;

	float DamageProgress;

	/** Get teh Value from the Timeline and Multiply it by Damage to get the final damage value */
	void CalculateDamage(float Value);

	/** Sweep the length of the Lazer and damage any actors hit. This will automatically reset Damage Progress */
	void DealDamage();

	void SetDamageCollisionChannel(TArray<ECollisionChannel> ECC_Array);

	/** Object Types that this lazer will Collide with. Once a collision is detected the lazer will stop moving forward */
	UPROPERTY(EditAnywhere, Category = "Lazer|Tracking")
	TArray<TEnumAsByte<ECollisionChannel>> TrackingCollisionChannels;

	/** Object Types that this lazer will Damage */
	UPROPERTY(EditAnywhere, Category = "Lazer|Damage")
	TArray<TEnumAsByte<ECollisionChannel>> CollisionChannels;

protected:
	FCollisionObjectQueryParams CreateObjectQueryParams(TArray<TEnumAsByte<ECollisionChannel>>& ECC_Array);

};
