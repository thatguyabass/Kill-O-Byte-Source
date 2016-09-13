// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"
#include "Powers/Projectiles/SnakeProjectileMoveComponent.h"
#include "GameMode/PlayerState/SnakePlayerState.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ABaseProjectile : public AActor
{
	GENERATED_BODY()

public:
	ABaseProjectile(const class FObjectInitializer& PCIP);

	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile|Components")
	class USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile|Components")
	class USphereComponent* OverlapComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile|Components")
	class UParticleSystemComponent* ParticleSystemComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile|Components")
	class UAudioComponent* OnHitAudioComponent;

	/** Audio that fires when this projectile is fired */
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile|Components")
	class UAudioComponent* FiredAudioComponent;

	UPROPERTY(EditAnywhere, Category = "Projectile|Material")
	UMaterial* ProjectileMaterial;

	UPROPERTY(EditAnywhere, Category = "Projectile|Material")
	FName PrimaryColorName;

	UPROPERTY(EditAnywhere, Category = "Projectile|Material")
	FName SecondaryColorName;

	UPROPERTY(EditAnywhere, Category = "Projectile|Material")
	FName ParticleLightColorName;

	UPROPERTY(EditAnywhere, Category = "Projectile|Material|HitVFX")
	FName SizeParameterName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Components")
	class USnakeProjectileMoveComponent* MoveComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Properties")
	UParticleSystem* HitVFX;

	/** Rotate the Hit VFX to match face the oppisite direction of the hit */
	UPROPERTY(EditAnywhere, Category = "Projectile|Properties")
	bool bOrientHitVFX;

	UPROPERTY(EditAnywhere, Category = "Projectile|Properties")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere, Category = "Projectile|Properties")
	float ExpectedHeight;

	/** Set the Attack Mode of the projectile and the Primary Color */
	UFUNCTION()
	void SetAttackMode(const FAttackType InAttackType);

	/** Trigger event when the attack type is set */
	UFUNCTION(BlueprintImplementableEvent, Category = "Projectile|Properties")
	void OnAttackTypeSet(FAttackType OutAttackType);

	/** Get the Attack type of the projectile */
	UFUNCTION(BlueprintPure, Category = "Projectile|Properties")
	FAttackType GetAttackType() const;

	/** Homing Acceleration Magnitude */
	UPROPERTY(EditAnywhere, Category = "Projectile|Properties")
	float HomingMagnitude;

protected:
	/** Color of the Particle Effect */
	FAttackType AttackType;

public:
	/** Set the Initial Velocity */
	UFUNCTION(BlueprintCallable, Category = "Projectile|Methods")
	void InitVelocity(FVector InDirection, float AdditionalVelocity = 0.0f);

	/** Set the Velocity. Only call this after first calling InitVelocity */
	UFUNCTION(BlueprintCallable, Category = "Projectile|Methods")
	void SetVelocity(FVector InDirection);

	/** Set the Speed Modifier. This will automaticly call SetVelocity using the stored direction */
	UFUNCTION()
	void SetSpeedModifier(float InModifier);

	/** Reset the Speed Modifier back to 1.0f */
	void ResetSpeedModifier();

	/** Get the Current Speed Modifier */
	float GetSpeedModifier() const;

	/** 
	* Add a Unique Instance of each object to thier MoveIgnoreActor Array
	* 
	* @param Actor that will be ignored. The Actor will also be set to ignore this object as well.
	*/
	UFUNCTION(BlueprintCallable, Category = "Projectile|Methods")
	void AddUniqueIgnoreActor(AActor* Actor);

	/** Call the MultiCast AddIgnoreActor from an external class */
	void MultiAddUniqueIgnoreActor(AActor* Actor);

	/** When this Projectile Collides with another actor */
	UFUNCTION()
	virtual void OnImpact(const FHitResult& HitResult);

	/** Take Damage Override. This calls Reduce Health */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DetlaTime) override;

	/** Update the Velocity on Client */
	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;

	virtual void PostNetReceiveLocationAndRotation() override;

	/** The Actor that fired this projectile */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Projectile|Owner")
	AActor* ProjectileOwner;

	/** Get the Player State of the Projectile owner */
	ASnakePlayerState* GetOwnerPlayerState() const;

	AController* GetOwnerController() const;

	UFUNCTION(BlueprintPure, Category = "Projectile|Team Number")
	int32 GetTeamNumber() const;

	UFUNCTION()
	virtual void BeginOverlapCheck(class AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void NotifyHit(class UPrimitiveComponent* MyComponent, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	/** Trigger a Height Check */
	void TriggerHeightCheck();

	/** Height Check Frequency */
	UPROPERTY(EditAnywhere, Category = "Projectile|LineTrace")
	float Frequency;

	/** Get Direction */
	FVector GetDirection();

	/** Set Direction */
	void SetDirection(FVector InDirection);

	/** Get Damage */
	int32 GetDamage();

	/** Set Damage */
	void SetDamage(int32 InDamage);

	/** Set Explosion Radius. Override this if the projectile supports AoE. */
	virtual void SetExplosionRadius(float InRadius);

	virtual float GetExplosionRadius() const;

	/** Set Bounce Count */
	virtual void SetBounceCount(int32 InBounceCount);

	/** Set Slow Percent. Override this if the projectile supports Slow Effects */
	virtual void SetSlowPercent(float InPercent);

protected:
	/** Damage the Projectile can deal */
	UPROPERTY(BlueprintReadWrite, Category = "Projectile|Properties")
	int32 Damage;

	/** Current Direction of this projectile */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Projectile|Properties")
	FVector Direction;

	/** Additional Projectile Speed */
	UPROPERTY(Replicated)
	float AdditionalSpeed;

	/** Modify the Projectile Speed */
	UPROPERTY(Replicated)
	float SpeedModifier;

	/** Hide and Destory this actor after a short delay */
	virtual void HideAndDestroy(const FHitResult& Hit = FHitResult());

	/** Get the Rotation angle of the Hit before calling the multicast */
	void OnHitVFX(const FHitResult& Hit);

	/** Check the Projectiles Height */
	void HeightCheck();

	/** Did the Linetrace hit anything last frame? */
	bool bLastHit;

	/** Height Check Progress */
	float TraceProgress;

	/** Has the Pitch been locked? Pitch is locked when it equals 0.0f */
	bool bPitchLocked;

	/** Timer Handle */
	FTimerHandle TimerHandle;

	/** Enable Collision */
	void EnableCollision();

	/** Pending Destory */
	bool bPendingDestroy;

	UPROPERTY(EditAnywhere, Category = "Projectile|SelfDestruct")
	bool bSelfDestruct;

	/** Flag for the projectile in falling state */
	bool bInAir;

	/** Progress till the Projectile self destructs */
	float SelfDestructProgress;

	/** Duration before projectile self destructs */
	UPROPERTY(EditAnywhere, Category = "Projectile|SelfDestruct")
	float SelfDestructDuration;

public:
	/** Pass in the Target Component. This class initializes teh Homing Components in the Movement Component */
	virtual void InitializeHoming(USceneComponent* InTargetComponent, float IdealDistance);

protected:
	bool bHomingProjectile;

public:
	/** Player the Assigned Audio  */
	void PlayAudio(UAudioComponent* AudioComponent);

	/** Stop the Current Audio */
	void StopAudio(UAudioComponent* AudioComponent);

protected:

	/** [Server + Client] Remove the Collision */
	UFUNCTION(Reliable, NetMultiCast, WithValidation)
	void MultiCastRemoveCollision();
	virtual void MultiCastRemoveCollision_Implementation();
	virtual bool MultiCastRemoveCollision_Validate() { return true; }

	/** [Server + Client] Turn on the Collision */
	UFUNCTION(Reliable, NetMultiCast, WithValidation)
	void MultiCastEnableCollision();
	virtual void MultiCastEnableCollision_Implementation();
	virtual bool MultiCastEnableCollision_Validate() { return true; }

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerHideAndDestroy(const FHitResult& Hit);
	virtual void ServerHideAndDestroy_Implementation(const FHitResult& Hit);
	virtual bool ServerHideAndDestroy_Validate(const FHitResult& Hit) { return true; }

	/** [Server + Client] Add the actor to the MoveIgnoreActors */
	UFUNCTION(Reliable, NetMultiCast, WithValidation)
	void MultiCastAddIgnoreActor(AActor* Actor);
	virtual void MultiCastAddIgnoreActor_Implementation(AActor* Actor);
	virtual bool MultiCastAddIgnoreActor_Validate(AActor* Actor) { return true; }

	/** Spawn a Hit Effect */
	UFUNCTION(Reliable, NetMultiCast, WithValidation)
	void MultiCastOnHitVFX(const FRotator Rotation);
	virtual void MultiCastOnHitVFX_Implementation(const FRotator Rotation);
	virtual bool MultiCastOnHitVFX_Validate(const FRotator Rotation) { return true; }

	/** [Server] Set Direction */
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSetDirection(FVector InDirection);
	virtual void ServerSetDirection_Implementation(FVector InDirection);
	virtual bool ServerSetDirection_Validate(FVector InDirection) { return true; }

};
