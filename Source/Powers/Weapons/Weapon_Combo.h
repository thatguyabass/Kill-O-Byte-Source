// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Powers/Weapons/Weapon_Multi.h"
#include "Weapon_Combo.generated.h"

UENUM()
enum class EFireType : uint8
{
	Single,
	Multi
};

USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

	FWeaponData() 
	{
		FireRate = 0.5f;
		Type = EFireType::Single;
		ProjectileCount = -1;
		RotationSpread = 0.0f;
		Damage = 1;
		FireDirection = EFireDirection::Forward;
	}
	~FWeaponData() {}

	UPROPERTY(EditAnywhere, Category = "Data|Property")
	TSubclassOf<ABaseProjectile> Projectile;

	UPROPERTY(EditAnywhere, Category = "Data|Property")
	float FireRate;

	UPROPERTY(EditAnywhere, Category = "Data|Property")
	EFireType Type;

	UPROPERTY(EditAnywhere, Category = "Data|Property")
	int32 ProjectileCount;

	UPROPERTY(EditAnywhere, Category = "Data|Property")
	float RotationSpread;

	UPROPERTY(EditAnywhere, Category = "Data|Property")
	int32 Damage;

	UPROPERTY(EditAnywhere, Category = "Data|Property")
	EFireDirection FireDirection;

};

UCLASS()
class SNAKE_PROJECT_API AWeapon_Combo : public AWeapon_Multi
{
	GENERATED_BODY()

public:
	AWeapon_Combo();

	/** Array of projectiles that can be cycled through or selected from */
	UPROPERTY(EditAnywhere, Category = "Combo|Property")
	TArray<FWeaponData> Inventory;

	/** Gather Data and fire a projectile */
	void Fire(FVector FireOffset, int32 Index);

	/** Override the Server Fire Custom Method */
	virtual void ServerFireCustom_Implementation(const FVector& InLocations, const TArray<FRotator>& InRotations, int32 Index, TSubclassOf<ABaseProjectile> InProjectileClass, float InFireRate, int32 InDamage);

private:
	/** Fire a projectile from the inventory */
	void FireCombo(FVector FireOffset, int32 Index);

};