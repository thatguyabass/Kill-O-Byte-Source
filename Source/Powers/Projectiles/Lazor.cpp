// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "Lazor.h"
#include "Powers/AttackHelper.h"
#include "Powers/Projectiles/Components/BeamTarget.h"
#include "Utility/Utility.h"
#include "Bots/Controller/BotUtility.h"

// Sets default values
ALazor::ALazor()
{
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootComp;

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Lazor Particle System"));
	ParticleSystemComponent->AttachTo(RootComponent);

	FParticleSysParam Alpha = FParticleSysParam();
	Alpha.Name = "AlphaOverTime";
	Alpha.ParamType = EParticleSysParamType::PSPT_Scalar;
	Alpha.Scalar = 1.0f;

	ParticleSystemComponent->InstanceParameters.Add(Alpha);

	FParticleSysParam InitialSize = FParticleSysParam();
	InitialSize.Name = "InitialSize";
	InitialSize.ParamType = EParticleSysParamType::PSPT_Vector;
	InitialSize.Vector = FVector(100.0f, 0.0f, 0.0f);

	ParticleSystemComponent->InstanceParameters.Add(InitialSize);

	PrimaryColorName = "Primary Color";
	SecondaryColorName = "Secondary Color";

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAlwaysPlay = false;
	AudioComponent->AttachTo(RootComponent);

	Duration = 2.0f;
	Damage = 1;
	DamageProgress = 0;
	DamageFrequency = 0.125f;
	DamageBoxExtent = FVector(75.0f);

	LazerTraceMethod = ELazerTraceMethod::LeftRight;
	LeftRightTraceOffset = 50.0f;
	UpDownTraceOffset = 0.0f;
	bShowLineTrace = false;

	bFinished = false;
	bCanFadeOut = false;

	bFriendlyFire = false;
	bForceDamageOverride = false;

	TrackingCollisionChannels.Add(ECC_WorldStatic);

	CollisionChannels.Add(ECC_WorldStatic);
	CollisionChannels.Add(ECC_Pawn);
	CollisionChannels.Add(ECC_ProjectileChannel);
	CollisionChannels.Add(ECC_PlayerProjectile);

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ALazor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (BeamTargetClass && !BeamTarget)
	{
		FVector Location = GetActorLocation();
		FRotator Rotation = GetActorRotation();
		BeamTarget = GetWorld()->SpawnActor<ABeamTarget>(BeamTargetClass, Location, Rotation);
		BeamTarget->AttachRootComponentToActor(this, NAME_None, EAttachLocation::KeepWorldPosition, false);
		BeamTarget->SetBeamOwner(this);
	}
}

void ALazor::BeginPlay()
{
	Super::BeginPlay();

	if (BeamTarget)
	{
		if (Duration > 0)
		{
			float TimeTillFadeOut = Duration - FadeOutTimelineLength;
			GetWorldTimerManager().SetTimer(Fade_TimerHandle, this, &ALazor::StartFadeOutTimeline, TimeTillFadeOut, false);
			GetWorldTimerManager().SetTimer(Destroy_TimerHandle, this, &ALazor::HideAndDestroy, Duration, false);
		}
	}

	if (FadeOutCurve)
	{
		FadeOutTimeline = FTimeline();
		FOnTimelineFloatStatic Func;

		Func.BindUObject(this, &ALazor::FadeOut);
		FadeOutTimeline.AddInterpFloat(FadeOutCurve, Func);
		FadeOutTimeline.SetTimelineLength(FadeOutTimelineLength);
	}

	if (DamageCurve)
	{
		DamageTimeline = FTimeline();
		FOnTimelineFloatStatic Func;

		Func.BindUObject(this, &ALazor::CalculateDamage);
		DamageTimeline.AddInterpFloat(DamageCurve, Func);
		DamageTimeline.SetTimelineLength(Duration);

		DamageTimeline.PlayFromStart();
	}

	if (AudioComponent && AudioComponent->Sound)
	{
		AudioComponent->Play();
	}
}

// Called every frame
void ALazor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	TrackLazer(DeltaTime);

	FAttackType Type = AttackHelper::FindAttackTypeInActor(ProjectileOwner);
	if (Type.AttackMode != AttackType.AttackMode)
	{
		SetAttackType(Type);
	}

	//If playing Tick, else start timeline from start
	if (DamageTimeline.IsPlaying())
	{
		DamageTimeline.TickTimeline(DeltaTime);
	}

	if (FadeOutTimeline.IsPlaying())
	{
		FadeOutTimeline.TickTimeline(DeltaTime);
	}

	DamageProgress += DeltaTime;
	if (DamageProgress > DamageFrequency)
	{
		if (BeamTarget)
		{
			BeamTarget->ExecuteTick();
		}

		DealDamage();
	}
}

void ALazor::TrackLazer(float DeltaTime)
{
	if (BeamTarget)
	{
		FVector TargetLocation = BeamTarget->GetActorLocation();
		FVector MyLocation = GetActorLocation();

		ParticleSystemComponent->SetBeamEndPoint(0, TargetLocation);

		if (bFinished)
		{
			return;
		}

		FVector Direction = TargetLocation - MyLocation;
		float Distance = Direction.Size();
		Direction.Normalize();

		TArray<FHitResult> Hits;

		int32 LineTraceCount = 3;
		if (LazerTraceMethod == ELazerTraceMethod::Both || LazerTraceMethod == ELazerTraceMethod::UpDown)
		{
			LineTraceCount = 5;
		}

		for (int32 c = 0; c < LineTraceCount; c++)
		{
			FVector Offset = GetOffset(c);
			FVector Start = MyLocation + Offset;
			FVector End = Start + (Direction * (Distance + 50.0f));

			FHitResult Hit(ForceInit);

			FName TagName = "LazerTrace";
			FCollisionQueryParams Params(TagName, false, this);

			if (LazerOwner)
			{
				Params.AddIgnoredActor(LazerOwner);
			}

			GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, CreateObjectQueryParams(TrackingCollisionChannels), Params);

			if (bShowLineTrace)
			{
				GetWorld()->DebugDrawTraceTag = TagName;
			}

			if (Hit.bBlockingHit)
			{
				Hit.Location -= Offset;
				Hits.Add(Hit);
			}

			//Hacky: if the tracemethod is set to UpDown and its the first index, Increase the index by 2 to skip the left right offsets 
			if (LazerTraceMethod == ELazerTraceMethod::UpDown && c == 0)
			{
				//Increaseing by two will allow the c++ to occur.
				c += 2;
			}
		}

		if (Hits.Num() > 0)
		{
			int32 Index = -1;
			float Closest = FLT_MAX;

			for (int32 c = 0; c < Hits.Num(); c++)
			{
				float Dis = (Hits[c].Location - MyLocation).Size();
				if (Dis < Closest)
				{
					Index = c;
					Closest = Dis;
				}
			}

			//Stop the Beam Target and Set the location to the closest Hit
			BeamTarget->bMove = false;
			BeamTarget->SetActorLocation(Hits[Index].Location);
		}
		else
		{
			// No hit was detected, move Grow the Beam!
			BeamTarget->bMove = true;
		}
	}
}

FVector ALazor::GetOffset(int32 Index)
{
	FVector Offset = FVector::ZeroVector;
	switch (Index)
	{
	case 0: break;
	case 1: Offset = FVector::RightVector * LeftRightTraceOffset; break;
	case 2: Offset = (FVector::RightVector * -1) * LeftRightTraceOffset; break;
	case 3: Offset = FVector::UpVector * UpDownTraceOffset; break;
	case 4: Offset = (FVector::UpVector * -1) * UpDownTraceOffset; break;
	}

	return Offset;
}

void ALazor::HideAndDestroy()
{
	if (!bFinished)
	{
		bFinished = true;

		SetLifeSpan(2.0f);

		if (BeamTarget)
		{
			BeamTarget->StartDestroy();
		}
	}
}

void ALazor::SetLazerOwner(AActor* InOwner, FVector FireOffset, AActor* InProjectileOwner)
{
	LazerOwner = InOwner;
	ProjectileOwner = InProjectileOwner ? InProjectileOwner : InOwner;
	OwnerOffset = FireOffset;

	AttachRootComponentToActor(InOwner, NAME_None, EAttachLocation::KeepWorldPosition, false);

	//Set the color atleast once
	FAttackType Type = AttackHelper::FindAttackTypeInActor(ProjectileOwner);
	SetAttackType(Type);
}

void ALazor::SetAttackType(const FAttackType InAttackType)
{
	AttackType = InAttackType;

	if (LazerMaterial)
	{
		FTeamColorData Data = AttackType.ColorData;

		//Create a Dynamic Material Instance
		UMaterialInstanceDynamic* DMI = UMaterialInstanceDynamic::Create(LazerMaterial, this);
		//Set the Primary and Secondary Colors
		DMI->SetVectorParameterValue(PrimaryColorName, Data.PrimaryColor);
		DMI->SetVectorParameterValue(SecondaryColorName, Data.SecondaryColor);
		//Add this material to the Particle
		ParticleSystemComponent->SetMaterial(0, DMI);
	}
}

bool ALazor::IsFinished() const
{
	return bFinished;
}

void ALazor::TerminateLazer()
{
	GetWorldTimerManager().ClearTimer(Fade_TimerHandle);
	StartFadeOutTimeline();

	//Destroy this lazer
	GetWorldTimerManager().ClearTimer(Destroy_TimerHandle);
	GetWorldTimerManager().SetTimer(Destroy_TimerHandle, this, &ALazor::HideAndDestroy, FadeOutTimelineLength, false);
}

void ALazor::StartFadeOutTimeline()
{
	if (FadeOutCurve)
	{
		FadeOutTimeline.PlayFromStart();
	}

	if (AudioComponent)
	{
		AudioComponent->FadeOut(FadeOutTimelineLength, 0.0f);
	}
}

void ALazor::FadeOut(float Alpha)
{
	if (ParticleSystemComponent->InstanceParameters.Num() > 0)
	{
		ParticleSystemComponent->InstanceParameters[0].Scalar = Alpha;
	}
}

void ALazor::SetDamage(int32 InDamage)
{
	Damage = InDamage;
}

void ALazor::CalculateDamage(float Value)
{
	ScaledDamage = Damage * Value;
}

void ALazor::DealDamage()
{
	DamageProgress -= DamageFrequency;

	if (GetWorld() && !bFinished && BeamTarget)
	{
		FVector Start = GetActorLocation();
		FVector End = BeamTarget->GetActorLocation();

		FVector Direction = End - Start;
		FQuat Rotation = Direction.Rotation().Quaternion();

		Direction.Normalize();
		End += (Direction * 50.0f);

		FName TraceName = "DealDamageMultiSweep";
		FCollisionQueryParams Params(TraceName, false, this);

		TArray<FHitResult> Hits;

		GetWorld()->SweepMultiByObjectType(Hits, Start, End, Rotation, CreateObjectQueryParams(CollisionChannels), FCollisionShape::MakeBox(DamageBoxExtent), Params);

		for (int32 c = 0; c < Hits.Num(); c++)
		{
			if (Hits[c].bBlockingHit && (Hits[c].GetActor() && Hits[c].GetActor() != ProjectileOwner))
			{
				AActor* Actor = Hits[c].GetActor();

				bool OppositeTeam = UBotUtility::TargetCheck(ProjectileOwner, Actor);
				if (!OppositeTeam && !bFriendlyFire)
				{
					continue;
				}

				if (bForceDamageOverride)
				{
					ASnakeLinkHead* Head = Cast<ASnakeLinkHead>(Actor);
					if (Head)
					{
						Head->OverrideInvulnerable();
					}
				}

				FDamageEvent Event = AttackHelper::CreateDamageEvent(DamageTypeClass, AttackType);
				Actor->TakeDamage(ScaledDamage, Event, nullptr, ProjectileOwner);
			}
		}
	}
}

FCollisionObjectQueryParams ALazor::CreateObjectQueryParams(TArray<TEnumAsByte<ECollisionChannel>>& ECC_Array)
{
	FCollisionObjectQueryParams ObjParams;
	for (int32 c = 0; c < ECC_Array.Num(); c++)
	{
		ObjParams.AddObjectTypesToQuery(ECC_Array[c]);
	}

	return ObjParams;
}

void ALazor::SetDamageCollisionChannel(TArray<ECollisionChannel> ECC_Array)
{
	CollisionChannels.Empty();
	CollisionChannels.Append(ECC_Array);
}

float ALazor::GetRemainingDuration()
{
	if(GetWorld() && GetWorldTimerManager().IsTimerActive(Destroy_TimerHandle))
	{
		return GetWorldTimerManager().GetTimerRemaining(Destroy_TimerHandle);
	}

	return Duration;
}

ABeamTarget* ALazor::GetBeamTarget()
{
	return BeamTarget;
}