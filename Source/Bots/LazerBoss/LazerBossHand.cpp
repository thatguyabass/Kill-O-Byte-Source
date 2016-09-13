// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "LazerBossHand.h"
#include "SnakeCharacter/SnakeLinkHead.h"
#include "Utility/DataTables/StatData.h"

const FString ALazerBossHand::DataTableContext("LazerBossHandDataTableContext");

// Sets default values
ALazerBossHand::ALazerBossHand()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	HandRestComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandResetPo"));
	HandRestComponent->AttachTo(RootComponent);

	AttachmentComponent = CreateDefaultSubobject<USceneComponent>(TEXT("AttachmentComponent"));
	AttachmentComponent->AttachTo(RootComponent);

	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	HitBox->SetBoxExtent(FVector(250.0f));
	HitBox->AttachTo(AttachmentComponent);

	SlamAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SlamAudioComponent"));
	SlamAudioComponent->bAlwaysPlay = false;
	SlamAudioComponent->bAutoActivate = false;
	SlamAudioComponent->AttachTo(RootComponent);

	SlamState = EHandSlamState::Idle;

	MaxHealth = 0;
	Health = 0;
	bDead = false;

	WindupProgress = 0.0f;
	WindupDuration = 10.0f;

	PreSlamProgress = 0.0f;
	PreSlamDuration = 10.0f;

	UnwindProgress = 0.0f;
	UnwindDuration = 10.0f;

	MoveDownProgress = 0.0f;
	MoveDownDuration = 10.0f;

	PostSlamProgress = 0.0f;
	PostSlamDuration = 10.0f;

	ResetProgress = 0.0f;
	ResetDuration = 10.0f;

	LineTraceHeight = 1000;

	DestroyTime = 10.0f;

	static ConstructorHelpers::FObjectFinder<UDataTable> StatDataAssetObject(TEXT("/Game/Blueprints/Attributes/Stats_BotHealth"));
	StatDataTable = StatDataAssetObject.Object;

	FractureColorName = "Fracture Color";

	DissolveBlendName = "DissolveBlend";
	DissolveOutlineWidthName = "DissolveLineWidth";

	bDissolveInprogress = false;
	DissolveOutlineWidth = 0.05f;

	DissolveStartDelay = 2.0f;
	DissolveDuration = 2.0f;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ALazerBossHand::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	FTransform Trans = HandRestComponent->GetRelativeTransform();
	AttachmentComponent->SetRelativeTransform(Trans);

	FStatDataRow* StatData = StatDataTable->FindRow<FStatDataRow>(DataTableRowName, DataTableContext, false);
	if(StatData)
	{
		MaxHealth = StatData->Health;
		SetHealth(MaxHealth);
	}
	else
	{
		FString N = GetName();
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, "Bot Health Data Table Missing on: " + N);
	}

	for (int32 c = 0; c < DMI.Num(); c++)
	{
		DMI[c]->SetScalarParameterValue(DissolveBlendName, 1.0f);
		DMI[c]->SetScalarParameterValue(DissolveOutlineWidthName, DissolveOutlineWidth);
	}

	HitBox->OnComponentBeginOverlap.AddDynamic(this, &ALazerBossHand::OnBeginOverlap);
}

// Called every frame
void ALazerBossHand::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	switch (SlamState)
	{
	case EHandSlamState::Idle: break;
	case EHandSlamState::PreSlam: SlamTick(DeltaTime); break;
	case EHandSlamState::Windup: //overflow into the unwind state. Both states do the same thing with different variable values.  
	case EHandSlamState::Unwind: RotateHandSlam(DeltaTime); break;
	case EHandSlamState::MoveDown: SlamDown(DeltaTime, true); break;
	case EHandSlamState::PostSlam: SlamTick(DeltaTime); break;
	case EHandSlamState::Reset: SlamDown(DeltaTime, false); break;
	}

	if (bDissolveInprogress)
	{
		SetDissolveBlend();
	}
}

void ALazerBossHand::IncreaseSlamState()
{
	uint8 SlamIndex = (uint8)SlamState;
	const uint8 Max = (uint8)EHandSlamState::Reset;

	SlamIndex += 1;
	if (SlamIndex > Max)
	{
		SlamIndex = 0;
		OnSequenceFinished.ExecuteIfBound();
	}

	//Reset the current states progress 
	ResetSlamProgress();
	SlamState = TEnumAsByte<EHandSlamState>(SlamIndex);
	HandleSlamState();
}

void ALazerBossHand::SetSlamState(EHandSlamState InState)
{
	ResetSlamProgress();
	SlamState = InState;

	HandleSlamState();
}

void ALazerBossHand::HandleSlamState()
{
	switch (SlamState)
	{
	case EHandSlamState::Windup:
	{
		InitialHandRotation = GetActorRotation();
		FRotator Target = WindupTarget;
		Target.Yaw = GetActorRotation().Yaw;
		InitializeSlamRotation(Target);
		break;
	}
	case EHandSlamState::Unwind: 
		InitializeSlamRotation(InitialHandRotation);
		break;
	case EHandSlamState::MoveDown:
	{
		InitialActorLocation = GetActorLocation();

		FHitResult Hit(ForceInit);
		FVector Start = GetActorLocation();
		FVector Down = (FVector::UpVector * -1) * LineTraceHeight;
		FVector End = Start + Down;

		FCollisionObjectQueryParams ObjParams;
		ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);

		FName Trace = "Hand Trace";
		FCollisionQueryParams TraceParam(Trace, false, this);
		GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjParams, TraceParam);
		GetWorld()->DebugDrawTraceTag = Trace;
		if (Hit.IsValidBlockingHit())
		{
			TargetActorLocation = Hit.Location;
		}
		else
		{
			TargetActorLocation = End;
		}
		break;
	}
	case EHandSlamState::Reset:
	{
		TargetActorLocation = InitialActorLocation;
		InitialActorLocation = GetActorLocation();
		break;
	}
	}
}

void ALazerBossHand::SetSlamProgress(float DeltaTime)
{
	switch (SlamState)
	{
	case EHandSlamState::Windup: WindupProgress += DeltaTime; break;
	case EHandSlamState::Unwind: UnwindProgress += DeltaTime; break;
	case EHandSlamState::PreSlam: PreSlamProgress += DeltaTime; break;
	case EHandSlamState::MoveDown: MoveDownProgress += DeltaTime; break;
	case EHandSlamState::PostSlam: PostSlamProgress += DeltaTime; break;
	case EHandSlamState::Reset: ResetProgress += DeltaTime; break;
	}
}

float ALazerBossHand::GetSlamProgress()
{
	float Progress = 0.0f;

	switch (SlamState)
	{
	case EHandSlamState::Windup : Progress = WindupProgress; break;
	case EHandSlamState::Unwind : Progress = UnwindProgress; break;
	case EHandSlamState::PreSlam: Progress = PreSlamProgress; break;
	case EHandSlamState::MoveDown : Progress = MoveDownProgress; break;
	case EHandSlamState::PostSlam: Progress = PostSlamProgress; break;
	case EHandSlamState::Reset: Progress = ResetProgress; break;
	}

	return Progress;
}

float ALazerBossHand::GetSlamDuration()
{
	float Duration = 0.0f;

	switch (SlamState)
	{
	case EHandSlamState::Windup: Duration = WindupDuration; break;
	case EHandSlamState::Unwind: Duration = UnwindDuration; break;
	case EHandSlamState::PreSlam: Duration = PreSlamDuration; break;
	case EHandSlamState::MoveDown: Duration = MoveDownDuration; break;
	case EHandSlamState::PostSlam: Duration = PostSlamDuration; break;
	case EHandSlamState::Reset: Duration = IsStunned() ? ResetDuration / 2 : ResetDuration; break;
	}

	return Duration;
}

void ALazerBossHand::ResetSlamProgress()
{
	switch (SlamState)
	{
	case EHandSlamState::Windup: WindupProgress = 0.0f; break;
	case EHandSlamState::Unwind: UnwindProgress = 0.0f; break;
	case EHandSlamState::PreSlam: PreSlamProgress = 0.0f; break;
	case EHandSlamState::MoveDown: MoveDownProgress = 0.0f; break;
	case EHandSlamState::PostSlam: PostSlamProgress = 0.0f; break;
	case EHandSlamState::Reset: ResetProgress = 0.0f; break;
	}
}

void ALazerBossHand::RotateHandSlam(float DeltaTime)
{
	SetSlamProgress(DeltaTime);

	float Progress = GetSlamProgress();
	float Duration = GetSlamDuration();
	if (Progress > Duration)
	{
		//Progress is reset in Increase Slam State
		IncreaseSlamState();
	}
	else
	{
		const float Blend = Progress / Duration;
		FQuat Rotation = FMath::Lerp(InitialActorRotation.Quaternion(), TargetActorRotation.Quaternion(), Blend);
		SetActorRotation(Rotation);
	}
}

void ALazerBossHand::InitializeSlamRotation(FRotator TargetRotation)
{
	InitialActorRotation = GetActorRotation();
	TargetActorRotation = TargetRotation;
}

void ALazerBossHand::SlamDown(float DeltaTime, bool TriggerEvent)
{
	SetSlamProgress(DeltaTime);

	float Progress = GetSlamProgress();
	float Duration = GetSlamDuration();
	if (Progress > Duration)
	{
		if (TriggerEvent)
		{
			//Blueprint Event
			OnSlamDown();
			SlamAudioComponent->Play();
		}

		IncreaseSlamState();
	}
	else
	{
		float Blend = Progress / Duration;
		FVector Location = FMath::Lerp(InitialActorLocation, TargetActorLocation, Blend);
		SetActorLocation(Location);
	}
}

void ALazerBossHand::SlamTick(float DeltaTime)
{
	SetSlamProgress(DeltaTime);

	float Progress = GetSlamProgress();
	float Duration = GetSlamDuration();
	if (Progress > Duration)
	{
		IncreaseSlamState();
	}
}

void ALazerBossHand::StartSlam()
{
	if (SlamState == EHandSlamState::Idle)
	{
		IncreaseSlamState();
	}
}

bool ALazerBossHand::IsSlamInprogress() const
{
	return (SlamState != EHandSlamState::Idle);
}

bool ALazerBossHand::IsSlamDownInprogress() const
{
	if ((uint8)SlamState >= (uint8)EHandSlamState::Unwind)
	{
		return true;
	}

	return false;
}

EHandSlamState ALazerBossHand::GetSlamState() const
{
	return SlamState;
}

void ALazerBossHand::SetHandState(EHandState InState)
{
	if (HandState != InState)
	{
		HandState = InState;

		if (IsStunned())
		{
			//OnStunned.ExecuteIfBound(this);
			OnStunned.Broadcast(this);
		}
		else if (IsNormal())
		{
			//Restore the hands health. 
			SetHealth(MaxHealth);
			OnNormal.ExecuteIfBound(this);
		}
	}
}

bool ALazerBossHand::IsNormal() const
{
	return (HandState == EHandState::Normal);
}
bool ALazerBossHand::IsStunned() const
{
	return (HandState == EHandState::Stunned);
}

float ALazerBossHand::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Damage <= 0.0f || IsStunned() || bDead)
	{
		return 0.0f;
	}

	//Damage can only be applied if in the slam or post slam states 
	if (SlamState == EHandSlamState::MoveDown || SlamState == EHandSlamState::PostSlam)
	{
		ReduceHealth(Damage);

		if (Health <= 0.0)
		{
			SetSlamState(EHandSlamState::Reset);
			SetHandState(EHandState::Stunned);
		}
	}

	return 0.0f;
}

void ALazerBossHand::Dead()
{
	HideAndDestroy();
	bDead = true;
}

void ALazerBossHand::OnBeginOverlap(AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	//ASnakeLinkHead* Head = Cast<ASnakeLinkHead>(Other);
	APawn* OtherPawn = Cast<APawn>(Other);
	if (!OtherPawn/*Head*/ || SlamState != EHandSlamState::MoveDown)
	{
		return;
	}

	FDamageEvent Event;
	OtherPawn->TakeDamage(100000, Event, nullptr, this);
}

void ALazerBossHand::SetHealth(int32 InHealth)
{
	Health = InHealth;
	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}
}

void ALazerBossHand::AddHealth(int32 InHealth)
{
	Health += InHealth;
	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}
}

void ALazerBossHand::ReduceHealth(int32 Value)
{
	if (Value <= 0.0f)
	{
		return;
	}

	Health -= Value;
}

int32 ALazerBossHand::GetHealth() const
{
	return Health;
}

int32 ALazerBossHand::GetMaxHealth() const
{
	return MaxHealth;
}

void ALazerBossHand::SetBossState(uint8 InBossState)
{
	if (BossState != InBossState)
	{
		OnStateChange(BossState, InBossState);
	}

	BossState = InBossState;
}

uint8 ALazerBossHand::GetBossState()
{
	return BossState;
}

void ALazerBossHand::HideAndDestroy_Implementation()
{
	if (!bDead)
	{
		if (HitBox)
		{
			HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		GetWorldTimerManager().SetTimer(DissolveDelay_TimerHandle, this, &ALazerBossHand::StartDissolve, DissolveStartDelay, false);

		SetLifeSpan(DestroyTime);
	}
}

void ALazerBossHand::StartDissolve()
{
	if (!bDissolveInprogress)
	{
		bDissolveInprogress = true;
	}
}

void ALazerBossHand::SetDissolveBlend()
{
	if (!bDissolveInprogress)
	{
		return;
	}

	float DeltaTime = GetWorld()->GetDeltaSeconds();

	DissolveProgress += DeltaTime;

	float Blend = DissolveProgress / DissolveDuration;
	float DissolveBlend = FMath::Lerp(1.0f, 0.0f, Blend);

	for (int32 c = 0; c < DMI.Num(); c++)
	{
		DMI[c]->SetScalarParameterValue(DissolveBlendName, DissolveBlend);
		DMI[c]->SetScalarParameterValue(DissolveOutlineWidthName, DissolveOutlineWidth);
	}

	if (DissolveProgress > DissolveDuration)
	{
		bDissolveInprogress = false;
	}
}