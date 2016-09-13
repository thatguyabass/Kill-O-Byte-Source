// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "TubeBoss.h"
#include "Bots/TubeBoss/TubeBossAttackPattern.h"
#include "Bots/TubeBoss/TubeTrigger.h"
#include "Bots/Turret/TubeTurret.h"
#include "Bots/Bot/Component/WeakPoint.h"
#include "Utility/DataTables/StatData.h"

const float ATubeBoss::YawRotation = 180.0f;

const FString ATubeBoss::TubeDataTableContext("TubeDataTableContext");

// Sets default values
ATubeBoss::ATubeBoss()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	WeaponPlacementComponent01 = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponPlacement01"));
	WeaponPlacementComponent02 = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponPlacement02"));
	WeaponPlacementComponent03 = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponPlacement03"));
	WeaponPlacementComponent04 = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponPlacement04"));

	WeaponPlacementComponent01->AttachTo(RootComponent);
	WeaponPlacementComponent02->AttachTo(RootComponent);
	WeaponPlacementComponent03->AttachTo(RootComponent);
	WeaponPlacementComponent04->AttachTo(RootComponent);

	WeakPointPlacementComponent01 = CreateDefaultSubobject<USceneComponent>(TEXT("WeakPointPlacement01"));
	WeakPointPlacementComponent02 = CreateDefaultSubobject<USceneComponent>(TEXT("WeakPointPlacement02"));
	WeakPointPlacementComponent03 = CreateDefaultSubobject<USceneComponent>(TEXT("WeakPointPlacement03"));

	WeakPointPlacementComponent01->AttachTo(RootComponent);
	WeakPointPlacementComponent02->AttachTo(RootComponent);
	WeakPointPlacementComponent03->AttachTo(RootComponent);

	TurretMoveTarget01 = CreateDefaultSubobject<USceneComponent>(TEXT("TurretMoveTarget01"));
	TurretMoveTarget02 = CreateDefaultSubobject<USceneComponent>(TEXT("TurretMoveTarget02"));
	TurretMoveTarget03 = CreateDefaultSubobject<USceneComponent>(TEXT("TurretMoveTarget03"));

	TurretMoveTarget01->AttachTo(RootComponent);
	TurretMoveTarget02->AttachTo(RootComponent);
	TurretMoveTarget03->AttachTo(RootComponent);

	WeaponPlacements.Add(WeaponPlacementComponent01);
	WeaponPlacements.Add(WeaponPlacementComponent02);
	WeaponPlacements.Add(WeaponPlacementComponent03);
	WeaponPlacements.Add(WeaponPlacementComponent04);

	WeakPointPlacements.Add(WeakPointPlacementComponent01);
	WeakPointPlacements.Add(WeakPointPlacementComponent02);
	WeakPointPlacements.Add(WeakPointPlacementComponent03);

	TurretTargets.Add(TurretMoveTarget01);
	TurretTargets.Add(TurretMoveTarget02);
	TurretTargets.Add(TurretMoveTarget03);

	SceneComponents.Append(WeaponPlacements);
	SceneComponents.Append(WeakPointPlacements);
	SceneComponents.Append(TurretTargets);

	AttackPositions.Add(-30.0f);
	AttackPositions.Add(-15.0f);
	AttackPositions.Add(0.0f);
	AttackPositions.Add(15.0f);
	AttackPositions.Add(30.0f);

	NeutralDuration = 20.0f;
	NeutralProgress = 0.0f;
	bNeutralInProgress = false;
	bUseNeutralMovement = false;

	NeutralPositions.Add(-65.0f);
	NeutralPositions.Add(65.0f);

	static ConstructorHelpers::FObjectFinder<UTubeBossAttackPattern> AttackPatternObject(TEXT("/Game/Blueprints/Bots/Bosses/Tube/BP_TubeAttackPattern"));
	AttackPattern = AttackPatternObject.Object;
	AttackPatternIndex = FIntPoint(0, 0);
	APTrackingIndex = FIntPoint(0, 0);

	static ConstructorHelpers::FObjectFinder<UDataTable> TubeDataTableObject(TEXT("/Game/Blueprints/Attributes/Stats_BotHealth"));
	TubeDataTable = TubeDataTableObject.Object;

	TargetSpread = 10.0f;
	BaseSpread = 45.0f;
	Radius = 200.0f;

	TurretDuration = 10.0f;
	TurretProgress = 0.0f;

	MoveDuration = 10.0f;
	MoveProgress = 0.0f;
	TurretMovementDelay = 5.0f;

	bTurretsMoving = false;

	bEncounterStarted = false;

	MaxHealth = 1000;
	Health = 0;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called every frame
void ATubeBoss::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (bDead || !bEncounterStarted)
	{
		return;
	}

	if (bTurretsMoving)
	{
		MoveTurrets(DeltaTime);
	}
	else if (bNeutralInProgress)
	{
		NeutralMovement(DeltaTime);
	}
	else
	{
		MoveToTurret(DeltaTime);
	}
}

void ATubeBoss::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	FStatDataRow* TubeData = TubeDataTable->FindRow<FStatDataRow>(TubeDataRow, TubeDataTableContext, true);
	if (TubeData)
	{
		MaxHealth = TubeData->Health;
	}
	
	InitializeTurrets();
	InitializeWeakPoints();

	SetHealth(MaxHealth);
}

void ATubeBoss::StartEncounter()
{
	bEncounterStarted = true;

	SetActorRotation(FRotator(0.0f, YawRotation, 0.0f));

	AttackPatternIndex.X = AttackPatternIndexes[APTrackingIndex.X].PatternIndexes[APTrackingIndex.Y];
	SetNextAttackMove();
}

void ATubeBoss::InitializePlacement()
{
	FRotator Start = GetActorForwardVector().Rotation();
	FRotator Spread = FRotator(0.0f, 0.0f, BaseSpread * 2);

	SetLocationAndRotation(WeaponPlacements, Start, Spread);

	AdjustTurretTargets();

	Start += (Spread * 0.5f);
	SetLocationAndRotation(WeakPointPlacements, Start, Spread);
}

void ATubeBoss::SetLocationAndRotation(TArray<USceneComponent*>& Comp, FRotator Start, FRotator Spread)
{
	for (int32 c = 0; c < Comp.Num(); c++)
	{
		if (Comp[c] == nullptr)
		{
			Comp.RemoveAt(c);
			c--;
			continue;
		}
		Comp[c]->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
		FRotator Next = Start + (Spread * c);
		Comp[c]->SetRelativeRotation(Next);

		FVector Up = Comp[c]->GetUpVector();
		FVector RLocation = Comp[c]->GetRelativeTransform().GetLocation();
		FVector Location = RLocation + (Up * Radius);
		Comp[c]->SetRelativeLocation(Location);
	}
}

void ATubeBoss::AdjustTurretTargets()
{
	for (int32 c = 0; c < TurretTargets.Num(); c++)
	{
		TurretTargets[c]->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);

		FTransform Trans = WeaponPlacements[c]->GetRelativeTransform();
		FRotator Rotation = Trans.GetRotation().Rotator();
		Rotation += FRotator(0.0f, 0.0f, TargetSpread);
		TurretTargets[c]->SetRelativeRotation(Rotation);

		FVector Up = TurretTargets[c]->GetUpVector();
		FVector RLocation = TurretTargets[c]->GetRelativeTransform().GetLocation();
		FVector Location = RLocation + (Up * Radius);
		TurretTargets[c]->SetRelativeLocation(Location);
	}
}

void ATubeBoss::InitializeTurrets()
{
	for (int32 c = 0; c < Turrets.Num(); c++)
	{
		Turrets[c]->SetBotOwner(this, c);
		Turrets[c]->SpawnWeapon();
		
		if (TurretsAttackType.IsValidIndex(c))
		{
			Turrets[c]->SetAttackTypeDataAsset(TurretsAttackType[c]);
		}
	}
}

void ATubeBoss::InitializeWeakPoints()
{
	for (int32 c = 0; c < WeakPoints.Num(); c++)
	{
		WeakPoints[c]->OnDestroyed.BindUObject(this, &ATubeBoss::StartMovement);

		if (WeakPointsAttackType.IsValidIndex(c))
		{
			WeakPoints[c]->SetAttackType(WeakPointsAttackType[c]);
		}
	}
}

void ATubeBoss::StartMovement(AWeakPoint* WeakPoint)
{
	MovementIndex = WeakPoints.Find(WeakPoint) + 1;

	if (TurretTargets.IsValidIndex(MovementIndex - 1))
	{
		InitialRotation.Reset();
		TargetRotation.Reset();

		bTurretsMoving = true;

		InitialRotation.Add(WeaponPlacements[MovementIndex]->GetComponentRotation());
		TargetRotation.Add(TurretTargets[MovementIndex - 1]->GetComponentRotation());

		//Get the Difference between initial and target
		FRotator Diff = InitialRotation[0] - TargetRotation[0];

		for (int32 c = MovementIndex + 1; c < WeaponPlacements.Num(); c++)
		{
			FRotator Last = WeaponPlacements[c]->GetComponentRotation();
			InitialRotation.Add(Last);
			TargetRotation.Add(Last - Diff);
		}

		for (int32 c = MovementIndex - 1; c < WeakPointPlacements.Num(); c++)
		{
			FRotator Last = WeakPointPlacements[c]->GetComponentRotation();
			InitialRotation.Add(Last);
			TargetRotation.Add(Last - Diff);
		}
	}

	/** Track if all the Weak points have been destroyed */
	bool bDone = true;
	for (int32 c = 0; c < WeakPoints.Num(); c++)
	{
		if (!WeakPoints[c]->IsDead())
		{
			bDone = false;
			break;
		}
	}

	//All the Weakpoints have been destroyed, Shift the final turret into phase 2
	if (bDone)
	{
		Turrets[0]->SetState(ETubeTurretState::Phase02);
	}

	SetNextAttackMove();
}

void ATubeBoss::MoveTurrets(float DeltaTime)
{
	if (!bTurretsMoving)
	{
		return;
	}

	TurretProgress += DeltaTime;
	if (TurretProgress > TurretDuration)
	{
		TurretProgress = 0;
		bTurretsMoving = false;
		AdjustTurretTargets();
		Turrets[MovementIndex]->SetState(ETubeTurretState::Phase02);

		SetNextAttackMove();

		return;
	}

	//Move the Turret Actors
	int32 Index = 0;
	for (int32 c = MovementIndex; c < WeaponPlacements.Num(); c++, Index++)
	{
		MoveSceneComponent(WeaponPlacements[c], Turrets[c], Index);
	}

	// Move the Weak Point Actors
	for (int32 c = MovementIndex - 1; c < WeakPointPlacements.Num(); c++, Index++)
	{
		MoveSceneComponent(WeakPointPlacements[c], WeakPoints[c], Index);
	}
}

void ATubeBoss::MoveSceneComponent(USceneComponent* MyComponent, AActor* MyActor, int32 Index)
{
	float Blend = TurretProgress / TurretDuration;
	FQuat RLerp = FMath::Lerp(InitialRotation[Index].Quaternion(), TargetRotation[Index].Quaternion(), Blend);

	MyComponent->SetRelativeLocation(FVector::ZeroVector);
	MyComponent->SetWorldRotation(RLerp);

	FVector Up = MyComponent->GetUpVector();
	FVector RLocation = MyComponent->GetRelativeTransform().GetLocation();
	FVector Location = RLocation + (Up * Radius);
	MyComponent->SetRelativeLocation(Location);

	FTransform T;
	T.SetLocation(GetActorLocation() + Location);
	T.SetRotation(RLerp);
	MyActor->SetActorTransform(T);
}

void ATubeBoss::SetNextAttackMove()
{
	//Reset the initial and target, as they might be different from when they started
	FAttackPattern Pattern;
	FAttackMove Attack;

	AttackPattern->GetPatternAndAttackWithPoint(AttackPatternIndex, Pattern, Attack);
	NextSegment(Attack.TurretIndex, Attack.PositionIndex, Pattern.MoveDuration, Pattern.MoveDelay);
}

void ATubeBoss::NextSegment(int32 TurretIndex, int32 PositionIndex, float InMoveDuration, float MoveDelay)
{
	if (Turrets.IsValidIndex(TurretIndex) && AttackPositions.IsValidIndex(PositionIndex))
	{
		InitialActorRotation = GetActorRotation();
		TargetActorRotation = GetActorRotation() + (Turrets[TurretIndex]->GetActorRotation() * -1) + FRotator(0.0f, YawRotation, AttackPositions[PositionIndex]);

		if (InMoveDuration > 0)
		{
			MoveDuration = InMoveDuration;
		}

		if (MoveDelay > 0)
		{
			TurretMovementDelay = MoveDelay;
		}
	}
}

void ATubeBoss::MoveToTurret(float DeltaTime)
{
	if (bAtTurret)
	{
		return;
	}

	MoveProgress += DeltaTime;
	if (MoveProgress > MoveDuration)
	{
		bAtTurret = true;
		MoveProgress = 0.0f;

		if (TubeTrigger)
		{
			TubeTrigger->FireTurretsInOverlap();
		}

		GetWorldTimerManager().ClearTimer(ActorMove_TimerHandle);
		GetWorldTimerManager().SetTimer(ActorMove_TimerHandle, this, &ATubeBoss::SetNextSegment, TurretMovementDelay, false);
	}
	else
	{
		float Blend = MoveProgress / MoveDuration;
		FQuat RLerp = FMath::Lerp(InitialActorRotation.Quaternion(), TargetActorRotation.Quaternion(), Blend);
		SetActorRotation(RLerp);
	}
}

void ATubeBoss::SetNextSegment()
{
	bAtTurret = false;
	
	const int32 PatternLength = AttackPatternIndexes[APTrackingIndex.X].GetLength();
	const int32 AttackLength = AttackPattern->GetAttackLength(AttackPatternIndex.X);
	
	//Increase the AttackMove index
	AttackPatternIndex.Y++;
	if (AttackPatternIndex.Y >= AttackLength)
	{
		//Increase the Attack Pattern Tracking Index 
		APTrackingIndex.Y++;
		if (APTrackingIndex.Y >= PatternLength)
		{
			//Reset the Attack Pattern Index
			APTrackingIndex.Y = 0;
			//Move into the next pattern
			APTrackingIndex.X++;
			if (APTrackingIndex.X >= AttackPatternIndexes.Num())
			{
				//Reset back to the base pattern 
				APTrackingIndex.X = 0;
			}

			//If bUseNeutralMovement is true, Start the Neutral movement, time for the player to shoot the weak points
			//Else Transition to the Next Attack Pattern
			bUseNeutralMovement ? StartNeutralMovement() : FinishNeutralMovement(true);
		}		

		//Set the Next Attack Pattern
		AttackPatternIndex.X = AttackPatternIndexes[APTrackingIndex.X].PatternIndexes[APTrackingIndex.Y];
		//Reset the Attack Move Index 
		AttackPatternIndex.Y = 0;
	}

	//Set the next segment
	SetNextAttackMove();
}

void ATubeBoss::StartNeutralMovement(int32 Index)
{
	bNeutralInProgress = true;
	NeutralRotation = GetActorRotation();
	NeutralTargetRotation = FRotator(0.0f, YawRotation, NeutralPositions[Index]);
}

void ATubeBoss::NeutralMovement(float DeltaTime)
{
	if (!bNeutralInProgress)
	{
		return;
	}

	NeutralProgress += DeltaTime;
	NeutralProgressTotal += DeltaTime;
	if (NeutralProgressTotal > NeutralDuration)
	{
		FinishNeutralMovement();
	}
	else if (NeutralProgress > NeutralDuration / 2.0f)
	{
		//Reset the progress to ensure the next move.
		NeutralProgress = 0;
		//Set the next neutral position 
		StartNeutralMovement(1);
	}
	else
	{
		float Blend = NeutralProgress / (NeutralDuration / 2.0f);
		FQuat RLerp = FMath::Lerp(NeutralRotation.Quaternion(), NeutralTargetRotation.Quaternion(), Blend);
		SetActorRotation(RLerp);
	}
}

void ATubeBoss::FinishNeutralMovement(bool bSkipAssignSegment)
{
	NeutralProgress = 0;
	NeutralProgressTotal = 0;
	bNeutralInProgress = false;

	if (bSkipAssignSegment)
	{
		return;
	}

	SetNextAttackMove();
}

void ATubeBoss::SetHealth(int32 InHealth)
{
	Health = InHealth;

	for (int32 c = 0; c < Turrets.Num(); c++)
	{
		Turrets[c]->SetHealth(InHealth / 4);
	}
}

int32 ATubeBoss::GetHealth() const
{
	return Health;
}

int32 ATubeBoss::GetMaxHealth() const
{
	return MaxHealth;
}

float ATubeBoss::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Damage <= 0 || bDead)
	{
		return 0.0f;
	}

	ReduceHealth(Damage, DamageCauser);

	return 0.0f;
}

bool ATubeBoss::IsDead() const
{
	return bDead;
}

void ATubeBoss::ReduceHealth(int32 InDamage, AActor* DamageCauser)
{
	if (bDead || InDamage <= 0)
	{
		return;
	}

	Health -= InDamage;
	if (Health <= 0)
	{
		Dead();
	}
}

void ATubeBoss::Dead()
{
	if (Health <= 0)
	{
		bDead = true;

		HideAndDestroy();
	}
}

void ATubeBoss::HideAndDestroy()
{
	if (!bDead)
	{
		return;
	}

	for (int32 c = 0; c < Turrets.Num(); c++)
	{
		Turrets[c]->CleanDestroy();
	}

	for (int32 c = 0; c < WeakPoints.Num(); c++)
	{
		WeakPoints[c]->HideAndDestroy();
	}

	SetLifeSpan(10.0f);
}