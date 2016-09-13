// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "TrapTrigger.generated.h"

UCLASS()
class SNAKE_PROJECT_API ATrapTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrapTrigger();

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UBoxComponent* BoxComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USplineComponent* Spline;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UParticleSystemComponent* FuzeComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UAudioComponent* AudioSingleComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UAudioComponent* AudioLoopComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	TArray<class AActor*> ActivatedActors;

	/** Time before the Event fires after being triggered */
	UPROPERTY(EditAnywhere, Category = "Properties")
	float Delay;

	/** if true, this can be triggered multiple times */
	UPROPERTY(EditAnywhere, Category = "Properties")
	bool bMultiActivate;

	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void BeginOverlapCheck(class AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Blueprint Event executed when triggered */
	UFUNCTION(BlueprintImplementableEvent, Category = "Event")
	void EventTriggeredStart();

	/** Blueprint Event executed after delay */
	UFUNCTION(BlueprintImplementableEvent, Category = "Event")
	void EventTriggered();

	UFUNCTION(BlueprintImplementableEvent, Category = "Event")
	void ResetTrigger();

	/** Reset bHasBeenActivated */
	UFUNCTION(BlueprintCallable, Category = "Control")
	void Reset();

	/** Has this trigger been activated? */
	UFUNCTION(BlueprintCallable, Category = "Control")
	bool HasBeenActivated() const;

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlaySingleSFX(USoundBase* inSFX);

private:
	void Trigger(AActor* Actor);

	/** Call the Trigger Event to execute in blueprint */
	void ExecuteTriggerEvent();

	/** Delay progress till the traps are triggered */
	float Progress;

	FTimerHandle EventTrigger_TimerHandle;

	/** If true, this trigger has been activated. */
	bool bHasBeenActivated;

};
