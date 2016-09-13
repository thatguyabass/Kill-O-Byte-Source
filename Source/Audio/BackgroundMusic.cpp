// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BackgroundMusic.h"


// Sets default values
ABackgroundMusic::ABackgroundMusic()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
	AudioComponent->bIgnoreForFlushing = false;
	AudioComponent->bIsMusic = true;
	AudioComponent->bCenterChannelOnly = true;
	AudioComponent->AttachTo(RootComponent);

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABackgroundMusic::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
}

void ABackgroundMusic::BeginPlay()
{
	FadeAudioIn();
	Super::BeginPlay();
}

void ABackgroundMusic::FadeAudioIn(float InFadeDuration)
{
	if (AudioComponent)
	{
		AudioComponent->FadeIn(InFadeDuration, 1.0f);
	}
}

void ABackgroundMusic::FadeAudioOut(float InFadeDuration)
{
	if (AudioComponent)
	{
		AudioComponent->FadeOut(InFadeDuration, 0.0f);
	}
}

void ABackgroundMusic::PlayNewAudio(USoundBase* NewAudio, float InFadeDuration)
{
	if (!NewAudio)
	{
		return;
	}
	
	NextSFX = NewAudio;
	LastSFX = AudioComponent->Sound;
	FadeAudioOut(InFadeDuration);

	SwapFadeDuration = InFadeDuration;

	GetWorldTimerManager().SetTimer(Swap_TimerHandle, this, &ABackgroundMusic::SwapAudio, InFadeDuration, false);
}

USoundBase* ABackgroundMusic::GetLastSFX()
{
	return LastSFX;
}

USoundBase* ABackgroundMusic::GetNextSFX()
{
	return NextSFX;
}

void ABackgroundMusic::SwapAudio()
{
	AudioComponent->SetSound(NextSFX);
	FadeAudioIn(SwapFadeDuration);
	
	GetWorldTimerManager().ClearTimer(Swap_TimerHandle);
}