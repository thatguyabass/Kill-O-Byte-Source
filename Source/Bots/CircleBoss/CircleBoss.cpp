// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "CircleBoss.h"
#include "CircleBossAI.h"
#include "Components/MoveToLocation.h"
#include "Bots/Bot/Component/WeakPoint.h"
#include "Bots/CircleBoss/CircleBossWall.h"
#include "Bots/CircleBoss/CircleBossArm.h"
#include "Utility/AttackTypeDataAsset.h"
#include "Powers/AttackHelper.h"
#include "Bots/SingleBotSpawner.h"
#include "Powers/Shield/ForceField.h"

const int32 ACircleBoss::WallCount = 2;

ACircleBoss::ACircleBoss()
	: Super()
{
	WallWeakPointPlacement = CreateDefaultSubobject<USceneComponent>(TEXT("WallWeakPointPlacement"));
	WallWeakPointPlacement->AttachTo(RootComponent);

	BossWeakPointPlacement = CreateDefaultSubobject<USceneComponent>(TEXT("BossWeakPointPlacement"));
	BossWeakPointPlacement->AttachTo(RootComponent);

	ArmPlacement01 = CreateDefaultSubobject<USceneComponent>(TEXT("CircleBossArmPlacement01"));
	ArmPlacement01->AttachTo(RootComponent);

	ArmPlacement02 = CreateDefaultSubobject<USceneComponent>(TEXT("CircleBossArmPlacement02"));
	ArmPlacement02->AttachTo(RootComponent);

	WallCrushSFXComp = CreateDefaultSubobject<UAudioComponent>(TEXT("WallCrushSFX"));
	WallCrushSFXComp->bAutoActivate = false;
	WallCrushSFXComp->bAlwaysPlay = false;
	WallCrushSFXComp->AttachTo(RootComponent);

	SplinePointCount = 8;
	PointDistance = 2000.0f;
	HeightOffset = 0.0f;

	ExplosionForce = 2000.0f;
	ExplosionRadius = 1500.0f;

	/** This bot works on a hit system over a total health point system */
	BaseHealth = 3;

	BotsSpawned = 0;

	BotCounts.Add(8);
	BotCounts.Add(12);
	BotCounts.Add(16);

	WaveCount = 0;
	HealthScale = 0.2f;
	ArmDamage = 150;

	bArmPhaseInprogress = false;

	CircleBossArmRowName = "CircleBossArm";

	SplineComponent01 = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent01"));
	SplineComponent01->AttachTo(RootComponent);

	SplineComponent02 = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent02"));
	SplineComponent02->AttachTo(RootComponent);

	LazerBoxExtents = 1750.0f;
	LazerPosition = 2000.0f;
	LazerDamage = 20;

	ShieldDelay = 1.0f;
}

void ACircleBoss::StartEncounter()
{
	ACircleBossAI* AI = Cast<ACircleBossAI>(Controller);
	if (AI)
	{
		AI->StartEncounter();
	}
}

void ACircleBoss::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PlaceSplinePoints(SplineComponent01, 0);
	PlaceSplinePoints(SplineComponent02, 1);

	SpawnWalls();

	InitializeWeakPoints();

	InitializeArms();

	FStatDataRow* StatData = StatDataTable->FindRow<FStatDataRow>(CircleBossArmRowName, DataTableContext, false);
	if (StatData)
	{
		ArmHealth = StatData->Health;
		ResetArmHealth();
	}
}

void ACircleBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ACircleBossAI* AI = Cast<ACircleBossAI>(Controller);
	if (AI && AI->GetEncounterStarted())
	{
		TickBotMethods();
	}
}

float ACircleBoss::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bDead)
	{
		return 0.0f;
	}

	AWeakPoint* Weak = Cast<AWeakPoint>(DamageCauser);
	if (Weak)
	{
		WaveCount++;
		ResetArmHealth();

		Damage = 1;
		//Set the Damage Override to deal the true Damage to the boss 
		bForceDamageOverride = true;
		Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	}

	return 0.0f;
}

void ACircleBoss::HideAndDestroy()
{
	if (!bDead)
	{
		bDead = true;

		for (int32 c = 0; c < WeakPoints.Num(); c++)
		{
			WeakPoints[c]->HideAndDestroy();
		}

		if (ForceField)
		{
			ForceField->HideAndDestroy(false);
		}

		for (int32 c = 0; c < Bots.Num(); c++)
		{
			if (Bots[c] && !Bots[c]->IsDead())
			{
				FDamageEvent Event = AttackHelper::CreateDamageEvent(nullptr, FAttackType(EAttackMode::Attack05_White));
				Bots[c]->TakeDamage(100000000, Event, nullptr, nullptr);
			}
		}
		Bots.Empty();

		GetWorldTimerManager().ClearTimer(Shield_TimerHandle);

		GetController() ? Cast<ABaseAIController>(GetController())->StopAI() : nullptr;
	}
}

void ACircleBoss::PlaceSplinePoints(USplineComponent* SplineComp, int32 Direction)
{
	int32 Dir = Direction % 2 == 0 ? 1 : -1;
	FRotator Spread = FRotator(0.0f, (180 / SplinePointCount) * Dir, 0.0f);
	FRotator MyRotation = GetActorRotation();
	FVector Start = GetActorForwardVector() * -1;

	SplineComp->ClearSplinePoints();

	//Add two points to over shoot the 180 target. This creates a much smoother circle.
	for (int32 c = 0; c < SplinePointCount + 2; c++)
	{
		FRotator R = MyRotation + (Spread * c);
		FVector Location = R.RotateVector(Start);
		Location *= PointDistance;
		SplineComp->AddSplineLocalPoint(Location);
	}

	SplineComp->Duration = 1.1f;
}

void ACircleBoss::SpawnWalls()
{
	FVector Location = SplineComponent01->GetWorldLocationAtSplinePoint(0);
	FRotator Rotation = SplineComponent01->GetWorldRotationAtDistanceAlongSpline(0.0f);
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (int32 c = 0; c < WallCount; c++)
	{
		ACircleBossWall* Wall = GetWorld()->SpawnActor<ACircleBossWall>(WallActorClass, Location, Rotation, Params);
		if (Wall)
		{
			ACircleBossAI* AI = Cast<ACircleBossAI>(Controller);
			if (AI)
			{
				Wall->SetWallOwner(AI);
			}

			UMoveToLocation* MoveTo = Wall->MoveToComponent;
			if (MoveTo)
			{
				MoveTo->MoveToImmediately(DownIndex);
				MoveToComps.Add(MoveTo);
			}

			WallActors.Add(Wall);
		}
	}
}

void ACircleBoss::InitializeWeakPoints()
{
	if (WeakPoints.Num() <= 0)
	{
		return;
	}

	int32 Index = 0;
	WallWeakPoint = WeakPoints.IsValidIndex(Index) ? WeakPoints[Index] : nullptr;
	if (WallWeakPoint)
	{
		WallWeakPoint->SetDeathFlag(true);
		WallWeakPoint->RemoveCollision();
		WallWeakPoint->SetActorHiddenInGame(true);
		//WallWeakPoint->Dead();
	}

	++Index;
	BossWeakPoint = WeakPoints.IsValidIndex(Index) ? WeakPoints[Index] : nullptr;
	if (BossWeakPoint)
	{
		BossWeakPoint->SetWeakPointOwner(this);
		BossWeakPoint->OnWeakPointTakeDamage.BindUObject(this, &ACircleBoss::EndArmPhase);
		RemoveBossWeakPointCollision();
	}
}

void ACircleBoss::ResetWallWeakPoint()
{
	if (!WallWeakPoint || AttackTypes.Num() <= 0)
	{
		return;
	}

	OnWallWeakPointReset();

	int32 Rand = FMath::RandRange(0, AttackTypes.Num() - 1);
	WallWeakPoint->SetAttackType(AttackTypes[Rand]);

	WallWeakPoint->ResetWeakPoint();
}

void ACircleBoss::HideWallWeakPoint()
{
	WallWeakPoint->Dead();
}

void ACircleBoss::RemoveBossWeakPointCollision()
{
	if (BossWeakPoint)
	{
		//Remove the collision, but keep the weakpoint visible 
		BossWeakPoint->RemoveCollision();

		OnRemoveBossWeakPointCollision();
	}

	GetWorldTimerManager().SetTimer(Shield_TimerHandle, this, &ACircleBoss::ActivateShield, ShieldDelay, false);
}

void ACircleBoss::EnableBossWeakPointCollision()
{
	if (BossWeakPoint)
	{
		BossWeakPoint->Show();
	}

	if (ForceField)
	{
		ForceField->DeactivateForceField();
	}
}

bool ACircleBoss::IsWallWeakPointDead() const
{
	return WallWeakPoint ? WallWeakPoint->IsDead() : false;
}

float ACircleBoss::GetTravelTime() const
{
	if (MoveToComps.Num() > 0)
	{
		return MoveToComps[0]->TravelTime;
	}

	return 0.0f;
}

void ACircleBoss::ResetWalls()
{
	FVector Location = SplineComponent01->GetWorldLocationAtSplinePoint(0);
	FRotator Rotation = SplineComponent01->GetWorldRotationAtDistanceAlongSpline(0.0f);

	for (int32 c = 0; c < WallActors.Num(); c++)
	{
		WallActors[c]->SetActorLocationAndRotation(Location, Rotation, false);
		MoveToComps[c]->MoveWaypoints(WallActors[c]->GetActorLocation());
		MoveToComps[c]->MoveToImmediately(DownIndex);
	}
}

void ACircleBoss::MoveWallsUp()
{
	for (int32 c = 0; c < MoveToComps.Num(); c++)
	{
		WallActors[c]->Show();
		MoveToComps[c]->SetNextWaypoint(UpIndex);
	}
}

void ACircleBoss::MoveWallsDown()
{
	float TravelTime = 0.0f;

	for (int32 c = 0; c < MoveToComps.Num(); c++)
	{
		TravelTime = MoveToComps[c]->TravelTime;
		MoveToComps[c]->MoveWaypoints(WallActors[c]->GetActorLocation());
		MoveToComps[c]->SetNextWaypoint(DownIndex);
	}

	GetWorldTimerManager().ClearTimer(HideWalls_TimerHandle);
	GetWorldTimerManager().SetTimer(HideWalls_TimerHandle, this, &ACircleBoss::HideWalls, TravelTime + 0.1f, false);
}

void ACircleBoss::MoveWallsAlongSpline(float Progress)
{
	if (WallActors.Num() > 0)
	{
		MoveWallAlongSpline(Progress, WallActors[0], SplineComponent01);
		MoveWallAlongSpline(Progress, WallActors[1], SplineComponent02);
	}
}

void ACircleBoss::MoveWallAlongSpline(float Progress, AActor* Wall, USplineComponent* SplineComp)
{
	const bool bConstantVelocity = true;
	FVector Location = SplineComp->GetWorldLocationAtTime(Progress, bConstantVelocity);
	FRotator Rotation = SplineComp->GetWorldRotationAtTime(Progress, bConstantVelocity);

	Location += FVector(0.0f, 0.0f, HeightOffset);

	FVector MyLocation = Wall->GetActorLocation();
	FVector Delta = Location - MyLocation;

	FHitResult Hit(ForceInit);
	Wall->SetActorLocationAndRotation(Location, Rotation, true, &Hit);
	if (Hit.GetActor())
	{
		Hit.GetActor()->AddActorWorldOffset(Delta);
		Wall->SetActorLocationAndRotation(Location, Rotation);
	}
}

float ACircleBoss::GetSplineDistance() const
{
	return SplineComponent01->GetSplineLength();
}

void ACircleBoss::HideWalls()
{
	for (int32 c = 0; c < WallActors.Num(); c++)
	{
		WallActors[c]->Hide();
	}
}

int32 ACircleBoss::GetArmHealth()
{
	return ArmHealth + FMath::CeilToInt(((float)ArmHealth * HealthScale) * (float)WaveCount);
}

void ACircleBoss::ReduceArmHealth()
{
	CurrentArmHealth -= ArmDamage;
	if (CurrentArmHealth <= 0)
	{
		if (!bArmPhaseInprogress)
		{
			TriggerArmPhase();
		}
	}
}

void ACircleBoss::ResetArmHealth()
{
	CurrentArmHealth = GetArmHealth();
}

void ACircleBoss::AddArm(ACircleBossArm* InArm)
{
	if (InArm)
	{
		Arms.Add(InArm);
	}
}

void ACircleBoss::ClearArms()
{
	Arms.Empty();
}

void ACircleBoss::InitializeArms()
{
	for (int32 c = 0; c < Arms.Num(); c++)
	{
		if (Arms[c])
		{
			Arms[c]->OnSequenceFinish.BindUObject(this, &ACircleBoss::OnArmSequenceFinished);
		}
	}
}

void ACircleBoss::StartForwardArmSequence()
{
	for (int32 c = 0; c < Arms.Num(); c++)
	{
		Arms[c]->StartForwardSequence();
	}
}

void ACircleBoss::StartReverseArmSequence()
{
	for (int32 c = 0; c < Arms.Num(); c++)
	{
		Arms[c]->StartReverseSequence();
	}
}

void ACircleBoss::OnArmSequenceFinished()
{
	ACircleBossAI* AI = Cast<ACircleBossAI>(Controller);
	if (AI)
	{
		AI->SetArmsInPosition();
	}
}

void ACircleBoss::EndArmPhase()
{
	ACircleBossAI* AI = Cast<ACircleBossAI>(Controller);
	if(AI)
	{
		if (bDead)
		{
			return;
		}

		bArmPhaseInprogress = false;

		TArray<FHitResult> Hit;
		FVector Start = GetActorLocation();
		FVector End = GetActorLocation() + (GetActorUpVector() * 500.0f);

		FCollisionQueryParams Params("SphereCheck", false, false);
		FCollisionObjectQueryParams ObjParams;
		ObjParams.AddObjectTypesToQuery(ECC_Pawn);

		GetWorld()->SweepMultiByObjectType(Hit, Start, End, FQuat::Identity, ObjParams, FCollisionShape::MakeSphere(ExplosionRadius), Params);

		for (int32 c = 0; c < Hit.Num(); c++)
		{
			ACharacter* MyCharacter = Cast<ACharacter>(Hit[c].GetActor());
			if (MyCharacter && MyCharacter != this)
			{
				FVector Direction = MyCharacter->GetActorLocation() - Start;
				Direction.Normalize();

				MyCharacter->LaunchCharacter(Direction * ExplosionForce, true, true);
			}
		}

		AI->ResetLazer();
		RemoveBossWeakPointCollision();

		for (int32 c = 0; c < Bots.Num(); c++)
		{
			FDamageEvent Event = AttackHelper::CreateDamageEvent(nullptr, FAttackType(EAttackMode::Attack05_White));
			Bots[c]->TakeDamage(15000, Event, nullptr, nullptr);
		}
		Bots.Empty();
	}
}

bool ACircleBoss::GetArmPhaseInprogress()
{
	return bArmPhaseInprogress;
}

void ACircleBoss::TriggerArmPhase()
{
	ACircleBossAI* AI = Cast<ACircleBossAI>(Controller);
	if (AI)
	{
		bArmPhaseInprogress = true;
		AI->TriggerArmPhase();
	}
}

void ACircleBoss::FireLazer()
{
	//TArray<FOverlapResult> Hits;
	//FVector Location = GetActorLocation() + (GetActorForwardVector() * LazerPosition);

	//FCollisionQueryParams Params("LazerBoxTrace");
	//FCollisionObjectQueryParams ObjParams;
	//ObjParams.AddObjectTypesToQuery(ECC_Pawn);

	//FQuat Rotation = FQuat(FRotator(0.0f, 45.0f, 0.0f));

	//GetWorld()->OverlapMultiByObjectType(Hits, Location, Rotation, ObjParams, FCollisionShape::MakeBox(FVector(LazerBoxExtents)), Params);
	//if (bDrawOverlapSweep)
	//{
	//	DrawDebugBox(GetWorld(), Location, FVector(LazerBoxExtents), Rotation, FColor::Red, true, 5.0f);
	//}

	TArray<AActor*> Targets;
	for (TActorIterator<ACharacter> Character(GetWorld()); Character; ++Character)
	{
		bool bAddTarget = false;

		ASnakeLinkHead* Head = Cast<ASnakeLinkHead>(*Character);
		if (Head && !Head->IsDead())
		{
			bAddTarget = true;
		}

		ABot* Bot = Cast<ABot>(*Character);
		if (Bot && !Bot->IsDead() && Bot != this)
		{
			bAddTarget = true;
		}

		if (bAddTarget)
		{
			Targets.Add(*Character);
		}
	}
	
	for (int32 c = 0; c < Targets.Num(); c++)
	{
		FDamageEvent Event = AttackHelper::CreateDamageEvent(nullptr, FAttackType(EAttackMode::Attack05_White));
		Targets[c]->TakeDamage(LazerDamage, Event, nullptr, this);
	}
}

void ACircleBoss::AddBotSpawner(ASingleBotSpawner* InSpawner)
{
	if (!InSpawner)
	{
		return;
	}

	InSpawner->AttachRootComponentToActor(this, NAME_None, EAttachLocation::KeepWorldPosition, true);
	BotSpawners.Add(InSpawner);
}

void ACircleBoss::SpawnBots()
{
	ACircleBossAI* AI = Cast<ACircleBossAI>(Controller);
	if (AI && AI->CanSpawnBots() && CanSpawnBots())
	{
		int32 Rand = FMath::RandRange(0, BotSpawners.Num() - 1);

		ASingleBotSpawner* Spawner = BotSpawners[Rand];
		Spawner->SoftReset();
		Spawner->StartSpawning();

		ABot* NewBot = Spawner->GetLatestBot();
		if (NewBot)
		{
			Bots.Add(Spawner->GetLatestBot());
			BotsSpawned++;
		}
	}
}

void ACircleBoss::VerifyBotStatus()
{
	for (int32 c = 0; c < Bots.Num(); c++)
	{
		if (Bots[c]->IsDead())
		{
			ReduceArmHealth();
			Bots.RemoveAt(c);
			c--;
		}
	}

	if (Bots.Num() <= 0)
	{
		BotsSpawned = 0;
	}
}

void ACircleBoss::TickBotMethods()
{
	if (bDead)
	{
		return;
	}

	SpawnBots();
	VerifyBotStatus();
}

bool ACircleBoss::CanSpawnBots()
{
	return (BotsSpawned < BotCounts[WaveCount]) ? true : false;
}

void ACircleBoss::Dead(ASnakePlayerState* Killer)
{
	Super::Dead(Killer);

	OnResetLazer();

	for (int32 c = 0; c < Arms.Num(); c++)
	{
		Arms[c]->BossDead();
	}

	ACircleBossAI* AI = Cast<ACircleBossAI>(Controller);
	if (Controller)
	{
		AI->ClearTimers();
	}
}

void ACircleBoss::AddForceField(AForceField* InForceField)
{
	ForceField = InForceField;
}

void ACircleBoss::ActivateShield()
{
	if (ForceField)
	{
		ForceField->ActivateForceField();
	}
}

void ACircleBoss::PlayWallCrushSFX()
{
	WallCrushSFXComp->Play();
}