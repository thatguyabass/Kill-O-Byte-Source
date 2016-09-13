// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"
#include "GameFramework/Actor.h"
#include "WeakPoint.generated.h"


UCLASS()
class SNAKE_PROJECT_API AWeakPoint : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AWeakPoint();

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UBoxComponent* HitBox;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UAudioComponent* HitSFXComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UAudioComponent* DestroyedSFXComp;

	/** Amplify the Damage before applying it to the owner */
	UPROPERTY(EditAnywhere, Category = "Propteries")
	float DamageModifier;

	UPROPERTY(EditAnywhere, Category = "Properties")
	class UAttackTypeDataAsset* AttackTypeDataAsset;

	UFUNCTION(BlueprintCallable, Category = "Properties")
	void SetAttackType(class UAttackTypeDataAsset* InType);

	UFUNCTION(BlueprintCallable, Category = "Properties")
	void SetAttackTypeStruct(FAttackType InType);

	UFUNCTION(BlueprintImplementableEvent, Category = "Properties")
	void OnAttackTypeChange();

	UFUNCTION(BlueprintPure, Category = "Properties")
	uint8 GetAttackMode();

	/** does this object take damage or the owner? If True, this damage will take priority */
	UPROPERTY(EditAnywhere, Category = "Health")
	bool bCanTakeDamage;

	/** Does this weak point fragment when shot (Destroy Components on itself). Only valid if bCanTakeDamage is true */
	bool bCanFragment;

	/** How many objects can fragemtn off. Used for calculating the health thresholds */
	UPROPERTY(EditAnywhere, Category = "Health")
	int32 FragmentCount;

	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void Fragment();

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const;

	/** Mark For death */
	void Dead();

	/** Reset weak point to the default state. Owner is left unchanged */
	void ResetWeakPoint();

	/** Set the Health Value */
	void SetHealth(int32 InHealth);

	UFUNCTION(BlueprintPure, Category = "Health")
	int32 GetHealth();

	UFUNCTION(BlueprintPure, Category = "Health")
	int32 GetMaxHealth();

	/** This method will set the death flag, but will not call the death methods. USE DURING INITIALIZATION ONLY! */
	UFUNCTION()
	void SetDeathFlag(bool InFlag);

protected:
	/** Current Health of this actor */
	int32 Health;

	/** Max health of this actor */
	UPROPERTY()
	int32 MaxHealth;

	/** Reduce the Current health by InDamage */
	void ReduceHealth(int32 InDamage);

	/** Death Flag */
	bool bDead;

	/** Total Damage Taken, Used for the Fragment Calculation */
	int32 StoredDamage;

public:
	/** Initialize Component */
	virtual void PostInitializeComponents() override;

	/** Override the base TakeDamage Method */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, class AActor* DamageCauser) override;
	
	DECLARE_DELEGATE(FOnWeakPointTakeDamage);
	FOnWeakPointTakeDamage OnWeakPointTakeDamage;
	
	/** Set a new Owner of this Weak Point */
	void SetWeakPointOwner(AActor* InOwner);

	/** Hide and DESTROY this actor. Removes collision from hit box. */
	UFUNCTION(BlueprintCallable, Category = "Method")
	void HideAndDestroy();

	/** Hide Actor and Remove collision */
	UFUNCTION(BlueprintCallable, Category = "Method")
	void Hide();

	/** Blueprint Event triggered when the weakpoitn becomes hidden */
	UFUNCTION(BlueprintImplementableEvent, Category = "Weakpoint")
	void OnHide();

	/** Actor not hidden in game and collision enabled. */
	UFUNCTION(BlueprintCallable, Category = "Method")
	void Show();

	/** Blueprint Event when the Weakpoint becomes visible */
	UFUNCTION(BlueprintImplementableEvent, Category = "Weakpoint")
	void OnShow();

	/** Remove the collision from the colliders */
	UFUNCTION(BlueprintCallable, Category = "Method")
	void RemoveCollision();

	/** Event Delegate called during Hide And Destroy */
	DECLARE_DELEGATE_OneParam(FOnDestroyed, AWeakPoint*);
	FOnDestroyed OnDestroyed;

protected:
	/** Current Owner of the Weak point*/
	UPROPERTY()
	AActor* WeakPointOwner;

public:
	UPROPERTY(EditAnywhere, Category = "Material")
	FName PrimaryColorName;

	UPROPERTY(EditAnywhere, Category = "Material")
	FName SecondaryColorName;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Material")
	UMaterialInstanceDynamic* DMI;

	UFUNCTION(BlueprintCallable, Category = "Material")
	void SetDMIColor();

protected:
	UPROPERTY()
	UDataTable* WeakPointDataTable;

	UPROPERTY(EditAnywhere, Category = "Health")
	FName WeakPointDataRow;

	static const FString WeakPointDataTableContext;

public:
	UFUNCTION(BlueprintCallable, Category = "SFX")
	void PlayDestroySFX();

	/** Play a random hit SFX */
	UFUNCTION(BlueprintCallable, Category = "SFX")
	void PlayHitSFX();

	/** Array of Hit Cues */
	UPROPERTY(EditAnywhere, Category = "SFX")
	TArray<USoundBase*> HitSFXCues;

};
