// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CircleBossArm.generated.h"

UENUM(BlueprintType)
enum class EArmState : uint8
{
	Rest	= 0,
	Stage01 = 1,
	Stage02 = 2
};

UCLASS()
class SNAKE_PROJECT_API ACircleBossArm : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACircleBossArm();

	/** Root Component */
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* RootSceneComponent;

	/** Attachment for the upper arm components */
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* Attachment01;

	/** Atachment for the lower arm components */
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* Attachment02;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* DoorPlacement;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* BlockerPlacement;

	/** Upper arm static mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UStaticMeshComponent* UpperArmMesh;

	/** Lower arm static mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UStaticMeshComponent* LowerArmMesh;

	/** SFX component for the Upper Arm movement */
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UAudioComponent* UpperArmSFXComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UAudioComponent* LowerArmSFXComponent;

	/** Target rotation during stage 01 */
	UPROPERTY(EditAnywhere, Category = "Properties")
	FRotator UpperTargetRotation;

	/** Target Rotation during stage 02 */
	UPROPERTY(EditAnywhere, Category = "Properties")
	FRotator LowerTargetRotation;

	/** Travel Time for the Stage 01 */
	UPROPERTY(EditAnywhere, Category = "Properties")
	float UpperDuration;

	/** Travel Time for the Stage 02 */
	UPROPERTY(EditAnywhere, Category = "Properties")
	float LowerDuration;

protected:
	UPROPERTY()
	float Progress;

	UPROPERTY()
	float Duration;

	FRotator InitialUpperRotation;
	FRotator InitialLowerRotation;

	FRotator BaseInitialUpperRotation;
	FRotator BaseInitialLowerRotation;

	/** Target Rotation */
	FRotator InternalTargetRotation;

	/** the Arm is moving backwards through the states */
	bool bReverse;

public:
	virtual void PostInitializeComponents() override;
	
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** Play throguh the Movement sequence */
	UFUNCTION(BlueprintCallable, Category = "Control")
	void StartForwardSequence();

	/** Play through the movement sequence Reverse */
	UFUNCTION(BlueprintCallable, Category = "Control")
	void StartReverseSequence();

	/** Blueprint Event for when the movement starts */
	UFUNCTION(BlueprintImplementableEvent, Category = "Control")
	void MovementSequenceStarted();

	/** Blueprint Event for when the Movement has stopped */
	UFUNCTION(BlueprintImplementableEvent, Category = "Control")
	void MovementSequenceFinished();

	void LerpRotation(float Blend, FRotator Initial, FRotator Target, USceneComponent* Comp);
	
	UFUNCTION(BlueprintCallable, Category = "State")
	void SetState(EArmState InState);

	UFUNCTION(BlueprintCallable, Category = "State")
	EArmState GetState() const;

	/** Delegate event that fires when the movement sequence starts */
	DECLARE_DELEGATE(FOnSequenceStart);
	FOnSequenceStart OnSequenceStart;

	/** Delegate event that fires when the movement sequence has finished */
	DECLARE_DELEGATE(FOnSequenceFinish);
	FOnSequenceFinish OnSequenceFinish;

protected:
	/** Start Moving the arms into position */
	void StartSequence();

	/** Current Arm State */
	EArmState ArmState;

	/** Increment the State Value and set as active state */
	void IncreaseState();

	/** Decrement the state value and set as active state */
	void DecreaseState();

	static const int32 StateCount;

public:
	UFUNCTION(BlueprintCallable, Category = "Door")
	void SetDoor(class ABoundary* InDoor);

	UFUNCTION(BlueprintCallable, Category = "Door")
	void SetBlocker(class ABoundary* InBlocker);

protected:
	UPROPERTY()
	class ABoundary* Door;

	UPROPERTY()
	class ABoundary* Blocker;

public:
	void BossDead();

};
