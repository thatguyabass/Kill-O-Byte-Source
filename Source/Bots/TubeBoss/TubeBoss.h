// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TubeBoss.generated.h"

USTRUCT(BlueprintType)
struct FAttackPatternIndexes
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Indexes")
	TArray<int32> PatternIndexes;

	int32 GetLength() const
	{
		return PatternIndexes.Num();
	}
};

UCLASS()
class SNAKE_PROJECT_API ATubeBoss : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATubeBoss();

	/** Radius of the Tube */
	UPROPERTY(EditAnywhere, Category = "Properties")
	float Radius;

	/** Component Spread */
	UPROPERTY(EditAnywhere, Category = "Properties")
	float BaseSpread;

	/** Spread from the Weapon Placement components */
	UPROPERTY(EditAnywhere, Category = "Properties")
	float TargetSpread;

	virtual void PostInitializeComponents() override;

	/** Start the entire boss encounter. */
	UFUNCTION(BlueprintCallable, Category = "Start")
	void StartEncounter();

	UPROPERTY(BlueprintReadOnly, Category = "Start")
	bool bEncounterStarted;

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** Start Turret Movement */
	void StartMovement(class AWeakPoint* WeakPoint);

	/** Shift the Turrets when a Weak Point is cleared */
	void MoveTurrets(float DeltaTime);

	/** Positions on the tube the turrets can attack from */
	UPROPERTY(EditAnywhere, Category = "Attack Positions")
	TArray<float> AttackPositions;

	/** Attack Patterns Data Asset */
	UPROPERTY(EditAnywhere, Category = "Attack Patterns")
	class UTubeBossAttackPattern* AttackPattern;

	/** Attack Pattern Indexes, use to create custom groups of attacks */
	UPROPERTY(EditAnywhere, Category = "Attack Patterns")
	TArray<FAttackPatternIndexes> AttackPatternIndexes;

	/** If true, the neutralmovement will start. Else, it will be skipped */
	UPROPERTY(EditAnywhere, Category = "Neutral Movement")
	bool bUseNeutralMovement;

	/** Delay after a completed Attack Pattern */
	UPROPERTY(EditAnywhere, Category = "Neutral Movement")
	float NeutralDuration;

protected:
	UPROPERTY(EditAnywhere, Category = "Neutral Movement")
	TArray<float> NeutralPositions;

	/** elapsed time of the neutral move */
	float NeutralProgress;

	/** Total Elapsed time of hte neutral move */
	float NeutralProgressTotal;

	/** Bool Tracking flag */
	bool bNeutralInProgress;

	/** Initialize the Neutral movement variables */
	void StartNeutralMovement(int32 Index = 0);

	/** Execute the Neutral Movement */
	void NeutralMovement(float DeltaTime);

	/** Finish the Neutral Movement and Transition back to normal gameplay */
	void FinishNeutralMovement(bool bSkipAssignSegment = false);

	/** Neutral Initial Rotation */
	FRotator NeutralRotation;
	/** Neutral Target Rotation*/
	FRotator NeutralTargetRotation;

public:
	void SetNextAttackMove();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void NextSegment(int32 TurretIndex, int32 PositionIndex, float InMoveDuration = -1.0f, float MoveDelay = -1);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveToTurret(float DeltaTime);

	/** Duration of Total Actor Movement */
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveDuration;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float TurretMovementDelay;

protected:
	/** Current attack pattern index */
	FIntPoint APTrackingIndex;

	/** 2d int, tracks both attack pattern and attack move index */
	FIntPoint AttackPatternIndex;

	/** this Actors Initial Rotation */
	FRotator InitialActorRotation;

	/** This Actors Target Rotation */
	FRotator TargetActorRotation;

	/** Current Actor Move Progress */
	float MoveProgress;

	/** At the Turret Location */
	bool bAtTurret;

	FTimerHandle ActorMove_TimerHandle;

	/** Set a new Segment. Called from Timer Manager */
	void SetNextSegment();

public:
	/** Actor Movement duration */
	UPROPERTY(EditAnywhere, Category = "Movement")
	float TurretDuration;

protected:
	/** Rotation Target for Move */
	TArray<FRotator> TargetRotation;
	
	/** Initial Rotation as a base. */
	TArray<FRotator> InitialRotation;

	/** Current Moves progress */
	float TurretProgress;

	/** Turrets Movement Flag */
	bool bTurretsMoving;
	
	/** Index of the Destroyed Weak Point */
	int32 MovementIndex;

	/** Move the Scene Component to its initial Location And Rotation 
	*	@Param MyComponent, the Component that is being moved
	*	@Param MyActor, The actor that will be moved to the components relative location 
	*	@Param Index, The current index in the Initial Roation and the Target Rotation arrays 
	*/
	void MoveSceneComponent(USceneComponent* MyComponent, AActor* MyActor, int32 Index);

public:
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* WeaponPlacementComponent01;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* WeaponPlacementComponent02;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* WeaponPlacementComponent03;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* WeaponPlacementComponent04;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* WeakPointPlacementComponent01;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* WeakPointPlacementComponent02;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* WeakPointPlacementComponent03;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* TurretMoveTarget01;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* TurretMoveTarget02;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* TurretMoveTarget03;

	/** All the Scene Components attached to this actor */
	UPROPERTY(BlueprintReadOnly, Category = "Components")
	TArray<USceneComponent*> SceneComponents;

	/** Scene Components for weapon placement */
	UPROPERTY(BlueprintReadOnly, Category = "Components")
	TArray<USceneComponent*> WeaponPlacements;

	/** Scene Components for Weak Point Placement */
	UPROPERTY(BlueprintReadOnly, Category = "Components")
	TArray<USceneComponent*> WeakPointPlacements;

	/** Scene Components for Turret Targets */
	UPROPERTY(BlueprintReadOnly, Category = "Components")
	TArray<USceneComponent*> TurretTargets;

	/** Turrents on the outside of the tube */
	UPROPERTY(BlueprintReadWrite, Category = "Turret")
	TArray<class ATubeTurret*> Turrets;

	/** Turrets Attack Type */
	UPROPERTY(EditAnywhere, Category = "Turret")
	TArray<class UAttackTypeDataAsset*> TurretsAttackType;

	/** Weak point actors between the turrets */
	UPROPERTY(BlueprintReadWrite, Category = "Weak Point")
	TArray<class AWeakPoint*> WeakPoints;

	/** Weak points attack type. */
	UPROPERTY(EditAnywhere, Category = "Weak Point")
	TArray<class UAttackTypeDataAsset*> WeakPointsAttackType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	class ATubeTrigger* TubeTrigger;

protected:
	UFUNCTION(BlueprintCallable, Category = "Initialize")
	void InitializePlacement();

	void SetLocationAndRotation(TArray<USceneComponent*>& Comp, FRotator Start, FRotator Spread);
	void AdjustTurretTargets();

	UFUNCTION(BlueprintCallable, Category = "Initialize")
	void InitializeTurrets();

	UFUNCTION(BlueprintCallable, Category = "Initialize")
	void InitializeWeakPoints();

private:
	static const float YawRotation;

public:
	/** Set the health And pass it to the attached turrets */
	void SetHealth(int32 InHealth);

	UFUNCTION(BlueprintPure, Category = "Health")
	int32 GetMaxHealth() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	int32 GetHealth() const;

	/** Reduce the Health of the Boss */
	void ReduceHealth(int32 InDamage, AActor* DamageCauser);

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const;

	void HideAndDestroy();

protected:
	/** Death Flag */
	bool bDead;

	/** Mark this boss as dead */
	void Dead();

	/** Total health of the boss. This is devided evenly between the turrets */
	UPROPERTY()
	int32 MaxHealth;

	/** Current Health */
	int32 Health;

	UPROPERTY(EditAnywhere, Category = "TubeBoss|Stats")
	FName TubeDataRow;

	UPROPERTY()
	UDataTable* TubeDataTable;

	static const FString TubeDataTableContext;

};
