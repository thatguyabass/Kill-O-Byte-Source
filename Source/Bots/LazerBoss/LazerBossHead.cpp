// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "LazerBossHead.h"
#include "Bots/LazerBoss/LazerBossAI.h"
#include "Bots/LazerBoss/LazerBossHand.h"
#include "Powers/Weapons/Weapon_Special.h"
#include "Utility/AttackTypeDataAsset.h"

ALazerBossHead::ALazerBossHead()
	: Super()
{
	GetCapsuleComponent()->SetCollisionProfileName("CharacterCapsule_Pawn");
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_SnakeTraceChannel, ECR_Ignore);

	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	HitBox->AttachTo(RootComponent);
	HitBox->SetBoxExtent(FVector(250.0f));
	HitBox->SetCollisionProfileName("Pawn");
	HitBox->SetCollisionResponseToChannel(ECC_SnakeTraceChannel, ECR_Ignore);
	HitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	HandPlacement01 = CreateDefaultSubobject<USceneComponent>(TEXT("HandPlacement01"));
	HandPlacement02 = CreateDefaultSubobject<USceneComponent>(TEXT("HandPlacement02"));

	HandPlacement01->AttachTo(RootComponent);
	HandPlacement02->AttachTo(RootComponent);

	HandPlacements.Add(HandPlacement01);
	HandPlacements.Add(HandPlacement02);

	LazerChargingComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	LazerChargingComponent->AttachTo(RootComponent);
	LazerChargingComponent->Deactivate();

	LazerChargeAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("LazerChargeAudioComponent"));
	LazerChargeAudioComp->bAlwaysPlay = false;
	LazerChargeAudioComp->bAutoActivate = false;
	LazerChargeAudioComp->AttachTo(LazerChargingComponent);

	OneOffAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("OneOffAudioComponent"));
	OneOffAudioComp->bAlwaysPlay = false;
	OneOffAudioComp->bAutoActivate = false;
	OneOffAudioComp->AttachTo(RootComponent);

	HandInterpSpeed = 15.0f;
	HandInterpRotationSpeed = 0.05f;

	LazerInterpSpeed = 30.0f;
	LazerInterpRotationSpeed = 1.0f;

	LazerScale = 1.0f;
	LazerRotationProgress = 0.0f;
	LazerRotationDuration = 10.0f;

	MinScaleTimelineValue = 0.75f;
	MaxScaleTimelineValue = 5.0f;
	LazerRadiusRange.X = 400.0f;
	LazerRadiusRange.Y = 250.0f;
	FireLazerAt = 8.0f;

	bDead = false;
	bEncounterStarted = false;

	CyclePhaseCount = 3;
	DamageTaken = 0.0f;

	NormalHandPitch = 0.0f;
	StunnedHandPitch = 90.0f;
}

void ALazerBossHead::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	LazerChargingComponent->DeactivateSystem();

	SpawnHands();
	SpawnLazer();

	HandRelativeTransforms.Empty();
	for (int32 c = 0; c < HandPlacements.Num(); c++)
	{
		FTransform Trans = HandPlacements[c]->GetRelativeTransform();
		HandRelativeTransforms.Add(Trans);
	}

	InitializeMovementState();

	if (ScaleCurve)
	{
		ScaleTimeline = FTimeline();
		FOnTimelineFloatStatic Func;

		Func.BindUObject(this, &ALazerBossHead::SetLazerScale);
		ScaleTimeline.AddInterpFloat(ScaleCurve, Func);
		
		FOnTimelineEvent Event;
		Event.BindUFunction(this, "FireLazer");
		ScaleTimeline.AddEvent(FireLazerAt, Event);

		FOnTimelineEventStatic FinishEvent;
		FinishEvent.BindUObject(this, &ALazerBossHead::EndLazerState);
		ScaleTimeline.SetTimelineFinishedFunc(FinishEvent);

		ScaleTimeline.SetTimelineLength(ScaleTimelineLength);
		ScaleTimeline.SetLooping(false);
	}
}

void ALazerBossHead::SpawnHands()
{
	if (RightHandClass)
	{
		FVector Location = HandPlacement01->GetComponentLocation();
		FRotator Rotation = HandPlacement01->GetComponentRotation();

		ALazerBossHand* NewHand = GetWorld()->SpawnActor<ALazerBossHand>(RightHandClass, Location, Rotation);
		if (NewHand)
		{
			Hands.Add(NewHand);
		}
	}
	if (LeftHandClass)
	{
		FVector Location = HandPlacement02->GetComponentLocation();
		FRotator Rotation = HandPlacement02->GetComponentRotation();

		ALazerBossHand* NewHand = GetWorld()->SpawnActor<ALazerBossHand>(LeftHandClass, Location, Rotation);
		if (NewHand)
		{
			Hands.Add(NewHand);
		}
	}

	for (int32 c = 0; c < Hands.Num(); c++)
	{
		Hands[c]->OnSequenceFinished.BindUObject(this, &ALazerBossHead::SlamFinished);
		//Hands[c]->OnStunned.BindUObject(this, &ALazerBossHead::HandStunned);
		Hands[c]->OnStunned.AddDynamic(this, &ALazerBossHead::HandStunned);
		Hands[c]->OnNormal.BindUObject(this, &ALazerBossHead::ResetHand);
	}
}

void ALazerBossHead::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bEncounterStarted || bDead)
	{
		return;
	}

	MoveHands(DeltaTime);

	if (ScaleTimeline.IsPlaying())
	{
		ScaleTimeline.TickTimeline(DeltaTime);
	}
}

void ALazerBossHead::MoveHands(float DeltaTime)
{
	if (Hands.Num() <= 0)
	{
		return;
	}

	for (int32 c = 0; c < HandPlacements.Num(); c++)
	{
		if (Hands[c]->IsSlamDownInprogress())
		{
			continue;
		}

		const FVector TargetLocation = HandPlacements[c]->GetComponentLocation();
		const FVector HandLocation = Hands[c]->GetActorLocation();

		/** Interp to the Target Location */
		const FVector VInterp = FMath::VInterpTo(HandLocation, TargetLocation, DeltaTime, InternalInterpSpeed);

		Hands[c]->SetActorLocation(VInterp);

		if (!Hands[c]->IsSlamInprogress())
		{
			const FQuat TargetRotation = HandPlacements[c]->GetComponentQuat();
			const FQuat HandRotation = Hands[c]->GetActorQuat();

			const FQuat QLerp = FMath::Lerp(HandRotation, TargetRotation, InternalRotationInterpSpeed);
		
			Hands[c]->SetActorRotation(QLerp);
		}
	}
}

void ALazerBossHead::StartSlam(int32 Index)
{
	if(Hands.IsValidIndex(Index))
	{
		Hands[Index]->StartSlam();
	}
}

void ALazerBossHead::TriggerHandIdle(int32 Index)
{
	if (Hands.IsValidIndex(Index))
	{
		Hands[Index]->TriggerIdle();
	}
}

void ALazerBossHead::SlamFinished()
{
	ALazerBossAI* AI = Cast<ALazerBossAI>(Controller);
	if (!AI)
	{
		return;
	}

	if (AI->GetBossState() == ELazerBossState::Slam)
	{
		AI->SetBossState(ELazerBossState::Movement);
	}
}

int32 ALazerBossHead::GetRandHandIndex()
{
	TArray<int32> Valid;

	for (int32 c = 0; c < Hands.Num(); c++)
	{
		if (!Hands[c]->IsStunned())
		{
			Valid.Add(c);
		}
	}

	int32 Index = 0;
	if (Valid.Num() > 0)
	{
		int32 Rand = FMath::RandRange(0, Valid.Num() - 1);
		Index = Valid[Rand];
	}

	return Index;
}

void ALazerBossHead::HandStunned(ALazerBossHand* StunnedHand)
{
	int32 Index = Hands.Find(StunnedHand);

	FRotator Rotation = HandPlacements[Index]->GetComponentRotation();
	Rotation.Pitch = StunnedHandPitch;
	HandPlacements[Index]->SetWorldRotation(Rotation);

	if (AreHandsStunned())
	{
		ALazerBossAI* AI = Cast<ALazerBossAI>(Controller);
		if (AI)
		{
			//Reset the Damage Taken allowing the damage to track how much was taken this vulnerable state. 
			ResetDamageTaken();

			//Play Growl when both hands are stunned 
			PlayOneOffAudioEvent(GrowlSFX);

			AI->SetBossState(ELazerBossState::Vulnerable);
		}
	}
	else
	{
		//Play Muffles when only one hand has been stunned 
		PlayOneOffAudioEvent(MuffledSFX);
	}
}

void ALazerBossHead::ResetHand(ALazerBossHand* InHand)
{
	int32 Index = Hands.Find(InHand);

	FRotator Rotation = HandPlacements[Index]->GetRelativeTransform().GetRotation().Rotator();
	Rotation.Pitch = NormalHandPitch;
	HandPlacements[Index]->SetRelativeRotation(Rotation);

	ALazerBossAI* AI = Cast<ALazerBossAI>(Controller);
	if (AI && AI->GetBossState() == ELazerBossState::Slam)
	{
		AI->SetBossState(ELazerBossState::Movement);
	}
}

bool ALazerBossHead::AreHandsStunned()
{
	bool bStunned = true;

	for (int32 c = 0; c < Hands.Num(); c++)
	{
		if (Hands[c]->IsNormal())
		{
			bStunned = false;
		}
	}

	return bStunned;
}

bool ALazerBossHead::IsSlamInprogress() const
{
	for (int32 c = 0; c < Hands.Num(); c++)
	{
		if (Hands[c]->IsSlamInprogress())
		{
			return true;
		}
	}

	return false;
}

bool ALazerBossHead::IsSlamDownInprogress() const
{
	for (int32 c = 0; c < Hands.Num(); c++)
	{
		if (Hands[c]->IsSlamDownInprogress())
		{
			return true;
		}
	}

	return false;
}

void ALazerBossHead::InitializeLazerState()
{
	InternalInterpSpeed = LazerInterpSpeed;
	InternalRotationInterpSpeed = LazerInterpRotationSpeed;

	for (int32 c = 0; c < Hands.Num(); c++)
	{
		//Disable collision on this hand 
		Hands[c]->HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	ScaleTimeline.PlayFromStart();
	LazerChargingComponent->ActivateSystem();

	GetRandAttackType();

	if (bEncounterStarted)
	{
		PlayLazerChargeAudio();
	}
}

void ALazerBossHead::InitializeMovementState()
{
	InternalInterpSpeed = HandInterpSpeed;
	InternalRotationInterpSpeed = HandInterpRotationSpeed;

	for (int32 c = 0; c < HandPlacements.Num(); c++)
	{
		if (!Hands[c]->IsStunned())
		{
			HandPlacements[c]->SetRelativeTransform(HandRelativeTransforms[c]);
		}
	}

	//Stop the scale timeline. 
	ScaleTimeline.Stop();
}

void ALazerBossHead::NeutralizeState()
{
	ScaleTimeline.Stop();
	LazerChargingComponent->DeactivateSystem();
	LazerChargeAudioComp->Stop();
}

void ALazerBossHead::PlayLazerChargeAudio()
{
	LazerChargeAudioComp->Play();
}

void ALazerBossHead::MoveHandsLazerSequence(float DeltaTime)
{
	LazerRotationProgress += (DeltaTime * LazerScale);
	if (LazerRotationProgress > LazerRotationDuration)
	{
		LazerRotationProgress -= LazerRotationDuration;
	}

	for (int32 c = 0; c < HandPlacements.Num(); c++)
	{
		USceneComponent* Scene = HandPlacements[c];

		FRotator RotationDir = FRotator(0.0f, 0.0f, 1.0f);
		FRotator Amount = RotationDir * (360.0f * (LazerRotationProgress / LazerRotationDuration));

		float Blend = MaxScaleTimelineValue - MinScaleTimelineValue;
		float Per = (LazerScale - MinScaleTimelineValue) / Blend;
		float Radius = LazerRadiusRange.X - (LazerRadiusRange.Y * Per);

		float Neg = c % 2 == 0 ? 1 : -1;
		FVector Offset = ((FVector::UpVector * Neg) * Radius);
		Offset = Amount.RotateVector(Offset);

		FVector DesiredLocation = Offset + LazerOrigin;

		Scene->SetRelativeLocationAndRotation(DesiredLocation, Amount);
	}
}

void ALazerBossHead::SetLazerScale(float Value)
{
	LazerScale = Value;
}

void ALazerBossHead::SpawnLazer()
{
	if (LazerWeaponClass)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		LazerWeapon = GetWorld()->SpawnActor<AWeapon_Special>(LazerWeaponClass, Params);
		LazerWeapon->SetWeaponOwner(this);
	}
}

void ALazerBossHead::FireLazer()
{
	LazerChargingComponent->DeactivateSystem();
	LazerChargeAudioComp->Stop();

	if (LazerWeapon)
	{
		//GetRandAttackType();
		LazerWeapon->Fire(LazerFireOffset);
	}
}

void ALazerBossHead::EndLazerState()
{
	ALazerBossAI* AI = Cast<ALazerBossAI>(Controller);
	if (!AI)
	{
		return;
	}
	
	for (int32 c = 0; c < Hands.Num(); c++)
	{
		Hands[c]->SetHandState(EHandState::Normal);
		Hands[c]->HitBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	//LazerChargingComponent->DeactivateSystem();
	PlayOneOffAudioEvent(LaughSFX);

	AI->SetBossState(ELazerBossState::PostLazer);

	OnLazerStateEnd.ExecuteIfBound();
}

void ALazerBossHead::GetRandAttackType()
{
	if (AttackTypeDataAssets.Num() > 0)
	{
		AttackTypeIndex = FMath::RandRange(0, AttackTypeDataAssets.Num() - 1);
		OnAttackTypeChange();
	}
}

FAttackType ALazerBossHead::GetAttackType()
{
	FAttackType AttackType = FAttackType(EAttackMode::Attack05_White);

	if (AttackTypeDataAssets.IsValidIndex(AttackTypeIndex))
	{
		AttackType = AttackTypeDataAssets[AttackTypeIndex]->Data;
	}

	return AttackType;
}

float ALazerBossHead::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Damage < 0.0f || bDead)
	{
		return 0.0f;
	}

	ALazerBossAI* AI = GetAIController();
	if (AI && AI->GetBossState() != ELazerBossState::Vulnerable)
	{
		return 0.0f;
	}

	bForceDamageOverride = true;
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (!bDead)
	{
		DamageTaken += Damage;
		float Threshold = BaseHealth / CyclePhaseCount;
		if (DamageTaken > Threshold)
		{
			ResetDamageTaken();
			AI->EndVulnerableState();
		}
	}

	return 0.0f;
}

void ALazerBossHead::ResetDamageTaken()
{
	DamageTaken = 0;
}

void ALazerBossHead::HideAndDestroy()
{
	if (!bDead)
	{
		Super::HideAndDestroy();

		for (int32 c = 0; c < Hands.Num(); c++)
		{
			Hands[c]->Dead();
		}

		OneOffAudioComp->Stop();

		HitBox->SetCollisionProfileName("NoCollision");
	}
}

void ALazerBossHead::SetBossState(ELazerBossState InBossState)
{
	uint8 StateValue = (uint8)InBossState;

	for (int32 c = 0; c < Hands.Num(); c++)
	{
		Hands[c]->SetBossState(StateValue);
	}
}

ELazerBossState ALazerBossHead::GetBossState()
{
	ALazerBossAI* AI = GetAIController();
	if (AI)
	{
		return AI->GetBossState();
	}

	return ELazerBossState::Movement;
}

void ALazerBossHead::StartEncounter()
{
	ALazerBossAI* AI = GetAIController();
	if (AI)
	{
		bEncounterStarted = true;
		AI->StartEncounter();
	}
}

ALazerBossAI* ALazerBossHead::GetAIController()
{
	return Cast<ALazerBossAI>(Controller);
}

void ALazerBossHead::PlayOneOffAudioEvent(USoundBase* BaseSound)
{
	if (BaseSound && OneOffAudioComp && !OneOffAudioComp->IsPlaying())
	{
		OneOffAudioComp->SetSound(BaseSound);
		OneOffAudioComp->Play();
	}
}

bool ALazerBossHead::IsLazerTimelinePlaying() const
{
	return ScaleTimeline.IsPlaying();
}

float ALazerBossHead::GetScaleTimelineProgress() const
{
	if (!ScaleCurve)
	{
		return -1.0f;
	}

	return ScaleTimeline.GetPlaybackPosition();
}