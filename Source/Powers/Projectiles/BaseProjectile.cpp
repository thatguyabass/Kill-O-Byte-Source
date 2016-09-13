// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BaseProjectile.h"
#include "SnakeCharacter/SnakeLink.h"
#include "SnakeCharacter/SnakeLinkHead.h"
#include "Bots/Bot/Bot.h"

ABaseProjectile::ABaseProjectile(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->SetSphereRadius(40.0f);
	CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
	CollisionComp->AlwaysLoadOnServer = true;
	CollisionComp->bCanEverAffectNavigation = false;
	RootComponent = CollisionComp;

	OverlapComp = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapComp"));
	OverlapComp->SetSphereRadius(80.0f);
	OverlapComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapComp->SetCollisionResponseToChannel(ECC_ProjectileChannel, ECR_Overlap);
	OverlapComp->AttachTo(RootComponent);

	OnHitAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("OnHitAudioComponent"));
	OnHitAudioComponent->bAlwaysPlay = false;
	OnHitAudioComponent->bAutoActivate = false;
	OnHitAudioComponent->AttachTo(RootComponent);

	FiredAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FireAudioComponent"));
	FiredAudioComponent->bAlwaysPlay = false;
	FiredAudioComponent->bAutoActivate = false;
	FiredAudioComponent->AttachTo(RootComponent);

	PrimaryColorName = "Primary Color";
	SecondaryColorName = "Secondary Color";
	ParticleLightColorName = "Particle Light Color";
	SizeParameterName = "Particle Size Param";

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	ParticleSystemComponent->AttachTo(RootComponent);

	bOrientHitVFX = true;

	MoveComp = CreateDefaultSubobject<USnakeProjectileMoveComponent>(TEXT("MoveComp"));
	MoveComp->InitialSpeed = 1000;
	MoveComp->ProjectileGravityScale = 0.0f;

	Damage = 1;

	AttackType.AttackMode = EAttackMode::Attack05_White;
	AttackType.ColorData.PrimaryColor = FLinearColor::White;
	AttackType.ColorData.SecondaryColor = FLinearColor::White;

	Direction = FVector::ZeroVector;
	AdditionalSpeed = 0;
	SpeedModifier = 1.0f;

	TraceProgress = 0;
	Frequency = 0.05f;

	bSelfDestruct = true;
	bInAir = false;
	SelfDestructProgress = 0.0f;
	SelfDestructDuration = 2.5f;

	HomingMagnitude = 10000.0f;
	bHomingProjectile = false;

	ExpectedHeight = 110.0f;
	bLastHit = true;
	bPitchLocked = false;

	bPendingDestroy = false;

	/** Ensure this actor ticks */
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bReplicateMovement = true;
	bNetUseOwnerRelevancy = true;
}

void ABaseProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (ProjectileOwner)
	{
		ASnakeLink* Link = Cast<ASnakeLink>(ProjectileOwner);
		if (Link)
		{
			Link->IgnoreProjectiles(this);
		}
		else
		{
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABaseProjectile::EnableCollision, 0.2f, false);
		}
	}

	OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::BeginOverlapCheck);
}

void ABaseProjectile::SetAttackMode(const FAttackType InAttackType)
{
	AttackType = InAttackType;
	FTeamColorData Data = InAttackType.ColorData;

	if (ParticleSystemComponent && ProjectileMaterial)
	{
		//Create a Dynamic Material Instance
		UMaterialInstanceDynamic* DMI = UMaterialInstanceDynamic::Create(ProjectileMaterial, this);
		//Set the Primary and Secondary Colors
		DMI->SetVectorParameterValue(PrimaryColorName, Data.PrimaryColor);
		DMI->SetVectorParameterValue(SecondaryColorName, Data.SecondaryColor);
		//Add this material to the Particle
		ParticleSystemComponent->SetMaterial(0, DMI);
	}

	OnAttackTypeSet(AttackType);
}

FAttackType ABaseProjectile::GetAttackType() const
{
	return AttackType;
}

int32 ABaseProjectile::GetDamage()
{
	return Damage;
}

void ABaseProjectile::SetDamage(int32 InDamage)
{
	Damage = InDamage;
}

void ABaseProjectile::SetExplosionRadius(float InRadius)
{
	//Not all projectiles support AoE attacks
}

float ABaseProjectile::GetExplosionRadius() const
{
	return 0.0f;
}

void ABaseProjectile::SetBounceCount(int32 InBounceCount)
{
	//Not all projectiles support bounce 
}

void ABaseProjectile::SetSlowPercent(float InPercent)
{
	//Not all projectiles support slow effects 
}

void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	PlayAudio(FiredAudioComponent);
}

void ABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TraceProgress += DeltaTime;
	if (TraceProgress > Frequency)
	{
		HeightCheck();
		TraceProgress = 0.0f;
	}

	if (bInAir && bSelfDestruct)
	{
		SelfDestructProgress += DeltaTime;
		if (SelfDestructProgress > SelfDestructDuration)
		{
			//Remove the Hit VFX so it is not displayed
			HitVFX = nullptr;
			HideAndDestroy();
		}
	}
	else
	{
		SelfDestructProgress = 0.0f;
	}
}

FVector ABaseProjectile::GetDirection()
{
	return Direction;
}

void ABaseProjectile::SetDirection(FVector InDirection)
{
	if (Role < ROLE_Authority)
	{
		ServerSetDirection(InDirection);
	}
	else
	{
		Direction = InDirection;
	}
}

void ABaseProjectile::InitVelocity(FVector InDirection, float AdditionalVelocity)
{
	if (MoveComp)
	{
		AdditionalSpeed = AdditionalVelocity;
		SetVelocity(InDirection);
	}
}

void ABaseProjectile::SetVelocity(FVector InDirection)
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	if(MoveComp)
	{
		Direction = InDirection;
		MoveComp->Velocity = Direction * ((MoveComp->InitialSpeed + AdditionalSpeed) * SpeedModifier);
		SetActorRotation(Direction.Rotation());
	}
}

void ABaseProjectile::SetSpeedModifier(float InModifier)
{
	if (InModifier <= 0.0f || InModifier > 1.0f)
	{
		return;
	}

	SpeedModifier = InModifier;
	SetVelocity(Direction);
}

void ABaseProjectile::ResetSpeedModifier()
{
	SetSpeedModifier(1.0f);
}

float ABaseProjectile::GetSpeedModifier() const
{
	return SpeedModifier;
}

void ABaseProjectile::OnImpact(const FHitResult& HitResult)
{
	PlayAudio(OnHitAudioComponent);
	HideAndDestroy();
}

float ABaseProjectile::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageAmount > 1.0f)
	{
		FHitResult Hit(ForceInit);
		Hit.bBlockingHit = true;
		Hit.Location = GetActorLocation();

		HideAndDestroy(Hit);
	}

	return 0.0f;
}

void ABaseProjectile::AddUniqueIgnoreActor(AActor* Actor)
{
	if (Actor != this)
	{
		Cast<UPrimitiveComponent>(Actor->GetRootComponent())->MoveIgnoreActors.AddUnique(this);
		Cast<UPrimitiveComponent>(GetRootComponent())->MoveIgnoreActors.Add(Actor);
	}
}

void ABaseProjectile::MultiAddUniqueIgnoreActor(AActor* Actor)
{
	MultiCastAddIgnoreActor(Actor);
}

void ABaseProjectile::HideAndDestroy(const FHitResult& Hit)
{
	if (bPendingDestroy)
	{
		return;
	}

	bPendingDestroy = true;

	//if (Role < ROLE_Authority)
	//{
	//	ServerHideAndDestroy(Hit);
	//}
	//else
	//{

	OnHitVFX(Hit);
	MoveComp->StopMovementImmediately();

	Frequency = FLT_MAX;
	this->SetActorHiddenInGame(true);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->SetLifeSpan(2.0f);
	//}
}

void ABaseProjectile::NotifyHit(class UPrimitiveComponent* MyComponent, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation,
	FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComponent, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	PlayAudio(OnHitAudioComponent);

	ABaseProjectile* Projectile = Cast<ABaseProjectile>(Other);
	if (Projectile && Projectile->ProjectileOwner && Projectile->ProjectileOwner != ProjectileOwner)
	{
		Projectile->HideAndDestroy();
		HideAndDestroy();
	}
}

void ABaseProjectile::BeginOverlapCheck(class AActor* Other, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Other == this)
	{
		return;
	}

	ABaseProjectile* Projectile = Cast<ABaseProjectile>(Other);
	if (Projectile && ProjectileOwner && Projectile->ProjectileOwner && Projectile->ProjectileOwner == ProjectileOwner)
	{
		if (Cast<UPrimitiveComponent>(GetRootComponent())->MoveIgnoreActors.Contains(Projectile))
		{
			return;
		}
		AddUniqueIgnoreActor(Other);
	}
}

void ABaseProjectile::OnHitVFX(const FHitResult& Hit)
{
	if (!HitVFX)
	{
		return;
	}

	FVector Location = GetActorLocation();
	FRotator Rotation = FRotator::ZeroRotator;

	if (Hit.IsValidBlockingHit() && bOrientHitVFX)
	{
		Rotation = FRotationMatrix::MakeFromX(Hit.ImpactNormal).Rotator();
	}

	UParticleSystemComponent* VFX = UGameplayStatics::SpawnEmitterAtLocation(this, HitVFX, Location, Rotation, true);

	if (VFX && (GetExplosionRadius() > 0.0f))
	{
		FVector SizeVector = FVector(GetExplosionRadius(), 0.0f, 0.0f);

		FParticleSysParam SizeParam;
		SizeParam.ParamType = EParticleSysParamType::PSPT_Vector;
		SizeParam.Vector = SizeVector;
		SizeParam.Name = SizeParameterName;
		VFX->InstanceParameters.Add(SizeParam);
	}
}

void ABaseProjectile::EnableCollision()
{
	CollisionComp->SetCollisionProfileName("Projectile");
}

void ABaseProjectile::MultiCastRemoveCollision_Implementation()
{
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseProjectile::MultiCastEnableCollision_Implementation()
{
	CollisionComp->SetCollisionProfileName("Projectile");
}

void ABaseProjectile::ServerHideAndDestroy_Implementation(const FHitResult& Hit)
{
	HideAndDestroy(Hit);
}

void ABaseProjectile::MultiCastAddIgnoreActor_Implementation(AActor* Actor)
{
	if (Actor)
	{
		AddUniqueIgnoreActor(Actor);
	}
}

void ABaseProjectile::MultiCastOnHitVFX_Implementation(const FRotator Rotation)
{
	if (HitVFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, HitVFX, GetActorLocation(), Rotation, true);
	}
}

void ABaseProjectile::ServerSetDirection_Implementation(FVector InDirection)
{
	SetDirection(InDirection);
}

void ABaseProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseProjectile, ProjectileOwner);
	DOREPLIFETIME(ABaseProjectile, Direction);
	DOREPLIFETIME(ABaseProjectile, AdditionalSpeed);
	DOREPLIFETIME(ABaseProjectile, SpeedModifier);
}

ASnakePlayerState* ABaseProjectile::GetOwnerPlayerState() const
{
	ACharacter* Character = Cast<ACharacter>(ProjectileOwner);
	return Character ? Cast<ASnakePlayerState>(Character->PlayerState) : nullptr;
}

AController* ABaseProjectile::GetOwnerController() const
{
	APawn* Pawn = Cast<APawn>(ProjectileOwner);
	return Pawn ? Pawn->GetController() : nullptr;
}

int32 ABaseProjectile::GetTeamNumber() const
{
	ASnakeLinkHead* Head = Cast<ASnakeLinkHead>(ProjectileOwner);
	if (Head)
	{
		return Head->GetTeamNumber();
	}

	ABot* Bot = Cast<ABot>(ProjectileOwner);
	if (Bot)
	{
		return Bot->TeamNumber;
	}

	return -1;
}

void ABaseProjectile::HeightCheck()
{
	//Check for the Projectile movement component
	if (!MoveComp)
	{
		return;
	}

	const FVector Start = GetActorLocation();
	const FVector Down = GetActorUpVector() * -1;
	const FVector End = Start + Down * (ExpectedHeight * 1.25f);
	const FName TraceName("CheckHeightTrace");

	FCollisionQueryParams Param(TraceName, false, this);
	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);

	FHitResult Hit(ForceInit);
	bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjParams, Param);

	const FRotator Rot = GetActorRotation();
	if (Rot.Pitch == 0.0f)
	{
		bPitchLocked = true;
	}

	if (bHit && !bPitchLocked)
	{
		FVector Location = GetActorLocation();
		float Z = (Hit.Location.Z + ExpectedHeight);
		SetActorLocation(FVector(Location.X, Location.Y, Z));
	}

	if (bHit && !bLastHit)
	{
		//Remove Gravity
		MoveComp->ProjectileGravityScale = 0.0f;
		bInAir = false;

		FVector Forward = GetActorForwardVector();
		FMatrix Matrix = FRotationMatrix::MakeFromXY(Forward, Hit.ImpactNormal);
		FVector Direction = Matrix.Rotator().RotateVector(FVector::ForwardVector);

		SetVelocity(Direction);
	}
	else if (!bHit && bLastHit)
	{
		//Apply Gravity 
		MoveComp->ProjectileGravityScale = 1.0f;
		bInAir = true;

		//Reset the Projectiles Pitch to 0.0f
		FRotator Rotation = GetActorRotation();
		FRotator XY = FRotator(0.0f, Rotation.Yaw, Rotation.Roll);

		SetActorRotation(XY);
	}

	bLastHit = bHit;
}

void ABaseProjectile::TriggerHeightCheck()
{
	//This will force the Height Check and redirect the Projectile
	bLastHit = false;
}

void ABaseProjectile::PostNetReceiveVelocity(const FVector& NewVelocity)
{
	if (MoveComp)
	{
		MoveComp->Velocity = NewVelocity;
	}
}

void ABaseProjectile::PostNetReceiveLocationAndRotation()
{
	if (Role == ROLE_SimulatedProxy)
	{
		if (RootComponent && RootComponent->IsRegistered() && (ReplicatedMovement.Location != GetActorLocation() || ReplicatedMovement.Rotation != GetActorRotation()))
		{
			SetActorLocationAndRotation(ReplicatedMovement.Location, ReplicatedMovement.Rotation, false);
		}
	}
}

void ABaseProjectile::InitializeHoming(USceneComponent* InTargetComponent, float IdealDistance)
{
	MoveComp->HomingTargetComponent = InTargetComponent;
}

void ABaseProjectile::PlayAudio(UAudioComponent* AudioComponent)
{
	if (AudioComponent && AudioComponent->Sound)
	{
		AudioComponent->Play();
	}
}

void ABaseProjectile::StopAudio(UAudioComponent* AudioComponent)
{
	if (AudioComponent)
	{
		AudioComponent->Stop();
	}
}