// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "StatData.generated.h"

USTRUCT()
struct FDataTableName
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Name")
	FName RowName;

	/** All the Names used by the */
	UPROPERTY(VisibleDefaultsOnly, Category = "Name")
	TArray<FName> TableNames;

	/** Modify the Index to change the Name used */
	UPROPERTY(EditAnywhere, Category = "Name")
	int32 NameIndex;

	/** Select the name using the Name Index */
	void SelectName();

};

USTRUCT(BlueprintType)
struct FStatDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	int32 Health;

	FStatDataRow()
		: Health(0)
	{
	}
};

USTRUCT(BlueprintType)
struct FForceFieldDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	int32 ShieldHealth;

	/** Regeneration Tick Rate. How often per second the health is regenerated */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float RegenTickRate;

	/** Amount of health regenerated each regeneration tick */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
	int32 HealthPerTick;

	FForceFieldDataRow()
		: ShieldHealth(0)
		, RegenTickRate(0.5f)
		, HealthPerTick(0)
	{

	}
};

USTRUCT(BlueprintType)
struct FWeaponStatDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStats")
	int32 Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStats")
	float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStats")
	float ExplosionRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStats")
	float SlowPercent;

	FWeaponStatDataRow()
		: Damage(0)
		, FireRate(0.0f)
		, ExplosionRange(0.0f)
		, SlowPercent(0.0f)
	{

	}
};

USTRUCT(BlueprintType)
struct FLazerStatDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "LazerStats")
	int32 Damage;

	UPROPERTY(EditAnywhere, Category = "LazerStats")
	float Duration;

	FLazerStatDataRow()
		: Damage(0)
		, Duration(0.0f)
	{

	}
};

UCLASS()
class SNAKE_PROJECT_API UStatDataTable : public UObject
{
	GENERATED_BODY()

};