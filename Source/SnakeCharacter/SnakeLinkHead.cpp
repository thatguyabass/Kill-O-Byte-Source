// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "SnakeLinkHead.h"
#include "SnakePlayer.h"
#include "Powers/Weapons/Weapon.h"
#include "Powers/Shield/ForceField.h"
#include "Powers/Projectiles/BaseProjectile.h"
#include "GameMode/PlayerState/SnakePlayerState.h"
#include "GameMode/BaseGameMode.h"
#include "Utility/PlayerAttackTypeAsset.h"
#include "Utility/DataTables/StatData.h"
#include "UserData/UserData.h"

const FName ASnakeLinkHead::RotateX("RotateX");
const FName ASnakeLinkHead::RotateY("RotateY");

const FString ASnakeLinkHead::CharacterHealthContext("CharacterHealthContext");
const FString ASnakeLinkHead::ForceFieldContext("CharacterShieldContext");
const FString ASnakeLinkHead::WeaponContext("CharacterWeaponContext");

const int32 ASnakeLinkHead::MaxBodies = 4;
const int32 ASnakeLinkHead::WallShapeIndex = 0;
const int32 ASnakeLinkHead::FocusShapeIndex = 1;
const int32 ASnakeLinkHead::LazerShapeIndex = 2;
const int32 ASnakeLinkHead::ShieldShapeIndex = 3;
const int32 ASnakeLinkHead::SpeedShapeIndex = 4;
const int32 ASnakeLinkHead::SnakeShapeIndex = 5;

const float ASnakeLinkHead::BaseSpeedModifier = 1.0f;
const float ASnakeLinkHead::BaseSlowModifier = 1.0f;
const float ASnakeLinkHead::BaseShotDuration = 1.0f;
const float ASnakeLinkHead::SlowShotDuration = 1.5f;

ASnakeLinkHead::ASnakeLinkHead(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	CameraBoom = PCIP.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 10.0f;
	CameraBoom->SocketOffset = FVector(0, 0, 2000.0f);

	//make sure the Camera boom isn't affected by the Target's rotation
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bInheritYaw = false;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("CharacterCapsule_Pawn"));

	BoxComponent->SetCollisionProfileName(TEXT("Pawn"));
	BoxComponent->SetCollisionResponseToChannel(ECC_JumpPadChannel, ECR_Overlap);

	CameraComp = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("CameraComponent"));
	CameraComp->AttachTo(CameraBoom);
	CameraComp->bUsePawnControlRotation = false;
	CameraComp->SetRelativeRotation(FRotator(-85.0f, 0.0f, 0.0f));

	//Sphere Collider that is used to collect Pickups and other link bodies
	CollectionSphere = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("CollectionSphere"));
	CollectionSphere->SetSphereRadius(150.0f);
	CollectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollectionSphere->SetCollisionObjectType(ECC_PowerUpChannel);
	CollectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollectionSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	CollectionSphere->SetCollisionResponseToChannel(ECC_SnakeTraceChannel, ECR_Overlap);
	CollectionSphere->AttachTo(RootComponent);

	WeaponFireAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("WeaponFireAduioComponent"));
	WeaponFireAudioComponent->bAlwaysPlay = false;
	WeaponFireAudioComponent->bAutoActivate = false;
	WeaponFireAudioComponent->AttachTo(RootComponent);

	ShapeAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ShapeAudioComponent"));
	ShapeAudioComponent->bAlwaysPlay = false;
	ShapeAudioComponent->bAutoActivate = false;
	ShapeAudioComponent->AttachTo(RootComponent);

	bCanPlayShapeAudio = false;

	NullZoneAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("NullZoneAudioComponent"));
	NullZoneAudioComponent->bAlwaysPlay = false;
	NullZoneAudioComponent->bAutoActivate = false;
	NullZoneAudioComponent->AttachTo(RootComponent);

	OneOffAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("OneOffAudioComponent"));
	OneOffAudioComponent->bAlwaysPlay = false;
	OneOffAudioComponent->bAutoActivate = false;
	OneOffAudioComponent->AttachTo(RootComponent);

	TargetCursor = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("TargetCursor"));
	TargetCursor->AttachTo(RootComponent);
	InterpRate = 10.0f;

	RotationRate = 180.0f;

	BodyTargetComponent01 = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("BodyTargetComponent01"));
	BodyTargetComponent02 = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("BodyTargetComponent02"));
	BodyTargetComponent03 = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("BodyTargetComponent03"));
	BodyTargetComponent04 = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("BodyTargetComponent04"));

	BodyTargetComponent01->AttachTo(RootComponent);
	BodyTargetComponent02->AttachTo(RootComponent);
	BodyTargetComponent03->AttachTo(RootComponent);
	BodyTargetComponent04->AttachTo(RootComponent);

	BodyTargetArray.Add(BodyTargetComponent01);
	BodyTargetArray.Add(BodyTargetComponent02);
	BodyTargetArray.Add(BodyTargetComponent03);
	BodyTargetArray.Add(BodyTargetComponent04);

	LineTraceDepth = 250.0f;
	TraceProgress = 0.0f;
	Frequency = 0.25f;

	bCanFire = true;
	bFireHeld = false;

	bNullifyState = false;
	NullifyProgress = 0.0f;
	NullifyDuration = 5.0f;

	LazerCooldown = 5.0f;
	bLazerCooldownFlag = false;

	bShapeChanged = false;
	ChangeFireDelay = 0.35f;
	ChangeFireDelayProgress = 0.0f;

	MaxHealth = 0;
	Health = 0;
	bDead = false;

	bSetNextWeaponCue = false;

	MaxSlope = 41.0f;
	CurrentState = ESnakeState::Head;

	CurrentShapeIndex = -1;
	AttackTypeIndex = -1;

	MinThumbStickDeflection = 0.1f;
	LazerLerpBlend = 0.05f;

	MaxSpeedModifier = 2.5f;
	MaxSlowModifier = 0.8f;
}

void ASnakeLinkHead::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	ASnakeLink::SetupPlayerInputComponent(InputComponent);

	//Setup Actions
	InputComponent->BindAction("ChangeShape01", IE_Pressed, this, &ASnakeLinkHead::SetShape01);
	InputComponent->BindAction("ChangeShape02", IE_Pressed, this, &ASnakeLinkHead::SetShape02);
	InputComponent->BindAction("ChangeShape03", IE_Pressed, this, &ASnakeLinkHead::SetShape03);
	InputComponent->BindAction("ChangeShape04", IE_Pressed, this, &ASnakeLinkHead::SetShape04);
	//InputComponent->BindAction("ChangeShape05", IE_Pressed, this, &ASnakeLinkHead::SetShape05);
	InputComponent->BindAction("ChangeShape06", IE_Pressed, this, &ASnakeLinkHead::SetShape06);

	InputComponent->BindAction("ChangeShape04", IE_Released, this, &ASnakeLinkHead::ShieldButtonReleased);

	InputComponent->BindAction("ChangeAttack01", IE_Pressed, this, &ASnakeLinkHead::SetAttackType01);
	InputComponent->BindAction("ChangeAttack02", IE_Pressed, this, &ASnakeLinkHead::SetAttackType02);
	InputComponent->BindAction("ChangeAttack03", IE_Pressed, this, &ASnakeLinkHead::SetAttackType03);
	InputComponent->BindAction("ChangeAttack04", IE_Pressed, this, &ASnakeLinkHead::SetAttackType04);

	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ASnakeLinkHead::FirePrimary);
	InputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ASnakeLinkHead::FireReleased);

	//Controller Only Binds 
	InputComponent->BindAction("Secondary", EInputEvent::IE_Pressed, this, &ASnakeLinkHead::SecondaryPressed);
	InputComponent->BindAction("Secondary", EInputEvent::IE_Released, this, &ASnakeLinkHead::SecondaryReleased);

	InputComponent->BindAction("Controller_Face_Bottom", EInputEvent::IE_Pressed, this, &ASnakeLinkHead::ControllerFaceBottom);
	InputComponent->BindAction("Controller_Face_Top", EInputEvent::IE_Pressed, this, &ASnakeLinkHead::ControllerFaceTop);
	InputComponent->BindAction("Controller_Face_Right", EInputEvent::IE_Pressed, this, &ASnakeLinkHead::ControllerFaceRight);
	InputComponent->BindAction("Controller_Face_Left", EInputEvent::IE_Pressed, this, &ASnakeLinkHead::ControllerFaceLeft);
	InputComponent->BindAction("Controller_DPad_Up", EInputEvent::IE_Pressed, this, &ASnakeLinkHead::ControllerDPadUp);

	InputComponent->BindAction("Controller_Right_Shoulder", EInputEvent::IE_Pressed, this, &ASnakeLinkHead::ControllerRightShoulder);
	InputComponent->BindAction("Controller_Left_Shoulder", EInputEvent::IE_Pressed, this, &ASnakeLinkHead::ControllerLeftShoulder);
	InputComponent->BindAction("Controller_Left_Shoulder", EInputEvent::IE_Released, this, &ASnakeLinkHead::ShieldButtonReleased);

	//Setup Axis
	InputComponent->BindAxis("Forward", this, &ASnakeLinkHead::MoveForward);
	InputComponent->BindAxis("Right", this, &ASnakeLinkHead::MoveRight);

	InputComponent->BindAxis(RotateX);
	InputComponent->BindAxis(RotateY);

	InputComponent->BindAxis(RotateX, this, &ASnakeLinkHead::FirePrimaryThumbStick);
	InputComponent->BindAxis(RotateY, this, &ASnakeLinkHead::FirePrimaryThumbStick);
}

void ASnakeLinkHead::PostInitializeComponents()
{
	//Check Locals Here 
	Super::PostInitializeComponents();
}

void ASnakeLinkHead::PossessedBy(AController* InController)
{
	Super::PossessedBy(InController);

	if (ensure(PlayerDataAsset) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, "No Valid Player Attack Type Data Asset Found. Cant proceed with spawn");
		return;
	}

	FStatDataRow* StatData = CharacterDataTable->FindRow<FStatDataRow>(CharacterRowName, CharacterHealthContext, false);
	ensure(StatData);

	MaxHealth = StatData->Health;

	for (int32 c = 0; c < WeaponRowNames.Num(); c++)
	{
		FWeaponStatDataRow* WeaponData = WeaponDataTable->FindRow<FWeaponStatDataRow>(WeaponRowNames[c], WeaponContext, false);
		ensure(WeaponData);

		FReloadData Data = FReloadData();
		Data.FireRate = WeaponData->FireRate;
		ReloadData.Add(Data);
	}

	ShotDurationScale = BaseShotDuration;

	SpawnBodies();
	SpawnForceField();
	SetHealth(MaxHealth);
	SetCameraRotation();

	RetrieveControllerSettings();

	bool ManualPlay = GetSnakePlayer()->bPlaySpawnEffect;

	if (bPlaySpawnVFX && !ManualPlay)
	{
		InitializeSpawnEffect(false);
	}
	else
	{
		StartSpawnEffect(false);
	}
}

void ASnakeLinkHead::MoveForward(float Value)
{
	if (Controller && Value != 0.0f && !bSuppressInput)
	{	
		FVector Forward = FVector::ForwardVector;
		//if (!CurrentShape.bFreeMovement)
		//{
		//	FRotator Rotation = GetActorRotation();
		//	FRotator Yaw = FRotator(0.0f, Rotation.Yaw, 0.0f);

		//	Forward = FRotationMatrix(Yaw).GetUnitAxis(EAxis::X);
		//	Value = 1.0f;
		//}

		AddMovementInput(Forward, Value);
	}
}

void ASnakeLinkHead::MoveRight(float Value)
{
	if (Controller && Value != 0.0f && !bSuppressInput)
	{
		//if (!CurrentShape.bFreeMovement)
		//{
		//	float DeltaTime = GetWorld()->GetDeltaSeconds();
		//	float Rate = (Value * RotationRate) * DeltaTime;

		//	FRotator MyRotation = GetActorRotation();
		//	FRotator Rotation = MyRotation + FRotator(0.0f, Rate, 0.0f);

		//	SetActorRotation(Rotation);
		//}
		//else
		//{
			const FVector Right = FVector::RightVector;
			AddMovementInput(Right, Value);
		//}
	}
}

void ASnakeLinkHead::AddControllerYawInput(float Value)
{
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	Value *= DeltaTime * RotationRate;

	Super::AddControllerYawInput(Value);
}

void ASnakeLinkHead::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PowersTick(DeltaTime);

	TraceProgress += DeltaTime;
	if (TraceProgress >= Frequency)
	{
		OrientToGround();
		TraceProgress = 0.0f;
	}

	//bool Orbit = !CurrentShape.bFollowCursorAndShoot;

	//if (!CurrentShape.bFreeMovement)
	//{
	//	MoveForward(1.0f);
	//}
	//else if(Orbit)
	//{
		OrientToCursor();
	//}

	// Continue to fire as long as the fire button is held 
	if (bFireHeld)
	{
		FirePrimary();
	}

	if ((CurrentShape.bUseSecondaryFire && !LazerBeingFired()) || (CurrentShape.bUseShield && IsShieldDown()))
	{
		if (CurrentShape.bUseShield)
		{
			//Deactivate the shield to prevent it from reappearing when recharged. 
			ForceField->bIsActive = false;
		}

		ChangeShape(LastShapeIndex);
	}

	if (bShapeChanged)
	{
		ChangeFireDelayProgress += DeltaTime;
		if (ChangeFireDelayProgress >= ChangeFireDelay)
		{
			ChangeFireDelayProgress = 0.0f;
			bShapeChanged = false;
		}
	}

	if (bNullifyState)
	{
		NullifyProgress += DeltaTime;
		if (NullifyProgress >= NullifyDuration)
		{
			ExitNullifyState();
		}
	}

	if (!bCanFire)
	{
		FReloadData& Reload = ReloadData[AttackTypeIndex];

		Reload.Progress += DeltaTime;
		if (Reload.Progress > (Reload.FireRate * ShotDurationScale))
		{
			Reload.Reset();
			ResetCanFire();
		}
	}

	//Reset each tick.
	bDamageOverride = false;
}

void ASnakeLinkHead::OrientToGround()
{
	const FVector Start = GetActorLocation();
	const FVector Down = GetActorUpVector() * -1;
	const FVector End = Start + Down * LineTraceDepth;
	const FName TraceName("OrientTrace");

	FCollisionQueryParams Params(TraceName, false, this);
	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);

	FHitResult Hit(ForceInit);
	bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjParams, Params);

	if (bHit)
	{
		FVector Forward = GetActorForwardVector();
		FMatrix Matrix = FRotationMatrix::MakeFromZX(Hit.ImpactNormal, Forward);
		FRotator Orient = Matrix.Rotator();

		Orient.Yaw = GetActorRotation().Yaw;
		Clamp(Orient.Pitch);
		Clamp(Orient.Roll);

		SetActorRotation(Orient);
	}
	else
	{
		SetActorRotation(FRotator(0.0f, GetActorRotation().Yaw, 0.0f));
	}
}

void ASnakeLinkHead::Clamp(float& Axis)
{
	if (Axis > MaxSlope)
	{
		Axis = MaxSlope;
	}
	else if (Axis < -MaxSlope)
	{
		Axis = -MaxSlope;
	}
}

void ASnakeLinkHead::OrientToCursor()
{
	if (!InputComponent)
	{
		return;
	}

	if (Controller)
	{
		float DeltaX = InputComponent->GetAxisValue(RotateX);
		float DeltaY = InputComponent->GetAxisValue(RotateY);
		FVector DeltaDirection = FVector(DeltaX, DeltaY, 0.0f);
		float DeltaTime = GetWorld()->GetDeltaSeconds();

		if (!bUseController)
		{
			//Get the Mouse Location, and Direction
			float MouseLocationX = 0.0f;
			float MouseLocationY = 0.0f;
			FVector MouseWorldLocation, MouseWorldDirection;
			GetController()->GetMousePosition(MouseLocationX, MouseLocationY);
			GetController()->DeprojectScreenPositionToWorld(MouseLocationX, MouseLocationY, MouseWorldLocation, MouseWorldDirection);
			DeltaDirection = MouseWorldDirection;

			if (MouseLocationX == 0.0f && MouseLocationY == 0.0f)
			{
				return;
			}
		}
		else if (bUseController)
		{
			//If no/very little stick deflection is detected, don't orient to a new direction. 
			if (DeltaDirection.Size() < MinThumbStickDeflection)
			{
				return;
			}
		}

		//Exclude Z from the Mouse Direction and get the rotation
		float DY = DeltaDirection.Y * -1;
		FVector Direction2D = FVector(DeltaDirection.X, DY, 0.0f);
		const FRotator TargetRotation = FRotationMatrix::MakeFromX(Direction2D).Rotator();

		//Set the new Relative Location of the Target Cursor
		FVector TargetLocation = FVector(200.0f, 0.0f, 0.0f);
		FVector T = TargetRotation.UnrotateVector(TargetLocation);
		TargetCursor->SetRelativeLocation(T);

		// Get the Direction of the Actor to the Target Cursor's relative location
		FVector MyLocation = FVector::ZeroVector;
		FVector TargetRelativeLocation = TargetCursor->RelativeLocation;
		const FVector TargetDirection = TargetRelativeLocation - MyLocation;
		const FRotator Rotation = FRotationMatrix::MakeFromX(TargetDirection).Rotator();
		FRotator Yaw = FRotator(GetActorRotation().Pitch, Rotation.Yaw, GetActorRotation().Roll);

		if (LazerBeingFired())
		{
			FRotator MyRotation = GetActorRotation();
			FRotator RLerp = FMath::Lerp(MyRotation, Yaw, LazerLerpBlend);

			SetActorRotation(RLerp);
		}
		else
		{
			//Set the Actors New Yaw Rotation
			SetActorRotation(Yaw);
		}
	}
}

void ASnakeLinkHead::SpawnBodies()
{
	if (BodyClass)
	{
		ASnakeLink* Tail = this;
		for (int32 c = 0; c < MaxBodies; c++)
		{
			FTransform SpawnTransform = GetBodyTarget(c)->GetComponentTransform();
			ASnakeLink* Temp = Cast<ASnakeLink>(UGameplayStatics::BeginSpawningActorFromClass(this, BodyClass, SpawnTransform, true));

			Temp->BodyIndex = c;
			Temp->LinkBrain = this;
			Temp->Instigator = Instigator;
			Temp->SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			UGameplayStatics::FinishSpawningActor(Temp, SpawnTransform);

			Temp->SetTarget(Tail);
			Tail = Temp;

			//Prevent the head and bodies from colliding with each other
			Temp->MoveIgnoreActorAdd(this);
			MoveIgnoreActorAdd(Temp);

			Temp->AdjustMovementSpeed();

			LinkBodies.Add(Temp);
		}

		SetShape01();
		SetAttackType01();

		bCanPlayShapeAudio = true;
	}
}

bool ASnakeLinkHead::IsRidgidShape() const
{
	return CurrentShape.bRidgidShape;
}

int32 ASnakeLinkHead::GetShapeIndex() const
{
	return CurrentShapeIndex;
}

bool ASnakeLinkHead::IsShapeName(EShapeName Name)
{
	return (CurrentShape.Name == Name);
}

bool ASnakeLinkHead::IsShieldButtonHeld() const
{
	return bShieldButtonHeld;
}

void ASnakeLinkHead::EnterSpeedShape()
{
	//Disable Shape Audio because the Speed Gate audio will play
	bCanPlayShapeAudio = false;

	//Set speed shape 
	SetShape05();

	//Ensure we are in the speed form 
	if (InSpeedShape())
	{
		//Prevent shape changing 
		bDisableShapeChanging = true;
	}
}

void ASnakeLinkHead::ExitSpeedShape()
{
	//Enable Shape Changing again 
	bDisableShapeChanging = false;

	//Revert to the last shape 
	ChangeShape(LastShapeIndex);

	//Reenable Shape Audio
	bCanPlayShapeAudio = true;
}

void ASnakeLinkHead::EnterNullifyState()
{
	//ensure Null State doesn't state 
	if (bNullifyState || IsInvulnerable())
	{
		return;
	}

	NullifyProgress = 0.0f;
	bNullifyState = true;

	if (CurrentShapeIndex == LazerShapeIndex || CurrentShapeIndex == ShieldShapeIndex)
	{
		if (CurrentShapeIndex == LazerShapeIndex && LazerBeingFired())
		{
			TerminateProjectile();
			StartLazerCooldown();
		}

		ChangeShape(LastShapeIndex);
	}

	//Disable Shape changing 
	bDisableShapeChanging = true;

	//Increase the shot times by 50%
	ShotDurationScale = SlowShotDuration;

	ASnakePlayer* SnakePlayer = GetSnakePlayer();
	if (SnakePlayer)
	{
		SnakePlayer->AddNullStateWidget();
	}

	PlayAudio(NullAudioIndex);
}

void ASnakeLinkHead::ExitNullifyState()
{
	//Enable Shape Changing
	bDisableShapeChanging = false;

	//Reset the shot duration 
	ShotDurationScale = BaseShotDuration;

	bNullifyState = false;
	NullifyProgress = 0.0f;

	ASnakePlayer* SnakePlayer = GetSnakePlayer();
	if (SnakePlayer)
	{
		SnakePlayer->RemoveNullStateWidget();
	}

	StopAudio(NullAudioIndex);
}

USceneComponent* ASnakeLinkHead::GetBodyTarget(int32 BodyIndex)
{
	return BodyTargetArray.IsValidIndex(BodyIndex) ? BodyTargetArray[BodyIndex] : nullptr;
}

void ASnakeLinkHead::ChangeShape(int32 NewShape)
{
	if (NewShape == CurrentShapeIndex || bDisableShapeChanging || bSuppressInput)
	{
		return;
	}
	else if (NewShape == LazerShapeIndex && IsLazerOnCooldown())
	{
		return;
	}
	else if (NewShape == ShieldShapeIndex && IsShieldDown())
	{
		return;
	}

	if (!Shapes.IsValidIndex(NewShape))
	{
		return;
	}

	FShapeMode LastShape;
	if (Shapes.IsValidIndex(CurrentShapeIndex))
	{
		LastShape = Shapes[CurrentShapeIndex];
	}

	if (!bNullifyState && LastShape.bUseSecondaryFire && LazerBeingFired())
	{
		return;
	}

	LastShapeIndex = (CurrentShapeIndex != ShieldShapeIndex && CurrentShapeIndex != SpeedShapeIndex) ? CurrentShapeIndex : LastShapeIndex;
	CurrentShapeIndex = NewShape;
	CurrentShape = Shapes[NewShape];
	for (int32 c = 0; c < BodyTargetArray.Num(); c++)
	{
		FTransform RelativeTrans = CurrentShape.RelativeLocations[c];
		BodyTargetArray[c]->SetRelativeTransform(RelativeTrans);
	}

	ApplyMovementChanges();

	if (bCanPlayShapeAudio)
	{
		PlayAudio(ShapeAudioIndex);
	}

	ToggleForceField();

	//Spawn the Secondary Weapon
	if (CurrentShape.bUseSecondaryFire)
	{
		SpawnSecondary();
		FirePrimary();
		FireReleased();
	}
	else if (LastShape.bUseSecondaryFire)
	{
		ChangePrimary(GetAttackTypeWeapon(), this);
	}
}

void ASnakeLinkHead::SetShape01()
{
	ChangeShape(0);
}
void ASnakeLinkHead::SetShape02()
{
	if (IsFocusUnlocked())
	{
		ChangeShape(1);
	}
}
void ASnakeLinkHead::SetShape03()
{
	if (IsLazerUnlocked() && !IsLazerOnCooldown())
	{
		ChangeShape(2);
	}
}
void ASnakeLinkHead::SetShape04()
{
	if (IsShieldUnlocked())
	{
		bShieldButtonHeld = true;
		ChangeShape(3);
	}
}
void ASnakeLinkHead::SetShape05()
{
	//if (IsSpeedUnlocked())
	//{
	ChangeShape(4);
	//}
}
void ASnakeLinkHead::SetShape06()
{
	ChangeShape(5);
}

void ASnakeLinkHead::ShieldButtonReleased()
{
	//Shield Button Held can be terminated early if a new shape is choosen. See ToggleForceField()
	if (bShieldButtonHeld)
	{
		ChangeShape(LastShapeIndex);
		bShieldButtonHeld = false;
	}
}

bool ASnakeLinkHead::InLazerShape() const
{
	return CurrentShapeIndex == LazerShapeIndex;
}

bool ASnakeLinkHead::InFocusShape() const
{
	return CurrentShapeIndex == FocusShapeIndex;
}

bool ASnakeLinkHead::InShieldShape() const
{
	return CurrentShapeIndex == ShieldShapeIndex;
}

bool ASnakeLinkHead::InSpeedShape() const
{
	return CurrentShapeIndex == SpeedShapeIndex;
}
EAttackMode ASnakeLinkHead::GetAttackMode()
{
	return CurrentAttackType.AttackMode;
}

FAttackType ASnakeLinkHead::GetAttackType()
{
	return CurrentAttackType;
}

int32 ASnakeLinkHead::GetTeamNumber()
{
	ASnakePlayerState* State = Cast<ASnakePlayerState>(PlayerState);
	return State ? State->GetTeamNumber() : -1;
}

int32 ASnakeLinkHead::GetAttackTypeIndex() const
{
	return AttackTypeIndex;
}

void ASnakeLinkHead::ChangeAttackType(int32 NewAttackType)
{
	//Prevent this call if the new attack type is the current attack type
	if (AttackTypeIndex == NewAttackType || bSuppressInput)
	{
		return;
	}

	AttackTypeIndex = NewAttackType;

	CurrentAttackType = PlayerDataAsset->Data[NewAttackType];

	OnAttackTypeChange();

	UpdateColors(CurrentAttackType.ColorData);
	for (int32 c = 0; c < LinkBodies.Num(); c++)
	{
		LinkBodies[c]->UpdateColors(CurrentAttackType.ColorData);
	}

	if (ForceField)
	{
		ForceField->SetAttackType(CurrentAttackType);
	}

	if (CurrentShape.bUseSecondaryFire == false)
	{
		if (WeaponFireSoundCues.IsValidIndex(AttackTypeIndex))
		{
			bSetNextWeaponCue = true;
		}

		ChangePrimary(GetAttackTypeWeapon(), this);

		bShapeChanged = true;
		ChangeFireDelayProgress = 0.0f;
	}
}

void ASnakeLinkHead::SetAttackType01()
{
	ChangeAttackType(0);
}
void ASnakeLinkHead::SetAttackType02()
{
	if (IsAttackType02Unlocked())
	{
		ChangeAttackType(1);
	}
}
void ASnakeLinkHead::SetAttackType03()
{
	if (IsAttackType03Unlocked())
	{
		ChangeAttackType(2);
	}
}
void ASnakeLinkHead::SetAttackType04()
{
	if (IsAttackType04Unlocked())
	{
		ChangeAttackType(3);
	}
}

bool ASnakeLinkHead::IsFocusUnlocked() const
{
	return GetSnakePlayer()->GetFocusShapeUnlocked();
}
bool ASnakeLinkHead::IsLazerUnlocked() const
{
	return GetSnakePlayer()->GetLazerShapeUnlocked();
}
bool ASnakeLinkHead::IsSpeedUnlocked() const
{
	return GetSnakePlayer()->GetSpeedShapeUnlocked();
}
bool ASnakeLinkHead::IsShieldUnlocked() const
{
	return GetSnakePlayer()->GetShieldShapeUnlocked();
}

bool ASnakeLinkHead::IsAttackType02Unlocked() const
{
	return GetSnakePlayer()->GetAttackType02Unlocked();
}
bool ASnakeLinkHead::IsAttackType03Unlocked() const
{
	return GetSnakePlayer()->GetAttackType03Unlocked();
}
bool ASnakeLinkHead::IsAttackType04Unlocked() const
{
	return GetSnakePlayer()->GetAttackType04Unlocked();
}

void ASnakeLinkHead::ReduceHealth(int32 Value, ASnakePlayerState* Damager)
{
	if (Value <= 0 || !CanTakeDamage())
	{
		return;
	}

	if (ForceField && ForceField->bIsActive && !ForceField->IsShutdown())
	{
		Value = ForceField->ReduceHealth(Value);
		if (!ForceField->IsShutdown())
		{
			return;
		}
	}

	PlayAudio(OneOffAudioIndex, HitSFX);

	Health -= Value;

	if (Health <= 0)
	{
		Killed(Damager);
	}
}

void ASnakeLinkHead::AddHealth(int32 Value)
{
	if (Value < 0)
	{
		Value = 0;
	}

	Health += Value;
	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}
}

void ASnakeLinkHead::SetHealth(int32 Value)
{
	Health = Value;
	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}
}

int32 ASnakeLinkHead::GetHealth() const
{
	return Health;
}

int32 ASnakeLinkHead::GetTotalHealth() const
{
	return MaxHealth;
}

bool ASnakeLinkHead::IsDead() const
{
	return bDead;
}

void ASnakeLinkHead::OverrideInvulnerable()
{
	bDamageOverride = true;
}

bool ASnakeLinkHead::IsInvulnerable()
{
	ASnakePlayer* Cont = GetSnakePlayer();
	if (Cont)
	{
		return Cont->IsPawnInvulnerable();
	}

	return false;
}

bool ASnakeLinkHead::CanTakeDamage()
{
	bool Invulnerable = IsInvulnerable();
	if (!Invulnerable || bDamageOverride)
	{
		return true;
	}
	else if (Invulnerable)
	{
		return false;
	}

	//Idk how we got here... but return true.
	return true;
}

void ASnakeLinkHead::StartMaterialFlicker()
{
	Super::StartMaterialFlicker();

	for (int32 c = 0; c < LinkBodies.Num(); c++)
	{
		LinkBodies[c]->StartMaterialFlicker();
	}
}

void ASnakeLinkHead::EndMaterialFlicker()
{
	Super::EndMaterialFlicker();

	for (int32 c = 0; c < LinkBodies.Num(); c++)
	{
		LinkBodies[c]->EndMaterialFlicker();
	}
}

void ASnakeLinkHead::Killed(ASnakePlayerState* Killer)
{
	if (!IsDead())
	{
		ABaseGameMode* GameMode = Cast<ABaseGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			GameMode->SnakeKilled(Killer, Cast<ASnakePlayerState>(PlayerState));
		}

		ASnakePlayerState* SnakePlayerState = Cast<ASnakePlayerState>(PlayerState);
		if (SnakePlayerState)
		{
			SnakePlayerState->ReducePlayerLives(1);
		}

		if (CurrentShape.bUseSecondaryFire && LazerBeingFired())
		{
			TerminateProjectile();
		}

		OnDead();

		PlayAudio(OneOffAudioIndex, DeathSFX);

		StopAudio(NullAudioIndex);

		if (Controller)
		{
			GetSnakePlayer()->RemoveNullStateWidget();
		}

		bFireHeld = false;
		bDead = true;
		HideAndDestroy();
		DetachFromControllerPendingDestroy();

		CleanUpBody();
	}
}

void ASnakeLinkHead::DespawnAndDestroy()
{
	if (!IsDead())
	{
		bDead = true;

		ASnakeLink::DespawnAndDestroy();

		for (int32 c = 0; c < LinkBodies.Num(); c++)
		{
			LinkBodies[c]->DespawnAndDestroy();
		}
	}
}

void ASnakeLinkHead::ApplyMovementChanges()
{
	AdjustMovementSpeed();

	for (int32 c = 0; c < LinkBodies.Num(); c++)
	{
		LinkBodies[c]->AdjustMovementSpeed();
	}
}

void ASnakeLinkHead::ApplySpeed(FSpeedPower Speed)
{
	SpeedPickupPower = Speed;
	SpeedPickupPower.bIsActive = true;

	ApplyMovementChanges();
}

float ASnakeLinkHead::GetSpeedModifier() const
{
	float Speed = CurrentShape.SpeedModifier 
		+ (SpeedBoostPower.bIsActive ? SpeedBoostPower.SpeedModifier : 0.0f)
		+ (SpeedPickupPower.bIsActive ? SpeedPickupPower.SpeedModifier : 0.0f);
	if (Speed > MaxSpeedModifier)
	{
		Speed = MaxSpeedModifier;
	}

	return BaseSpeedModifier + Speed;
}

void ASnakeLinkHead::SetSpeedBoostPower(FSpeedPower InSpeedBoost)
{
	SpeedBoostPower = InSpeedBoost;

	SpeedBoostPower.bIsActive = true;

	ApplyMovementChanges();
}

void ASnakeLinkHead::SpawnForceField()
{
	if (ForceFieldClass)
	{
		const FVector Location = GetActorLocation();
		const FRotator Rotation = GetActorRotation();

		FTransform SpawnTrans = FTransform(Rotation, Location);
		ForceField = Cast<AForceField>(UGameplayStatics::BeginSpawningActorFromClass(this, ForceFieldClass, SpawnTrans));
		UGameplayStatics::FinishSpawningActor(ForceField, SpawnTrans);

		ForceField->SetShieldOwner(this);
		ForceField->SetAttackType(CurrentAttackType);
		ForceField->bIsActive = false;
		ForceField->bCanRegen = true;

		for (int32 c = 0; c < LinkBodies.Num(); c++)
		{
			LinkBodies[c]->MoveIgnoreActorAdd(ForceField);
		}
	}
}

AForceField* ASnakeLinkHead::GetForceField() const
{
	return ForceField;
}

FAttackType ASnakeLinkHead::GetForceFieldAttackType() const
{
	return ForceField ? ForceField->GetAttackType() : FAttackType(EAttackMode::Attack05_White);
}

void ASnakeLinkHead::ToggleForceField()
{
	if (!ForceField)
	{
		return;
	}

	if (CurrentShape.bUseShield)
	{
		ForceField->ActivateForceField();
	}
	else
	{
		ForceField->DeactivateForceField();
		//Terminate the Held State 
		bShieldButtonHeld = false;
	}
}

bool ASnakeLinkHead::IsShieldDown() const
{
	if (ForceField)
	{
		return ForceField->IsShutdown();
	}

	return false;
}

void ASnakeLinkHead::PowersTick(float DeltaTime)
{
	if (SpeedBoostPower.bIsActive)
	{
		FSpeedPower& B = SpeedBoostPower;
		B.Progress += DeltaTime;
		if (B.Progress > B.Duration)
		{
			B.StopAndReset();
		}
	}

	if (SpeedPickupPower.bIsActive)
	{
		FSpeedPower& S = SpeedPickupPower;
		S.Progress += DeltaTime;
		if (S.Progress > S.Duration)
		{
			S.StopAndReset();
			ApplyMovementChanges();
		}
	}

	if (SlowModifiers.Num() > 0)
	{
		bool SlowChanged = false;

		for (int32 c = 0; c < SlowModifiers.Num(); c++)
		{
			SlowModifiers[c].Progress += DeltaTime;
			if (SlowModifiers[c].Progress > SlowModifiers[c].Duration)
			{
				SlowChanged = true;
				//Set as inactive and remove from the Array
				SlowModifiers[c].bIsActive = false;
				SlowModifiers.RemoveAt(c);
				c--;//Decrement the count to ensure all data is hit 
			}
		}

		if (SlowChanged)
		{
			ApplyMovementChanges();
		}
	}
}

void ASnakeLinkHead::ApplySlow(FSpeedPower SlowModifier)
{
	if (IsInvulnerable())
	{
		return;
	}

	SlowModifier.bIsActive = true;
	SlowModifiers.Add(SlowModifier);

	ApplyMovementChanges();
}

void ASnakeLinkHead::AddStasisSlowModifier(FSpeedPower StatisModifier)
{
	if (IsInvulnerable())
	{
		return;
	}

	StasisSlow = StatisModifier;
	StasisSlow.bIsActive = true;

	ApplyMovementChanges();
}

void ASnakeLinkHead::RemoveStasisSlowModifier()
{
	StasisSlow.bIsActive = false;

	ApplyMovementChanges();
}

float ASnakeLinkHead::GetSlowModifier() const
{
	float Slow = 0.0f;
	for (int32 c = 0; c < SlowModifiers.Num(); c++)
	{
		Slow += SlowModifiers[c].SpeedModifier;
	}

	if (StasisSlow.bIsActive)
	{
		Slow += StasisSlow.SpeedModifier;
	}

	if (Slow > MaxSlowModifier)
	{
		Slow = MaxSlowModifier;
	}

	return BaseSlowModifier - Slow;
}

void ASnakeLinkHead::CleanUpBody()
{
	bFireHeld = false;

	for (int32 c = 0; c < LinkBodies.Num(); c++)
	{
		LinkBodies[c]->HideAndDestroy();
	}
	CleanUpWeapons();

	if (ForceField)
	{
		ForceField->HideAndDestroy(false);
	}

	LinkBodies.Empty();
}

void ASnakeLinkHead::SecondaryPressed()
{
	if (CurrentShapeIndex == WallShapeIndex)
	{
		SetShape02();
	}
	else
	{
		SetShape01();
	}
}

void ASnakeLinkHead::SecondaryReleased()
{
	bSecondaryHeld = false;
	GetSnakePlayer()->SecondaryReleased();
}

bool ASnakeLinkHead::SecondaryHeld() const
{
	return bSecondaryHeld;
}

void ASnakeLinkHead::ControllerFaceBottom()
{
	SetAttackType02();
}

void ASnakeLinkHead::ControllerFaceTop()
{
	SetAttackType03();
}

void ASnakeLinkHead::ControllerFaceRight()
{
	SetAttackType01();
}

void ASnakeLinkHead::ControllerFaceLeft()
{
	SetAttackType04();
}

void ASnakeLinkHead::ControllerDPadUp()
{
	if (CurrentShapeIndex == SnakeShapeIndex)
	{
		//Wall Form
		SetShape01();
	}
	else
	{
		SetShape06();
	}
}

void ASnakeLinkHead::ControllerRightShoulder()
{
	//Lazer
	SetShape03();
}

void ASnakeLinkHead::ControllerLeftShoulder()
{
	//Shield
	SetShape04();
}

void ASnakeLinkHead::FirePrimary()
{
	if (bSuppressInput || IsDead())
	{
		return;
	}

	bFireHeld = true;

	if (bSetNextWeaponCue)
	{
		bSetNextWeaponCue = false;
		//This should always be valid. An index check is done when bSetNextWeaponCue is set to True
		SetWeaponSoundCue(WeaponFireSoundCues[AttackTypeIndex]);
	}

	//Can't Fire the Lazer while its on cooldown
	if (CurrentShape.bUseSecondaryFire && IsLazerOnCooldown() || bShapeChanged)
	{
		return;
	}

	if (CurrentShape.bHeadCanShoot)
	{
		ASnakeLink::FirePrimary();
	}

	for (int32 c = 0; c < LinkBodies.Num(); c++)
	{
		if (CurrentShape.bCanShoot[c])
		{
			LinkBodies[c]->FirePrimary();
		}
	}

	SetCanFire(false);
}

void ASnakeLinkHead::FireReleased()
{
	bFireHeld = false;
}

void ASnakeLinkHead::FirePrimaryThumbStick(float Delta)
{
	if (bUseController && bTwinStickMode)
	{
		if (Delta > MinThumbStickDeflection || Delta < -MinThumbStickDeflection)
		{
			FirePrimary();
		}
		else
		{
			FireReleased();
		}
	}
}

void ASnakeLinkHead::ChangePrimary(TSubclassOf<AWeapon> InWeaponClass, AActor* ProjectileOwner)
{
	if(InWeaponClass)
	{
		//Clean up the current weapons before spawning the new ones 
		CleanUpWeapons();

		bCanFireOverride = true;

		//Change the Active

		ASnakeLink::ChangePrimary(InWeaponClass, this);

		for (int32 c = 0; c < LinkBodies.Num(); c++)
		{
			LinkBodies[c]->ChangePrimary(InWeaponClass, this);
		}
	}
}

void ASnakeLinkHead::SpawnSecondary()
{
	if (CurrentShape.bUseSecondaryFire)
	{
		//Clean up current weapons before spawning new ones 
		CleanUpWeapons();

		bCanFireOverride = false;

		if (CurrentShape.bHeadCanShoot)
		{
			ASnakeLink::ChangePrimary(CurrentShape.SecondaryWeaponClass, this);
		}

		for (int32 c = 0; c < LinkBodies.Num(); c++)
		{
			if (CurrentShape.bCanShoot[c])
			{
				LinkBodies[c]->ChangePrimary(CurrentShape.SecondaryWeaponClass, this);
				LinkBodies[c]->GetPrimaryWeapon()->OnProjectileFinishedFire.BindUObject(this, &ASnakeLinkHead::StartLazerCooldown);
			}
		}
	}
}

TSubclassOf<AWeapon> ASnakeLinkHead::GetAttackTypeWeapon()
{
	uint8 Id = (uint8)GetAttackMode();

	if (PrimaryWeaponClasses.IsValidIndex(Id))
	{
		return PrimaryWeaponClasses[Id];
	}

	return nullptr;
}

void ASnakeLinkHead::IgnoreProjectiles(ABaseProjectile* InProjectile)
{
	InProjectile->AddUniqueIgnoreActor(this);

	for (int32 c = 0; c < LinkBodies.Num(); c++)
	{
		InProjectile->AddUniqueIgnoreActor(LinkBodies[c]);
	}
}

void ASnakeLinkHead::ResetPrimaryWeapon(TSubclassOf<class AWeapon> InPrimaryWeaponClass)
{
	ASnakeLink::ResetPrimaryWeapon(InPrimaryWeaponClass);

	for (int32 c = 0; c < LinkBodies.Num(); c++)
	{
		LinkBodies[c]->ResetPrimaryWeapon(InPrimaryWeaponClass);
	}
}

bool ASnakeLinkHead::LazerBeingFired()
{
	bool Fired = false;
	Fired = ASnakeLink::LazerBeingFired();

	if (Fired)
	{
		return Fired;
	}

	for (int32 c = 0; c < LinkBodies.Num(); c++)
	{
		Fired = LinkBodies[c]->LazerBeingFired();
		if (Fired)
		{
			return Fired;
		}
	}

	return false;
}

void ASnakeLinkHead::TerminateProjectile()
{
	ASnakeLink::TerminateProjectile();

	for (int32 c = 0; c < LinkBodies.Num(); c++)
	{
		LinkBodies[c]->TerminateProjectile();
	}
}

void ASnakeLinkHead::StartLazerCooldown()
{
	if (!LazerCooldown_TimerHandle.IsValid())
	{
		bLazerCooldownFlag = true;
		GetWorldTimerManager().SetTimer(LazerCooldown_TimerHandle, this, &ASnakeLinkHead::FinishLazerCooldown, LazerCooldown, false);
	}
}

void ASnakeLinkHead::FinishLazerCooldown()
{
	if (LazerRechargeSFX)
	{
		UGameplayStatics::PlaySound2D(this, LazerRechargeSFX);
	}

	bLazerCooldownFlag = false;
	LazerCooldown_TimerHandle.Invalidate();
}

bool ASnakeLinkHead::IsLazerOnCooldown()
{
	return bLazerCooldownFlag;
}

bool ASnakeLinkHead::CanFire()
{
	return bCanFire;
}

bool ASnakeLinkHead::GetOverrideCanFire() const
{
	return bCanFireOverride;
}

void ASnakeLinkHead::OverrideCanFire()
{
	if (bCanFireOverride && bCanFire)
	{
		ASnakeLink::OverrideCanFire();
	
		for (int32 c = 0; c < LinkBodies.Num(); c++)
		{
			LinkBodies[c]->OverrideCanFire();
		}
	}
}

void ASnakeLinkHead::SetCanFire(bool InValue)
{
	if (bCanFireOverride)
	{
		bCanFire = InValue;
	}
}

void ASnakeLinkHead::ResetCanFire()
{
	bCanFire = true;
}

float ASnakeLinkHead::GetLazerCooldown()
{
	if (IsLazerOnCooldown())
	{
		return GetWorldTimerManager().GetTimerRemaining(LazerCooldown_TimerHandle);
	}

	return LazerCooldown;
}

void ASnakeLinkHead::CleanUpWeapons()
{
	ASnakeLink::CleanUpWeapons();

	for (int32 c = 0; c < LinkBodies.Num(); c++)
	{
		LinkBodies[c]->CleanUpWeapons();
	}
}

ASnakePlayer* ASnakeLinkHead::GetSnakePlayer() const
{
	if (Controller)
	{
		return Cast<ASnakePlayer>(Controller);
	}

	return nullptr;
}

void ASnakeLinkHead::PlayAudio(int32 AudioCompIndex)
{
	UAudioComponent* InAudioComp = GetAudioComponent(AudioCompIndex);
	if (InAudioComp && InAudioComp->Sound)
	{
		InAudioComp->Play();
	}
}

void ASnakeLinkHead::PlayAudio(int32 AudioCompIndex, USoundBase* BaseSound)
{
	UAudioComponent* InAudioComp = GetAudioComponent(AudioCompIndex);
	if (InAudioComp)
	{
		InAudioComp->SetSound(BaseSound);
		InAudioComp->Play();
	}
}

void ASnakeLinkHead::StopAudio(int32 AudioCompIndex)
{
	UAudioComponent* InAudioComp = GetAudioComponent(AudioCompIndex);
	if (InAudioComp)
	{
		InAudioComp->Stop();
	}
}

bool ASnakeLinkHead::CanPlayWeaponAudio() const
{
	//The lazer has its own audio, don't play the fire sfx
	return CurrentShape.bUseSecondaryFire ? false : true;
}

UAudioComponent* ASnakeLinkHead::GetAudioComponent(int32 AudioCompIndex)
{
	UAudioComponent* Temp = nullptr;

	switch (AudioCompIndex)
	{
	case WeaponFireAudioIndex:	Temp = WeaponFireAudioComponent;	break;
	case ShapeAudioIndex:		Temp = ShapeAudioComponent;			break;
	case NullAudioIndex:		Temp = NullZoneAudioComponent;		break;
	case OneOffAudioIndex:		Temp = OneOffAudioComponent;		break;
	}

	return Temp;
}

void ASnakeLinkHead::SetWeaponSoundCue(USoundCue* InCue)
{
	if (WeaponFireAudioComponent)
	{
		WeaponFireAudioComponent->Stop();
		WeaponFireAudioComponent->SetSound(InCue);
	}
}

void ASnakeLinkHead::SetCameraRotation()
{
	const FVector CameraLocation = CameraComp->GetComponentLocation();
	const FVector Location = GetActorLocation();

	FVector Direction = Location - CameraLocation;
	Direction.Normalize();

	const FRotator Rotation = Direction.Rotation();
	CameraComp->SetRelativeRotation(Rotation);
}

bool ASnakeLinkHead::IsUsingController() const
{
	return bUseController;
}

void ASnakeLinkHead::RetrieveControllerSettings()
{
	UUserData* SavedData = GetSnakePlayer()->GetSavedData();
	if (SavedData)
	{
		bUseController = SavedData->GetInputSetting();
		bTwinStickMode = SavedData->GetTwinStickMode();
	}
	else
	{
		//Default to Using a controller.
		bUseController = true;
		bTwinStickMode = false;
	}
}

bool ASnakeLinkHead::GetTwinStickMode() const
{
	return bTwinStickMode;
}

void ASnakeLinkHead::InitializeSpawnEffect(bool InDirection)
{
	ASnakeLink::InitializeSpawnEffect(InDirection);
	for (int32 c = 0; c < LinkBodies.Num(); c++)
	{
		LinkBodies[c]->InitializeSpawnEffect(InDirection);
	}
}

void ASnakeLinkHead::StartSpawnEffect(bool InDirection)
{
	if (!bPlaySpawnVFX)
	{
		return;
	}

	PlayAudio(OneOffAudioIndex, SpawnSFX);

	ASnakeLink::StartSpawnEffect(InDirection);
	for (int32 c = 0; c < LinkBodies.Num(); c++)
	{
		LinkBodies[c]->StartSpawnEffect(InDirection);
	}
}

void FReloadData::Reset()
{
	Progress = 0.0f;
}