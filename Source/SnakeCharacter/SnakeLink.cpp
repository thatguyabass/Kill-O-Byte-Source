// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SnakeLink.h"
#include "GameMode/BaseGameMode.h"
#include "Powers/Shield/Repulsion.h"
#include "Powers/Weapons/Weapon.h"
#include "Powers/Projectiles/BaseProjectile.h"

#include "Powers/AttackHelper.h"

#include "SnakePlayer.h"
#include "SnakeLinkHead.h"
#include "AIController.h"

const int32 ASnakeLink::WeaponFireAudioIndex = 0;
const int32 ASnakeLink::ShapeAudioIndex = 1;
const int32 ASnakeLink::NullAudioIndex = 2;
const int32 ASnakeLink::OneOffAudioIndex = 3;

ASnakeLink::ASnakeLink(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	//Set if the character rotates when rotate input is given
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	//Configure the Character Movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0, 540.0f, 0);
	GetCharacterMovement()->SetJumpAllowed(false);
	GetCharacterMovement()->MaxWalkSpeed = 700.0f;
	GetCharacterMovement()->AirControl = 0.8f;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("CharacterCapsule_SB"));

	BoxComponent = PCIP.CreateDefaultSubobject<UBoxComponent>(this, TEXT("BoxComponent"));
	BoxComponent->SetCollisionProfileName(TEXT("SnakeBody"));
	BoxComponent->SetBoxExtent(FVector(50.0f));
	BoxComponent->bCanEverAffectNavigation = false;
	BoxComponent->AttachTo(RootComponent);

	DestructibleComponent = PCIP.CreateDefaultSubobject<UDestructibleComponent>(this, TEXT("DestructibleComponent"));
	DestructibleComponent->bCanEverAffectNavigation = false;
	DestructibleComponent->AttachTo(RootComponent);

	SpawnEffectVFX = PCIP.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("SpawnEffectVFXComponent"));
	SpawnEffectVFX->bAutoActivate = false;
	SpawnEffectVFX->AttachTo(RootComponent);

	ActorHalfHeightName = "Actor Half Height";
	SpawnBlendName = "Blend Value";
	SpawnOriginOffsetName = "Offset";
	SpawnEffectOutlineWidthName = "Spawn Line Width";

	bSpawnEffectDirection = false;
	bPlaySpawnVFX = false;
	bSpawnEffectInprogress = false;
	SpawnEffectDuration = 1.0f;
	SpawnEffectProgress = 0.0f;
	ActorHalfHeight = 60.0f;
	SpawnOffset = 0.0f;
	SpawnEffectOutlineWidth = 0.1f;

	DissolveColorName = "Fracture Color";
	DissolveBlendName = "DissolveBlend";
	DissolveOutlineWidthName = "DissolveLineWidth";

	bDissolveInprogress = false;
	DissolveOutlineWidth = 0.05f;

	DissolveStartDelay = 2.0f;
	DissolveDuration = 2.0f;

	DestroyTime = 5.0f;

	CurrentState = ESnakeState::Body;

	IdealDistance = 150.0f;
	MaxDistance = IdealDistance * 2.5f;

	PrimaryParamName = "Player_Base";
	SecondaryParamName = "Line_Glow";
	AlphaParamName = "PlayerAlpha";

	//Speed Stats - Initialize them to 0
	BaseMovementSpeed = -5.0f;
	GlobalSpeedModifier = 1.0f;

	bSuppressInput = false;

	FireOffset = FVector(50.0f, 0.0f, 0.0f);

	LineTraceToHeadObjectChannels.Add(ECC_WorldStatic);

	bReplicates = true;
	bReplicateMovement = true;
}

void ASnakeLink::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);
}

void ASnakeLink::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	BaseMovementSpeed = GetCharacterMovement()->MaxWalkSpeed;

	if (DestructibleComponent)
	{
		for (int32 c = 0; c < DestructibleComponent->GetNumMaterials(); c++)
		{
			DMI.Add(DestructibleComponent->CreateAndSetMaterialInstanceDynamic(c));
		}
	}
}

void ASnakeLink::BeginPlay()
{
	Super::BeginPlay();

	if (AlphaCurve)
	{
		AlphaTimeline = FTimeline();
		FOnTimelineFloatStatic Func;
		Func.BindUObject(this, &ASnakeLink::SetMaterialAlpha);

		AlphaTimeline.AddInterpFloat(AlphaCurve, Func);
		AlphaTimeline.SetLooping(true);
	}

	//bool ManualPlay = GetSnakePlayer()->bPlaySpawnEffect;

	//if (bPlaySpawnVFX && !ManualPlay)
	//{
	//	InitializeSpawnEffect(false);
	//}
	//else
	//{
	//	StartSpawnEffect(false);
	//}
}

void ASnakeLink::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsBody() && LinkBrain)
	{
		if (LinkBrain->IsRidgidShape())
		{
			FollowComponent();
		}
		else
		{
			FollowTarget();
		}

		LineTraceToHead();
	}

	if (AlphaTimeline.IsPlaying())
	{
		AlphaTimeline.TickTimeline(DeltaTime);
	}

	if (bDissolveInprogress)
	{
		SetDissolveBlend();
	}

	if (bSpawnEffectInprogress)
	{
		SpawnEffectTick();
	}
}

void ASnakeLink::FollowTarget()
{
	if (!IsBody())
	{
		return;
	}

	if (!Target)
	{
		return;
	}

	FVector TL = Target->GetActorLocation();
	FVector Direction = TL - GetActorLocation();

	float Distance = Direction.Size();

	float MoveScale = Distance / IdealDistance;

	Direction.Normalize();

	float MoveSpeed = MovementSpeed;
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	FVector Movement = Direction * ((MoveSpeed * MoveScale) * DeltaTime);
	FRotator Rotation = Direction.Rotation();
	FHitResult Hit(ForceInit);

	GetCharacterMovement()->SafeMoveUpdatedComponent(Movement, Rotation, true, Hit);
	if (Hit.IsValidBlockingHit())
	{
		const FVector Normal = Hit.Normal.GetSafeNormal();
		const FVector Deflection = FVector::VectorPlaneProject(Movement, Normal) * (1.0f - Hit.Time);
		GetCharacterMovement()->SafeMoveUpdatedComponent(Deflection, Rotation, true, Hit);
	}
}

void ASnakeLink::FollowComponent()
{
	if (!IsBody())
	{
		return;
	}

	if (!LinkBrain)
	{
		return;
	}

	const USceneComponent* TargetComponent = LinkBrain->GetBodyTarget(BodyIndex);

	//Get Locations
	const FVector TargetLocation = TargetComponent->GetComponentLocation();
	const FVector MyLocation = GetActorLocation();

	//Interp to the Target Location
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	const FVector VInterp = FMath::VInterpTo(MyLocation, TargetLocation, DeltaTime, 15.0f);

	//Lerp to the Interp Final Location
	const float Blend = FMath::Abs((TargetLocation - MyLocation).Size());
	const FVector VLerp = FMath::Lerp(MyLocation, VInterp, 1.0f);

	//Get the Delta of the location change 
	const FVector VDelta = VLerp - MyLocation;

	//Get Rotations
	const FRotator TargetRotation = TargetComponent->GetComponentRotation();
	const FRotator MyRotation = GetActorRotation();

	//Interp and Lerp the Rotations 
	const FRotator RInterp = FMath::RInterpTo(MyRotation, TargetRotation, DeltaTime, 15.0f);
	const FRotator RLerp = FMath::Lerp(MyRotation, RInterp, 1.0f);

	FHitResult Hit(ForceInit);
	GetCharacterMovement()->SafeMoveUpdatedComponent(VDelta, RLerp, true, Hit);
	if (Hit.IsValidBlockingHit())
	{
		const FVector Normal = Hit.Normal.GetSafeNormal();
		const FVector Deflection = FVector::VectorPlaneProject(VDelta, Normal) * (1.0f - Hit.Time);
		GetCharacterMovement()->SafeMoveUpdatedComponent(Deflection, RLerp, true, Hit);
	}
}

void ASnakeLink::LineTraceToHead()
{
	if (!IsBody() || !LinkBrain)
	{
		return;
	}

	const FVector Start = GetActorLocation();
	const FVector End = LinkBrain->GetActorLocation();
	
	FVector Direction = End - Start;
	float Distance = Direction.Size();
	Direction.Normalize();

	FHitResult Hit(ForceInit);
	FCollisionObjectQueryParams ObjParam;
	for (int32 c = 0; c < LineTraceToHeadObjectChannels.Num(); c++)
	{
		ObjParam.AddObjectTypesToQuery(LineTraceToHeadObjectChannels[c]);
	}

	const FName LineTraceTag = "HeadTraceTag";
	FCollisionQueryParams Params(LineTraceTag, false, this);

	bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjParam, Params);
	if (Hit.IsValidBlockingHit())
	{
		if (!GetWorldTimerManager().IsTimerActive(Teleport_TimerHandle))
		{
			//Set Timer
			GetWorldTimerManager().SetTimer(Teleport_TimerHandle, this, &ASnakeLink::TeleportToHead, 1.0f, false);
		}
	}
	else if(bHit != bLastTraceCheck)
	{
		ClearTimer(Teleport_TimerHandle);
	}

	bLastTraceCheck = bHit;
}

void ASnakeLink::TeleportToHead()
{
	if (LinkBrain)
	{
		const FVector Location = LinkBrain->GetActorLocation();
		const FRotator Rotation = LinkBrain->GetActorRotation();
		TeleportTo(Location, Rotation, false, true);
	}
}

void ASnakeLink::UpdateColors(const FTeamColorData& ColorData)
{
	//if (DMI)
	//{
	for(int32 c = 0; c < DMI.Num(); c++)
	{
		DMI[c]->SetVectorParameterValue(PrimaryParamName, ColorData.PrimaryColor);
		DMI[c]->SetVectorParameterValue(SecondaryParamName, ColorData.SecondaryColor);
		DMI[c]->SetVectorParameterValue(DissolveColorName, ColorData.PrimaryColor);
	}
}

FAttackType ASnakeLink::GetAttackType()
{
	if (IsHead())
	{
		// This will call the Overridden Method in ASnakeLinkHead
		return GetAttackType();
	}
	else if(LinkBrain)
	{
		return LinkBrain->GetAttackType();
	}

	return FAttackType();
}

void ASnakeLink::StartMaterialFlicker()
{
	AlphaTimeline.PlayFromStart();
}

void ASnakeLink::EndMaterialFlicker()
{
	AlphaTimeline.Stop();

	//Reset the alpha 
	SetMaterialAlpha(1.0f);
}

void ASnakeLink::SetMaterialAlpha(float InValue)
{
	//if (DMI)
	for(int32 c = 0; c < DMI.Num(); c++)
	{
		DMI[c]->SetScalarParameterValue(AlphaParamName, InValue);
	}
}

/** The Snake Component that for the head will always be the Character Cylinder. This is due to it beeing the root component and the root primitive component.
*	This will have to be how it is. The only issue that should arise would be the character appearing to clip into the body before the character truely hits the collider. 
*/
void ASnakeLink::NotifyHit(class UPrimitiveComponent* MyComponent, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation,
	FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComponent, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
}

void ASnakeLink::SetTarget(ASnakeLink* Link)
{
	Target = Link;
}
ASnakeLink* ASnakeLink::GetTarget()
{
	return Target;
}

void ASnakeLink::SetCurrentState(ESnakeState State)
{
	CurrentState = State;

	HandleNewState();
}
ESnakeState ASnakeLink::GetCurrentState()
{
	return CurrentState;
}

void ASnakeLink::HandleNewState()
{
	//Call the Blueprint Event
	StateChange(CurrentState);
}

bool ASnakeLink::IsHead()
{
	return (CurrentState == ESnakeState::Head);
}
bool ASnakeLink::IsHeadConst() const
{
	return (CurrentState == ESnakeState::Head);
}
bool ASnakeLink::IsBody()
{
	return (CurrentState == ESnakeState::Body);
}

bool ASnakeLink::IsShapeName(EShapeName Name)
{
	if (IsHead())
	{
		return IsShapeName(Name);
	}
	else if (LinkBrain)
	{
		return LinkBrain->IsShapeName(Name);
	}

	return false;
}

void ASnakeLink::DespawnAndDestroy()
{
	StartSpawnEffect(true);

	CleanupCharacter();
}

void ASnakeLink::HideAndDestroy()
{
	if (GetWorld())
	{
		GetWorldTimerManager().SetTimer(DissolveDelay_TimerHandle, this, &ASnakeLink::StartDissolve, DissolveStartDelay, false);
	}

	if (DestructibleComponent)
	{
		DestructibleComponent->ApplyRadiusDamage(500, GetActorLocation(), 500, 10, true);
	}
	CleanupCharacter();
}

void ASnakeLink::CleanupCharacter()
{
	this->SetLifeSpan(DestroyTime);

	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CollectionSphere)
	{
		CollectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ASnakeLink::CleanUpBody()
{
	if (IsHead())
	{
		CleanUpBody();
	}
}

float ASnakeLink::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, class AActor* DamageCauser)
{
	int32 DamageTaken = Damage;

	ASnakePlayerState* State = Cast<ASnakePlayerState>(DamageCauser);
	ReduceHealth(DamageTaken, State);

	return 0.0f;
}

void ASnakeLink::ReduceHealth(int32 Value, ASnakePlayerState* Damager)
{
	if (IsHead())
	{
		ReduceHealth(Value, Damager);
	}
	else if (LinkBrain)
	{
		LinkBrain->ReduceHealth(Value, Damager);
	}
}

void ASnakeLink::AddHealth(int32 Value)
{
	if (IsHead())
	{
		AddHealth(Value);
	}
	else if(LinkBrain)
	{
		LinkBrain->AddHealth(Value);
	}
}

void ASnakeLink::SetHealth(int32 Value)
{
	if (IsHead())
	{
		SetHealth(Value);
	}
	else if (LinkBrain)
	{
		LinkBrain->SetHealth(Value);
	}
}

int32 ASnakeLink::GetHealth() const
{
	if (IsHeadConst())
	{
		return GetHealth();
	}
	else if (LinkBrain)
	{
		return LinkBrain->GetHealth();
	}
	return 0;
}

int32 ASnakeLink::GetTotalHealth() const
{
	if (IsHeadConst())
	{
		return GetTotalHealth();
	}
	else if (LinkBrain)
	{
		return LinkBrain->GetTotalHealth();
	}

	return 0;
}

bool ASnakeLink::IsDead() const
{
	if (IsHeadConst())
	{
		return IsDead();
	}
	else if(LinkBrain)
	{
		return LinkBrain->IsDead();
	}

	return true;
}

////////////////////////////////////////////////////////////////
////// Weapon Methods

void ASnakeLink::IgnoreProjectiles(ABaseProjectile* InProjectile)
{
	if (LinkBrain)
	{
		LinkBrain->IgnoreProjectiles(InProjectile);
	}
}

void ASnakeLink::SetPrimary(AWeapon* NewWeapon, AActor* ProjectileOwner)
{
	if (NewWeapon)
	{
		PrimaryWeapon = NewWeapon;
		PrimaryWeapon->SetWeaponOwner(this, ProjectileOwner);
	}
}

AWeapon* ASnakeLink::GetPrimaryWeapon() const
{
	return PrimaryWeapon;
}

void ASnakeLink::ChangeWeapon(TSubclassOf<AWeapon> InWeaponClass)
{
	if (LinkBrain)
	{
		LinkBrain->ChangePrimary(InWeaponClass);
	}
	else if (IsHead())
	{
		this->ChangePrimary(InWeaponClass);
	}
}

void ASnakeLink::ChangePrimary(TSubclassOf<AWeapon> InWeaponClass, AActor* ProjectileOwner)
{
	if (!InWeaponClass)
	{
		return;
	}

	//if (IsHead() && GetOverrideCanFire())
	//{

	//}

	AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(InWeaponClass);

	SetPrimary(NewWeapon, ProjectileOwner);
}

void ASnakeLink::ResetPrimaryWeapon(TSubclassOf<class AWeapon> InPrimaryWeaponClass)
{
	//Destroy the Current Primary Weapon
	PrimaryWeapon->StartDestroy();

	ChangeWeapon(InPrimaryWeaponClass);
}

bool ASnakeLink::LazerBeingFired()
{
	if (PrimaryWeapon && PrimaryWeapon->ProjectileBeingFired())
	{
		return true;
	}

	return false;
}

void ASnakeLink::TerminateProjectile()
{
	if (PrimaryWeapon)
	{
		PrimaryWeapon->TerminateProjectile();
	}
}

void ASnakeLink::CleanUpWeapons()
{
	// All three may equal the same object, but this way every weapon should be destroyed 
	if (PrimaryWeapon)
	{
		PrimaryWeapon->StartDestroy();
	}
}

ASnakeLink* ASnakeLink::GetHead()
{
	if (IsHead())
	{
		return this;
	}
	else
	{
		return LinkBrain;
	}
}

ASnakeLinkHead* ASnakeLink::GetHeadCast()
{
	return Cast<ASnakeLinkHead>(GetHead());
}

void ASnakeLink::ApplySlow(FSpeedPower Slow)
{
	if (IsHead())
	{
		ApplySlow(Slow);
	}
	else if(LinkBrain)
	{
		LinkBrain->ApplySlow(Slow);
	}
}

void ASnakeLink::ApplySpeed(FSpeedPower Speed)
{
	if (IsHead())
	{
		ApplySpeed(Speed);
	}
	else if (LinkBrain)
	{
		LinkBrain->ApplySpeed(Speed);
	}
}

void ASnakeLink::AdjustMovementSpeed()
{
	float SlowModifier = GetSlowModifier();
	float SpeedModifier = GetSpeedModifier();

	MovementSpeed = BaseMovementSpeed * (SlowModifier * SpeedModifier) * GlobalSpeedModifier;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

float ASnakeLink::GetSpeedModifier() const
{
	if (IsHeadConst())
	{
		return GetSpeedModifier();
	}
	else if (LinkBrain)
	{
		return LinkBrain->GetSpeedModifier();
	}

	return 1.0f;
}

float ASnakeLink::GetSlowModifier() const
{
	if (IsHeadConst())
	{
		return GetSlowModifier();
	}
	else if (LinkBrain)
	{
		return LinkBrain->GetSlowModifier();
	}

	return 1.0f;
}

FVector ASnakeLink::GetMovementVelocity()
{
	//Head is moving using velocity. Only return the Heads Velocity
	if (IsHead())
	{
		return GetVelocity();
	}
	else if(LinkBrain)
	{
		return LinkBrain->GetVelocity();
	}

	return FVector::ZeroVector;
}

void ASnakeLink::FirePrimary()
{
	if (PrimaryWeapon && ASnakeLink::CanFire())
	{
		PrimaryWeapon->Fire(FireOffset);

		if (CanPlayWeaponAudio())
		{
			PlayAudio(WeaponFireAudioIndex);
		}
	}
}

bool ASnakeLink::CanFire()
{
	if (GetOverrideCanFire())
	{
		if (IsHead())
		{
			OverrideCanFire();
			return CanFire();
		}
		else if (LinkBrain)
		{
			return LinkBrain->CanFire();
		}
	}

	return PrimaryWeapon ? PrimaryWeapon->CanFire() : false;
}

bool ASnakeLink::GetOverrideCanFire() const
{
	if (IsHeadConst())
	{
		return GetOverrideCanFire();
	}
	else if (LinkBrain)
	{
		return LinkBrain->GetOverrideCanFire();
	}

	return false;
}

void ASnakeLink::OverrideCanFire()
{
	if (PrimaryWeapon)
	{
		PrimaryWeapon->OverrideCanFire(true);
	}
}

int32 ASnakeLink::GetTeamNumber()
{
	if (IsHead())
	{
		return GetTeamNumber();
	}
	else if (LinkBrain)
	{
		return LinkBrain->GetTeamNumber();
	}

	return -1;
}

void ASnakeLink::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASnakeLink, Target);

	DOREPLIFETIME_CONDITION(ASnakeLink, BaseMovementSpeed, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(ASnakeLink, MovementSpeed, COND_SimulatedOnly);
}

void ASnakeLink::PostNetReceiveVelocity(const FVector& NewVelocity)
{
	UCharacterMovementComponent* Move = GetCharacterMovement();
	if (Move)
	{
		Move->Velocity = NewVelocity;
	}
}

void ASnakeLink::PostNetReceiveLocationAndRotation()
{
	Super::PostNetReceiveLocationAndRotation();
}

void ASnakeLink::ClearTimer(FTimerHandle& TimerHandle)
{
	FTimerManager& Timer = GetWorld()->GetTimerManager();
	if (Timer.IsTimerActive(TimerHandle))
	{
		Timer.ClearTimer(TimerHandle);
		TimerHandle.Invalidate();
	}
}

APlayerController* ASnakeLink::GetController()
{
	if (Controller)
	{
		return Cast<APlayerController>(Controller);
	}

	return nullptr;
}

ASnakePlayer* ASnakeLink::GetSnakePlayer() const
{
	if (IsHeadConst())
	{
		return GetSnakePlayer();
	}
	else if(LinkBrain)
	{
		return LinkBrain->GetSnakePlayer();
	}

	return nullptr;
}

void ASnakeLink::PlayAudio(int32 AudioCompIndex)
{
	if (IsHead())
	{
		this->PlayAudio(AudioCompIndex);
	}
	else if (LinkBrain)
	{
		LinkBrain->PlayAudio(AudioCompIndex);
	}
}

void ASnakeLink::PlayAudio(int32 AudioCompIndex, USoundBase* BaseSound)
{
	if (IsHead())
	{
		this->PlayAudio(AudioCompIndex, BaseSound);
	}
	else if (LinkBrain)
	{
		LinkBrain->PlayAudio(AudioCompIndex, BaseSound);
	}
}

void ASnakeLink::StopAudio(int32 AudioCompIndex)
{
	if (IsHead())
	{
		this->StopAudio(AudioCompIndex);
	}
	else if (LinkBrain)
	{
		LinkBrain->StopAudio(AudioCompIndex);
	}
}

bool ASnakeLink::CanPlayWeaponAudio() const
{
	if (IsHeadConst())
	{
		return this->CanPlayWeaponAudio();
	}
	else if(LinkBrain)
	{
		return LinkBrain->CanPlayWeaponAudio();
	}

	return false;
}

void ASnakeLink::StartDissolve()
{
	if (!bDissolveInprogress)
	{
		bDissolveInprogress = true;
	}
}

void ASnakeLink::SetDissolveBlend()
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

void ASnakeLink::InitializeSpawnEffect(bool InDirection)
{
	bSpawnEffectDirection = InDirection;

	InDirection ? SetSpawnEffectBlend(1.0f) : SetSpawnEffectBlend(0.0f);
	
	SpawnEffectProgress = 0.0f;
	SpawnEffectInitialRelativeLocation = SpawnEffectVFX->GetRelativeTransform().GetLocation();

	for (int32 c = 0; c < DMI.Num(); c++)
	{
		DMI[c]->SetScalarParameterValue(ActorHalfHeightName, ActorHalfHeight);
		DMI[c]->SetScalarParameterValue(SpawnOriginOffsetName, SpawnOffset);
		DMI[c]->SetScalarParameterValue(SpawnEffectOutlineWidthName, SpawnEffectOutlineWidth);
	}
}

void ASnakeLink::StartSpawnEffect(bool InDirection)
{
	if (!bPlaySpawnVFX)
	{
		return;
	}

	InitializeSpawnEffect(InDirection);
	SpawnEffectVFX->ActivateSystem();
	bSpawnEffectInprogress = true;
}

void ASnakeLink::SetSpawnEffectBlend(float Blend)
{
	for (int32 c = 0; c < DMI.Num(); c++)
	{
		DMI[c]->SetScalarParameterValue(SpawnBlendName, Blend);
	}
}

void ASnakeLink::SpawnEffectTick()
{
	if (!bSpawnEffectInprogress)
	{
		return;
	}

	float DeltaTime = GetWorld()->GetDeltaSeconds();
	SpawnEffectProgress += DeltaTime;

	if (SpawnEffectProgress >= SpawnEffectDuration)
	{
		FinishSpawnEffect();
	}
	else
	{
		float Blend = SpawnEffectProgress / SpawnEffectDuration;
		
		float Min = 0.0f;
		float Max = 1.0f;
		if (bSpawnEffectDirection)
		{
			Min = 1.0f;
			Max = 0.0f;
		}

		float SpawnBlend = FMath::Lerp(Min, Max, Blend);
		SetSpawnEffectBlend(SpawnBlend);

		FVector Start = SpawnEffectInitialRelativeLocation + FVector(0.0f, 0.0f, ActorHalfHeight);
		FVector Finish = SpawnEffectInitialRelativeLocation - FVector(0.0f, 0.0f, ActorHalfHeight);

		FVector RelativeLocation = FMath::Lerp(Start, Finish, Blend);
		SpawnEffectVFX->SetRelativeLocation(RelativeLocation);
	}
}

void ASnakeLink::FinishSpawnEffect()
{
	SpawnEffectVFX->DeactivateSystem();

	bSpawnEffectDirection ? SetSpawnEffectBlend(0.0f) : SetSpawnEffectBlend(1.1f);

	SpawnEffectProgress = 0.0f;
	bSpawnEffectInprogress = false;
}