// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SplineComponent.h"
#include "Bots/Bot/BaseBoss.h"
#include "CircleBoss.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API ACircleBoss : public ABaseBoss
{
	GENERATED_BODY()
	
public:
	ACircleBoss();
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USplineComponent* SplineComponent01;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USplineComponent* SplineComponent02;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* WallWeakPointPlacement;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* BossWeakPointPlacement;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* ArmPlacement01;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* ArmPlacement02;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UAudioComponent* WallCrushSFXComp;

	UPROPERTY(EditAnywhere, Category = "Properties")
	TSubclassOf<class ACircleBossWall> WallActorClass;

	/** Height offset of the actors from the spline. Prevent half the actor from clipping through the floor */
	UPROPERTY(EditAnywhere, Category = "Properties")
	float HeightOffset;

	/** Distance the Spline points will be placed in the world */
	UPROPERTY(EditAnywhere, Category = "Properties")
	float PointDistance;

	UPROPERTY(EditAnywhere, Category = "Properties")
	int32 SplinePointCount;

	UPROPERTY(EditAnywhere, Category = "Properties")
	TArray<class UAttackTypeDataAsset*> AttackTypes;

	/** Start the encounter with the boss */
	virtual void StartEncounter() override;

	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void HideAndDestroy() override;

public:
	void InitializeWeakPoints();

	/** Weak point of the walls */
	UPROPERTY(BlueprintReadWrite, Category = "Weak Points")
	class AWeakPoint* WallWeakPoint;

	/** Boss Weak Points. Hurting these will directly hurt the boss */
	UPROPERTY(BlueprintReadWrite, Category = "Weak Points")
	class AWeakPoint* BossWeakPoint;

	/** Force used to send the player flying after they have hit the weak point */
	UPROPERTY(EditAnywhere, Category = "End Phase")
	float ExplosionForce;

	/** Radius of the sphere check during the end phase explosion */
	UPROPERTY(EditAnywhere, Category = "End Phase")
	float ExplosionRadius;

	UFUNCTION(BlueprintPure, Category = "Weak Points")
	bool IsWallWeakPointDead() const;

	UFUNCTION(BlueprintCallable, Category = "Weak Points")
	void ResetWallWeakPoint();

	/** Blueprint Event Triggered when the Wall movment has initialized */
	UFUNCTION(BlueprintImplementableEvent, Category = "Weak Points")
	void OnWallWeakPointReset();

	/** Mark the Weak point as dead. */
	UFUNCTION(BlueprintCallable, Category = "Weak Points")
	void HideWallWeakPoint();

	UFUNCTION(BlueprintCallable, Category = "Weak Points")
	void RemoveBossWeakPointCollision();

	/** Blueprint Event triggered when the boss weakpoint loses colision */
	UFUNCTION(BlueprintImplementableEvent, Category = "Weak Points")
	void OnRemoveBossWeakPointCollision();

	UFUNCTION(BlueprintCallable, Category = "Weak Points")
	void EnableBossWeakPointCollision();

protected:
	/** @Param Direction - If the value is an odd number, the direction will be negative */
	void PlaceSplinePoints(USplineComponent* SplineComp, int32 Direction);

	static const int32 WallCount;

	/** Spawn the Wall actors */
	void SpawnWalls();

	UPROPERTY()
	TArray<class ACircleBossWall*> WallActors;

	UPROPERTY()
	TArray<class UMoveToLocation*> MoveToComps;

public:
	/** Index of the waypoint to move the wall down */
	UPROPERTY(EditAnywhere, Category = "Wall Properties")
	int32 DownIndex;

	/** Index of the waypoint to move the wall up */
	UPROPERTY(EditAnywhere, Category = "Wall Properties")
	int32 UpIndex;

	UFUNCTION(BlueprintPure, Category = "Wall Phase")
	float GetTravelTime() const;

	/** Move the walls to the default starting location and in the Down Position */
	UFUNCTION(BlueprintCallable, Category = "Wall Phase")
	void ResetWalls();

	UFUNCTION(BlueprintCallable, Category = "Wall Phase")
	void MoveWallsUp();

	UFUNCTION(BlueprintCallable, Category = "Wall Phase")
	void MoveWallsDown();

	/** Move the two walls along the two splines. Progress is used to determine the position on the spline */
	UFUNCTION(BlueprintCallable, Category = "Wall Phase")
	void MoveWallsAlongSpline(float Progress);

	/** Move the Supplied actor along the Spline */
	UFUNCTION(BlueprintCallable, Category = "Wall Phase")
	void MoveWallAlongSpline(float Progress, AActor* Wall, USplineComponent* SplineComp);

	/** Get the Spline's Length. Both splines are the same size */
	float GetSplineDistance() const;

protected:
	
	FTimerHandle HideWalls_TimerHandle;

	/** Mark all the waves as hidden */
	void HideWalls();

public:
	UFUNCTION(BlueprintCallable, Category = "Arms Phase")
	void AddArm(class ACircleBossArm* InArm);

	UFUNCTION(BlueprintCallable, Category = "Arms Phase")
	void ClearArms();

	UFUNCTION(BlueprintCallable, Category = "Arms Phase")
	void StartForwardArmSequence();

	UFUNCTION(BlueprintCallable, Category = "Arms Phase")
	void StartReverseArmSequence();

	/** Tif true, the arm phase is inprogress */
	UPROPERTY()
	bool bArmPhaseInprogress;

	UFUNCTION(BlueprintPure, Category = "Arms Phase")
	bool GetArmPhaseInprogress();

	/** End the Arm Phase early. Launch the player off of the boss */
	void EndArmPhase();

	/** Trigger the Arm Phase in the AI Controller */
	UFUNCTION(BlueprintCallable, Category = "Arms Phase")
	void TriggerArmPhase();

	/** Name in the Data Table that contains the Arm Health */
	UPROPERTY(EditAnywhere, Category = "Arms Phase")
	FName CircleBossArmRowName;

	/** How much health the arms have before they collapse down on the player */
	UPROPERTY()
	int32 ArmHealth;

	UPROPERTY(EditAnywhere, Category = "Arms Phase")
	int32 ArmDamage;

	/** Current Health of the Arms */
	UPROPERTY(BlueprintReadOnly, Category = "Arms Phase")
	int32 CurrentArmHealth;

	UFUNCTION(BlueprintPure, Category = "Arms Phase")
	int32 GetArmHealth();

	/** Reduce the Arm Health by 1 */
	UFUNCTION(BlueprintCallable, Category = "Arms Phase")
	void ReduceArmHealth();

	UFUNCTION(BlueprintCallable, Category = "Arms Phase")
	void ResetArmHealth();
 
protected:
	UPROPERTY()
	TArray<class ACircleBossArm*> Arms;

	/** Setup the Arms for the boss */
	void InitializeArms();

	/** Call the AI controller and set bArms in position to TRUE. */
	UFUNCTION()
	void OnArmSequenceFinished();

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Arms Phase")
	void OnChargeLazer();

	UFUNCTION(BlueprintImplementableEvent, Category = "Arms Phase")
	void OnFireLazer();

	UFUNCTION(BlueprintImplementableEvent, Category = "Arms Phase")
	void OnResetLazer();

	/** Fire the lazer and Damage the actors hit */
	void FireLazer();

	/** Size of the Lazers hit box */
	UPROPERTY(EditAnywhere, Category = "Lazer")
	float LazerBoxExtents;

	/** Offset from the Actors Forward Vector and Position */
	UPROPERTY(EditAnywhere, Category = "Lazer")
	float LazerPosition;

	UPROPERTY(EditAnywhere, Category = "Lazer|Debug")
	bool bDrawOverlapSweep;

	UPROPERTY(EditAnywhere, Category = "Lazer")
	int32 LazerDamage;

protected:
	UPROPERTY()
	TArray<AActor*> HitByLazer;

public:
	UPROPERTY()
	TArray<class ASingleBotSpawner*> BotSpawners;

	UPROPERTY()
	TArray<ABot*> Bots;

	/** How much the health will scale by during waves 2 and 3 */
	UPROPERTY(EditAnywhere, Category = "Arm Phase")
	float HealthScale;

	/** Current Wave. Used with the Bot Counts to spawn bots */
	UPROPERTY()
	int32 WaveCount;

	/** max Count of bots that can be alive each wave */
	UPROPERTY(EditAnywhere, Category = "Bot Spawners")
	TArray<int32> BotCounts;

	UFUNCTION(BlueprintCallable, Category = "Bot Spawners")
	void AddBotSpawner(class ASingleBotSpawner* InSpawner);

	UFUNCTION()
	void SpawnBots();

	/** Check if the bot is still alive. */
	UFUNCTION()
	void VerifyBotStatus();

	/** Tick the Bot related methods */
	void TickBotMethods();

	bool CanSpawnBots();

	/** Track the number of bots spawned. */
	UPROPERTY()
	int32 BotsSpawned;

protected:
	virtual void Dead(class ASnakePlayerState* Killer) override;

public:
	/** Force Field to protect the Weakpoint during normal gameplay */
	UPROPERTY()
	class AForceField* ForceField;

	UFUNCTION(BlueprintCallable, Category = "Force Field")
	void AddForceField(class AForceField* InForceField);

protected:
	/** Delay before the shield is reenabled */
	UPROPERTY()
	float ShieldDelay;

	FTimerHandle Shield_TimerHandle;

	UFUNCTION()
	void ActivateShield();

public:
	/** Play the Sound attached to the WallCrushSFXComponent */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayWallCrushSFX();

};
