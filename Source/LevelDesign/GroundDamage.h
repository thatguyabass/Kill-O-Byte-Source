// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GroundDamage.generated.h"

USTRUCT(BlueprintType)
struct FGroundDamageContainer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Content")
	AActor* InActor;

	UPROPERTY(BlueprintReadWrite, Category = "Content")
	bool bCanBeDamaged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
	int32 TickCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
	int32 TickCountTotal;

	void CountDamageTick();

	void ResetTickCount()
	{
		TickCount = 0;
	}

	bool IsValid() const;

	FGroundDamageContainer(AActor* _InActor)
	{
		InActor = _InActor;
		bCanBeDamaged = false;
		TickCount = 0;
		TickCountTotal = 8;
	}

	FGroundDamageContainer()
	{
		FGroundDamageContainer(nullptr);
	}
};

UCLASS()
class SNAKE_PROJECT_API AGroundDamage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGroundDamage();

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UBoxComponent* OverlapComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UDecalComponent* DecalComponent;

	UPROPERTY(EditAnywhere, Category = "GroundDamage|Properties")
	TSubclassOf<UDamageType> DamageTypeClass;

	/** Damage Percent of the actors max health applied as damage */
	UPROPERTY(EditAnywhere, Category = "GroundDamage|Properties")
	float DamagePercent;

	/** Damage modifier if the actors are using the same attack mode */
	UPROPERTY(EditAnywhere, Category = "GroundDamage|Properties")
	float SameColorDamagePercent;

	/** Frequency the damage is applied per second. Value is in Seconds */
	UPROPERTY(EditAnywhere, Category = "GroundDamage|Properties")
	float DamageFrequency;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GroundDamage|Properties")
	TArray<class UAttackTypeDataAsset*> AttackTypeDataAssets;

	/** If true, the AttackTypeIndex must be manually set. Else, it will be randomly chossen based on the number of AttackTypeDataAssets count */
	UPROPERTY(EditAnywhere, Category = "GroundDamage|Properties")
	bool bManualSet;

	/** Index of the current Attack Type */
	UPROPERTY(EditAnywhere, Category = "GroundDamage|Properties")
	int32 AttackTypeIndex;

	/** Override the Default AttackType Array. */
	void SetAttackType(class UAttackTypeDataAsset* InAttackDataAsset);

	UFUNCTION(BlueprintPure, Category = "GroundDamage")
	FAttackType GetAttackType();

	UFUNCTION(BlueprintPure, Category = "GroundDamage")
	class ASnakeLink* GetOverlappedPlayer();

protected:
	/** Progress till the next damage cycle */
	float DamageProgress;

	/** Damage the Actors that overlap this actor */
	void DamageOverlapedActors();

public:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION()
	void OnBeginOverlap(class AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(class AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	UPROPERTY(EditAnywhere, Category = "GroundDamage|Material")
	FName PrimaryDecalColorName;

	UPROPERTY(EditAnywhere, Category = "GroundDamage|Material")
	FName AlphaParameterName;

	UPROPERTY(EditAnywhere, Category = "GroundDamage|Material")
	FName GlowParameterName;

	UPROPERTY(EditAnywhere, Category = "GroundDamage|Material")
	float GlowValue;
	
	/** DMI for the Decal Component */
	UPROPERTY(BlueprintReadWrite, Category = "GroundDamage|Material")
	UMaterialInstanceDynamic* DMI;

	/** Actors that are currently inside the overlap. */
	UPROPERTY()
	TArray<FGroundDamageContainer> ActorsInOverlap;

	bool FindActorInOverlap(AActor* InActor);

	/** Set the Dynamic Material Instance */
	UFUNCTION(BLueprintCallable, Category = "GroundDamage|Material")
	void SetDMIColor();

public:
	UPROPERTY(EditAnywhere, Category = "GroundDamage|ColorFade")
	float ColorLerpDuration;

	UFUNCTION(BlueprintCallable, Category = "GroundDamage|ColorFade")
	void StartColorLerp(int32 NewIndex);

	/** AttackTypeIndex Setter. bManualSet needs to be true! */
	UFUNCTION(BlueprintCallable, Category = "GroundDamage|Control")
	void SetAttackTypeIndex(int32 InIndex);

	UFUNCTION(BlueprintPure, Category = "GroundDamage|Control")
	int32 GetAttackTypeIndex() const;

protected:
	UPROPERTY()
	FLinearColor InitialColor;

	UPROPERTY()
	FLinearColor NewColor;

	UPROPERTY()
	float ColorLerpProgress;

	UPROPERTY()
	bool bColorLerpInprogress;

public:
	/** How long the Fade Takes to complete. */
	UPROPERTY(EditAnywhere, Category = "GroundDamage|Fade Properties")
	float FadeDuration;

	/** Start the Fade and Activate the actor */
	UFUNCTION(BlueprintCallable, Category = "GroundDamage|Fade")
	void ShowAndActivate();

	/** Hide the Actor. Start the Fade Out */
	UFUNCTION(BlueprintCallable, Category = "GroundDamage|Fade")
	void Hide();

	/** Hide the Actor and set the Life Span */
	UFUNCTION(BlueprintCallable, Category = "GroundDamage|Fade")
	void HideAndDestroy();

protected:
	/** Current state of the Actor. If false, the actor doesn't deal any damage */
	UPROPERTY()
	bool bActive;
	
	UPROPERTY()
	bool bFadeInprogress;

	/** Current Fade Progress */
	UPROPERTY()
	float FadeProgress;

public:
	/** A Player character has entered the overlap */
	UFUNCTION(BlueprintImplementableEvent, Category = "GroundDamage|Widget")
	void OnPlayerEnteredOverlap();

	/** A Player character has left the overlap */
	UFUNCTION(BlueprintImplementableEvent, Category = "GroundDamage|Widget")
	void OnPlayerExitedOverlap();

	UFUNCTION(BlueprintImplementableEvent, Category = "GroundDamage|Widget")
	void OnAttackTypeChange();

	/** Return true, if the attack types are the same */
	UFUNCTION(BlueprintPure, Category = "GroundDamage|Widget")
	bool CompareAttackTypes(FAttackType InAttackType);

};
