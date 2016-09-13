// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/Engine/Classes/Sound/SoundCue.h"
#include "GameFramework/Actor.h"
#include "BackgroundMusic.generated.h"

UCLASS()
class SNAKE_PROJECT_API ABackgroundMusic : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABackgroundMusic();

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	UAudioComponent* AudioComponent;

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void FadeAudioIn(float InFadeDuration = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void FadeAudioOut(float InFadeDuration = 1.0f);

	UPROPERTY(EditAnywhere, Category = "Audio")
	float FadeDuration;

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayNewAudio(USoundBase* NewAudio, float InFadeDuration);

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Audio")
	USoundBase* GetLastSFX();

	UFUNCTION(BlueprintPure, Category = "Audio")
	USoundBase* GetNextSFX();

protected:
	/** the Next Music SFX */
	UPROPERTY()
	USoundBase* NextSFX;
	
	/** the Last Music SFX */
	UPROPERTY()
	USoundBase* LastSFX;

	UPROPERTY()
	float SwapFadeDuration;

	UFUNCTION()
	void SwapAudio();

	FTimerHandle Swap_TimerHandle;

};
