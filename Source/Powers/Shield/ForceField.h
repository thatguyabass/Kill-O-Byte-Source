// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ForceField.generated.h"

UCLASS()
class SNAKE_PROJECT_API AForceField : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AForceField();

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	class USceneComponent* RootComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UAudioComponent* AudioComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UAudioComponent* HitAudioComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** Get the team number from the shields owner */
	int32 GetTeamNumber() const;

public:
	/** Start the ForceField. */
	void ActivateForceField();

	/** Shutdown the Forcefield regardless of current health */
	void DeactivateForceField();

	/** is the Force Field Currently Active? If false, the shield will Transition to the Hidden State */
	UPROPERTY(EditAnywhere, Category = "Shield|Properties")
	bool bIsActive;

public:
	UPROPERTY()
	int32 MaxHealth;

	/** Time it takes for the Shield to reactivate After being shutdown */
	UPROPERTY(EditAnywhere, Category = "Shield|Health")
	int32 RechargeTime;

	/** Can this shield regenerate Health over time? */
	UPROPERTY(EditAnywhere, Category = "Shield|Health")
	bool bCanRegen;

	/** Delay before the Regen takes place, this will be reset if damage is taken */
	UPROPERTY(EditAnywhere, Category = "Shield|Health")
	float DamageDelay;

	/** Health Regen Incrament. How much health is restored per HealthRegen Tick */
	UPROPERTY()
	int32 HealthRegen;

	UPROPERTY()
	float RegenTickRate;

	/** Damage Override */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/** Reduce Health by the Damage Value, Ensures damage is valid
	*	Return the the damage difference after being applied to the health */
	float ReduceHealth(int32 Damage);

	/** Add value to the Current Health */
	void AddHealth(int32 Value);

	/** Set the health value to Value */
	void SetHealth(int32 Value);

	UFUNCTION(BlueprintPure, Category = "Shield|Health")
	int32 GetHealth() const;

	UFUNCTION(BlueprintPure, Category = "Shield|Health")
	int32 GetMaxHealth() const;

	/** Remove Collision and Play the Shutdown Timeline */
	void Shutdown(bool PlayTimeline);

	void HideAndDestroy(bool PlayTimeline);

	/** Delegate Event when the force field has been recharged */
	DECLARE_DELEGATE(FOnRestart);
	FOnRestart OnRestart;

	/** Delegate event when the Force Field has been shutdown. */
	DECLARE_DELEGATE(FOnShutdown);
	FOnShutdown OnShutdown;

	/** Restart the Shield
	*	@ Reset - Reset the Sheilds back to max health
	*/
	void Restart(bool Reset = true);

	/** Has the shield been depleted? */
	bool IsShutdown() const;

protected:
	/** Current Health of the Shield */
	UPROPERTY()
	int32 Health;

	/** Is the Shield Shutdown Flag */
	bool bShutdown;

	FTimerHandle Recharge_TimerHandle;
	FTimerHandle Hide_TimerHandle;
	FTimerHandle Regen_TimerHandle;
	FTimerHandle RegenTick_TimerHandle;

	void InternalRestart();

	/** Collision Toggle Functions */
	UFUNCTION(BlueprintNativeEvent, Category = "Mesh")
	void EnableMeshCollision();
	virtual void EnableMeshCollision_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Mesh")
	void DisableMeshCollision();
	virtual void DisableMeshCollision_Implementation();

public:
	/** Hide the actor ingame AND remove collision */
	void Hide();

	/** Hide the Actor after playing the fade out timeline */
	void HideWithTimeline();
	
	/** Make the Actor Visible and add collision */
	void Show();

	/** Enable visiblity with the Timeline */
	void ShowWithTimeline();

private:
	/** Tick the Regen Flag. If true, RegenHealth is ticking */
	bool bRegenTick;

	void StartRegen();
	void StopRegen();

	/** Regenerate Health for the Shield */
	void RegenHealth();

public:
	UPROPERTY(EditAnywhere, Category = "Shield|Properties")
	class UAttackTypeDataAsset* AttackTypeDataAsset;

	UPROPERTY(EditAnywhere, Category = "Shield|Properties")
	bool bUsePrimaryColor;

	/** Set the Shield Owner, and Attach the shield to the Owner Keeping the World Position */
	void SetShieldOwner(AActor* InOwner);

	/** Get the Actor that owns this shield. */
	UFUNCTION(BlueprintPure, Category = "Shield|Owner")
	AActor* GetShieldOwner() const;

	void SetAttackType(struct FAttackType InAttackType);
	struct FAttackType GetAttackType();

protected:
	/** The actor that owns this Shield */
	AActor* ShieldOwner;

	class ASnakePlayerState* GetPlayerState();

	/** Current Attack Type of the Shield */
	struct FAttackType AttackType;

public:
	UPROPERTY(EditAnywhere, Category = "Shield|Dynamic Material")
	FName PrimaryColorParameterName;

	UPROPERTY(EditAnywhere, Category = "Shield|Dynamic Material")
	FName AlphaParameterName;

	UPROPERTY(EditAnywhere, Category = "Shield|Dynamic Material")
	FName GlowParameterName;

	UPROPERTY(EditAnywhere, Category = "Shield|Dynamic Material")
	float GlowValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield|Hit Timeline")
	UCurveFloat* HitCurve;

	UPROPERTY(EditAnywhere, Category = "Shield|Hit Timeline")
	float HitTimelineLength;

	UPROPERTY(EditAnywhere, Category = "Shield|Shutdown Timeline")
	UCurveFloat* ShutdownCurve;

	UPROPERTY(EditAnywhere, Category = "Shield|Shutdown Timeline")
	float ShutdownTimelineLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield|Regen Timeline")
	UCurveFloat* GlowCurve;

	UPROPERTY(EditAnywhere, Category = "Shield|Regen Timeline")
	float GlowTimelineLength;

	/** Set the Alpha of the Shield's Dynamic Material Instance */
	UFUNCTION(BlueprintCallable, Category = "Shield|Timeline")
	void SetShieldAlpha(float Alpha);

	/** Set the glow of the shield's Dynamic Material Instance */
	UFUNCTION(BlueprintCallable, Category = "Shield|Timeline")
	void SetShieldGlow(float Glow);

protected:
	/** Timelines */
	FTimeline HitTimeline;
	FTimeline ShutdownTimeline;
	FTimeline GlowTimeline;

	/** Material attached to this Actor */
	UMaterialInstanceDynamic* DMI;

public:
	UPROPERTY(EditAnywhere, Category = "Shield|Properties")
	UDataTable* ForceFieldDataTable;

	UPROPERTY(EditAnywhere, Category = "Shield|Properties")
	FName ForceFieldRowName;

	static const FString ForceFieldContext;

public:
	UPROPERTY(EditAnywhere, Category = "Shield|SFX")
	USoundBase* RegenSFX;

	UPROPERTY(EditAnywhere, Category = "Shield|SFX")
	USoundBase* ShutdownSFX;

	UFUNCTION()
	void PlaySFX(USoundBase* InSFX);

};