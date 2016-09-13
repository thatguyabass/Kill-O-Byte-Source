// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LazerBossHand.generated.h"

UENUM(BlueprintType)
enum class EHandSlamState : uint8
{
	Idle,//Not Slamming
	Windup,//Moving back, winding up the slam 
	PreSlam,//Delay before slamming down. 
	Unwind,//Moving hand into slam position
	MoveDown,//Hand moving towards the ground
	PostSlam,//After the Slam has occured 
	Reset//Reset back to the Idle Location/Rotation
};

UENUM(BlueprintType)
enum class EHandState : uint8
{
	Normal,
	Stunned
};

UCLASS()
class SNAKE_PROJECT_API ALazerBossHand : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALazerBossHand();

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* HandRestComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* AttachmentComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UBoxComponent* HitBox;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UAudioComponent* SlamAudioComponent;

	virtual void PostInitializeComponents() override;

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

public:
	/** Angle the Hand will reach when winding up */
	UPROPERTY(EditAnywhere, Category = "Slam|Properties")
	FRotator WindupTarget;

	/** Time it takes for the hand to windup */
	UPROPERTY(EditAnywhere, Category = "Slam|Properties")
	float WindupDuration;

	UPROPERTY(EditAnywhere, Category = "Slam|Properties")
	float PreSlamDuration;

	/** Time it takes for the hand to unwind */
	UPROPERTY(EditAnywhere, Category = "Slam|Properties")
	float UnwindDuration;

	/** Time it takes to slam down */
	UPROPERTY(EditAnywhere, Category = "Slam|Properties")
	float MoveDownDuration;

	UPROPERTY(EditAnywhere, Category = "Slam|Properties")
	float PostSlamDuration;
	
	UPROPERTY(EditAnywhere, Category = "Slam|Properties")
	float ResetDuration;

	/** Start the Slam Sequence */
	UFUNCTION(BlueprintCallable, Category = "Slam|Control")
	void StartSlam();

	/** Is the slam state not Idle? */
	UFUNCTION(BlueprintPure, Category = "Slam|State")
	bool IsSlamInprogress() const;

	/** Is the slam state in a state >= then MoveDown? */
	UFUNCTION(BlueprintPure, Category = "Slam|State")
	bool IsSlamDownInprogress() const;

	EHandSlamState GetSlamState() const;

protected:
	EHandSlamState SlamState;
	
	/** Increment the State by one. If the index exceeds the max, reset to 0 */
	UFUNCTION(BlueprintCallable, Category = "Slam|Control")
	void IncreaseSlamState();

	UFUNCTION(BlueprintCallable, Category = "Slam|Control")
	void SetSlamState(EHandSlamState InState);

	/** Handle and set the varibles for the new state */
	void HandleSlamState();

	void InitializeSlamRotation(FRotator TargetRotation);

	float WindupProgress;
	float UnwindProgress;

	/** Get and Set the Correct Slam Progress floats. use the SlamState to determine which variable is modified/retrieved */
	void SetSlamProgress(float DeltaTime);
	float GetSlamProgress();

	/** Reset the Slam Progress for the current state. */
	void ResetSlamProgress();

	/** Get the Slam Duration. Uses SlamState to determine which variable is retrieved */
	float GetSlamDuration();

	/** Rotate the Hand during both Windup and Unwind state of slam. */
	void RotateHandSlam(float DeltaTime);

	/** Rotation of the hand before windup state. */
	UPROPERTY()
	FRotator InitialHandRotation;

	/** Initial Rotation for the QLerp */
	FRotator InitialActorRotation;

	/** Target Rotation for the QLerp */
	FRotator TargetActorRotation;

	/** Move the Hand to the TargetActorLocation 
	@ Trigger Event - Event hack, if true trigger the OnSlamDown Event else ignore it */
	void SlamDown(float DeltaTime, bool TriggerEvent);

	/** Blueprint Event triggered when the hand has finished its slam */
	UFUNCTION(BlueprintImplementableEvent, Category = "LazerBossHand")
	void OnSlamDown();

	float PreSlamProgress;
	float MoveDownProgress;
	float PostSlamProgress;
	float ResetProgress;

	/** Location before Movement Lerp */
	FVector InitialActorLocation;

	/** Target Location for movement Lerp */
	FVector TargetActorLocation;

	/** Down Line Trace to check for any blocking volumes. If none are found, use this to offset the current location to get the target */
	UPROPERTY()
	int32 LineTraceHeight;

	void SlamTick(float DeltaTime);

public:
	DECLARE_DELEGATE(FOnSequenceFinished);
	FOnSequenceFinished OnSequenceFinished;

	//DECLARE_DELEGATE_OneParam(FOnStunned, ALazerBossHand*);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStunned, ALazerBossHand*, Hand);

	UPROPERTY(BlueprintAssignable, Category = "Custom Event")
	FOnStunned OnStunned;

	DECLARE_DELEGATE_OneParam(FOnNormal, ALazerBossHand*);
	FOnNormal OnNormal;

public:
	UFUNCTION(BlueprintCallable, Category = "Hand|State")
	void SetHandState(EHandState InState);

	/** Is the Hand in the Normal State? */
	UFUNCTION(BlueprintPure, Category = "Hand|State")
	bool IsNormal() const;

	/** Is the Hand Stunned */
	UFUNCTION(BlueprintPure, Category = "Hand|State")
	bool IsStunned() const;

protected:
	EHandState HandState;

public:
	UPROPERTY(EditAnywhere, Category = "Hand|Stats")
	FName DataTableRowName;

protected:
	static const FString DataTableContext;

	UPROPERTY()
	UDataTable* StatDataTable;

public:
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/** Mark the hand as dead */
	void Dead();

	UFUNCTION()
	void OnBeginOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

protected:
	/** Dead Flag. If true, this hand is dead. The hand should only be dead if called from the head. */
	bool bDead;

	UPROPERTY()
	int32 MaxHealth;

	UPROPERTY()
	int32 Health;

	UFUNCTION()
	void SetHealth(int32 InHealth);

	UFUNCTION()
	void AddHealth(int32 InHealth);

	UFUNCTION()
	void ReduceHealth(int32 Value);

public:
	UFUNCTION(BlueprintPure, Category = "Hand|Health")
	int32 GetHealth() const;

	UFUNCTION(BlueprintPure, Category = "Hand|Health")
	int32 GetMaxHealth() const;

public:
	void SetBossState(uint8 InBossState);

	UFUNCTION(BlueprintPure, Category = "Hand|Animation")
	uint8 GetBossState();

	UFUNCTION(BlueprintImplementableEvent, Category = "Hand|Animation")
	void OnStateChange(uint8 LastState, uint8 NewState);

	/** Trigger the Idle Animation */
	UFUNCTION(BlueprintImplementableEvent, Category = "Hand|Animation")
	void TriggerIdle();

protected:
	uint8 BossState;

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Hand|Destroy")
	void HideAndDestroy();
	virtual void HideAndDestroy_Implementation();

	UPROPERTY(EditAnywhere, Category = "Hand|Destroy")
	float DestroyTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bot|Material")
	FName FractureColorName;

	/** Dissolve Blend Param Name */
	UPROPERTY(EditAnywhere, Category = "Bot|Material|Dissolve")
	FName DissolveBlendName;

	/** Dissolve Outline Param Name */
	UPROPERTY(EditAnywhere, Category = "Bot|Material|Dissolve")
	FName DissolveOutlineWidthName;

	UPROPERTY(EditAnywhere, Category = "Bot|Material|Dissolve")
	float DissolveOutlineWidth;

	/** Total time for the duration to complete from start to end */
	UPROPERTY(EditAnywhere, Category = "Bot|Material|Dissolve")
	float DissolveDuration;

	/** Delay before the dissolve takes place */
	UPROPERTY(EditAnywhere, Category = "Bot|Material|Dissolve")
	float DissolveStartDelay;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Hand|Material")
	TArray<UMaterialInstanceDynamic*> DMI;

	/** if true, Dissolve is in progress */
	bool bDissolveInprogress;

	/** Total time expired of the dissolve process */
	float DissolveProgress;

	FTimerHandle DissolveDelay_TimerHandle;

	/** Start Dissolving the Hand */
	UFUNCTION()
	void StartDissolve();

	UFUNCTION()
	void SetDissolveBlend();
};
