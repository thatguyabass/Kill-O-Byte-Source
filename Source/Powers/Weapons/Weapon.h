// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Powers/Projectiles/BaseProjectile.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM()
enum class EFireDirection : uint8
{
	Forward,
	Back,
	Right,
	Left
};

UCLASS()
class SNAKE_PROJECT_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	UPROPERTY(EditAnywhere, Category = "Weapon|Property")
	TSubclassOf<ABaseProjectile> ProjectileClass;

	/** How long the Life span is when set to be destroyed */
	UPROPERTY(EditAnywhere, Category = "Weapon|Property")
	float DestructionLifeSpan;

	UFUNCTION()
	float GetFireRate() const;

	/** Override the Firerate aquired from the data table */
	void SetFireRate(float InFireRate);

protected:
	/** Damage that the Spawned projectiles can deal */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon|Property")
	int32 Damage;

	/** Time between weapon shots. Longer values equal a longer wait period */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon|Property")
	float FireRate;

	/** Radius of the Projectiles Impact. Not all projectiles support this */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon|Property")
	float Radius;

	/** Slow Percentage applied to hit actor. Not all projectiles support this */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon|Property")
	float SlowPercent;

public:
	/** Set Owner, Weapon Owner and Projectile Owner. Spawn Location is based on the Weapon Owner's Location */
	void SetWeaponOwner(AActor* InWeaponOwner, AActor* InProjectileOwner = nullptr);

	/** Set the object to be destoryed */
	void StartDestroy();

	/** Call the Weapon owner to signal they can shoot during destruction */
	FTimerHandle OwnerCanShoot_TimerHandle;

	/** Call to Fill Data */
	virtual void Fire(FVector FireOffset);

	/** Return bCanFire */
	bool CanFire();

	/** Override bCanFire. */
	void OverrideCanFire(bool Override);

	/** If true, a projectile is still being fired. IE. If a lazer is being fired, check and see if the lazer is still beign fired before switching weapons */
	virtual bool ProjectileBeingFired();

	/** Hack fix, Stop the lazers from being fired */
	virtual void TerminateProjectile();

	/** Delegate Triggered When a projectile has finished shooting */
	DECLARE_DELEGATE(FOnProjectileFinishedFire);

	/** Delegate Event when the Weapon is finished firing.
	*	@ Currently only tracks the Lazer Special Weapon. 
	*/
	FOnProjectileFinishedFire OnProjectileFinishedFire;

	/** Call to Fire Projectile */
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerFire(const FVector& InLocation, const TArray<FRotator>& InRotations, int32 Index = 1);
	virtual void ServerFire_Implementation(const FVector& InLocation, const TArray<FRotator>& InRotations, int32 Index);
	virtual bool ServerFire_Validate(const FVector& InLocation, const TArray<FRotator>& InRotations, int32 Index) { return true; }

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerFireCustomMulti(const FVector& InLocation, const TArray<FRotator>& InRotations, const TArray<TSubclassOf<ABaseProjectile>>& InProjectileClasses);
	virtual void ServerFireCustomMulti_Implementation(const FVector& InLocation, const TArray<FRotator>& InRotations, const TArray<TSubclassOf<ABaseProjectile>>& InProjectileClasses);
	virtual bool ServerFireCustomMulti_Validate(const FVector& InLocation, const TArray<FRotator>& InRotations, const TArray<TSubclassOf<ABaseProjectile>>& InProjectileClasses) { return true; }

	/** Call to fire a Projectile. */
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerFireCustom(const FVector& InLocation, const TArray<FRotator>& InRotations, int32 Index = 1, TSubclassOf<ABaseProjectile> InProjectileClass = nullptr, float InFireRate = 0.5f, int32 InDamage = 1);
	virtual void ServerFireCustom_Implementation(const FVector& InLocation, const TArray<FRotator>& InRotations, int32 Index, TSubclassOf<ABaseProjectile> InProjectileClass, float InFireRate, int32 InDamage);
	virtual bool ServerFireCustom_Validate(const FVector& InLocation, const TArray<FRotator>& InRotataions, int32 Index, TSubclassOf<ABaseProjectile> InProjectileClass, float InFireRate, int32 InDamage) { return true; }

protected:

	/** Get the initial Direction that the Projectile will be fired */
	FRotator GetInitialDirection();

	/** Direction State */
	EFireDirection FireDirection;

	/** Fire Weapon */
	void FireWeapon(const FVector& InLocation, const FRotator& InRotation);
	void FireWeapon(const FVector& InLocation, const TArray<FRotator>& InRotations, int32 Index);

	/** The Actor that owns this weapon */
	UPROPERTY(Replicated)
	AActor* WeaponOwner;

	/** The Actor that owns the fired projectile. Normaly this is also the Weapon Owner*/
	UPROPERTY(Transient, Replicated)
	AActor* ProjectileOwner;

	/** Projectile Spawn Location */
	FVector Location;

	/** Projectile Spawn Rotations */
	TArray<FRotator> Rotation;

	/** Can this projectile fire? */
	UPROPERTY(Replicated)
	bool bCanFire;

	/** Reset the Can Fire bool */
	void ResetCanFire();

	/** Time before the next projectile can be fired */
	float Progress;

	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

public:
	/** Set the Weapon to Fire Homing Projectiles */
	void SetHomingProjectile(AActor* Target, float Distance = 0.0f);

protected:
	/** Homing Projectiles Flag. Does this weapon fire homing projectiles? */
	bool bHomingProjectiles;

	/** Distance Benchmark. Projectile speed is adjusted using this value */
	float HomingDistance;

	/** Component Target for the Homing Projectiles */
	USceneComponent* HomingTarget;

protected:
	UPROPERTY(EditAnywhere, Category = "Weapon|Property")
	UDataTable* WeaponDataTable;

	/** Name of the Row in the Data table that this weapon will use */
	UPROPERTY(EditAnywhere, Category = "Weapon|Property")
	FName WeaponRow;

	static const FString WeaponDataContext;

	/** Initialize the Weapon Row Data and Fill the Varibles with the Data. */
	UFUNCTION()
	virtual void GetTableData();

};
