// Fill out your copyright notice in the Description page of Project Settings.

#include "Snake_Project.h"
#include "BaseAIController.h"
#include "Bots/Bot/Bot.h"
#include "Bots/PointOfInterest.h"
#include "Powers/Weapons/Weapon.h"
#include "Powers/Shield/ForceField.h"
#include "SnakeCharacter/SnakeLink.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

#include "BotUtility.h"

ABaseAIController::ABaseAIController(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	BlackboardComponent = PCIP.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComponent"));
	TreeComponent = PCIP.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorTreeComponent"));

	bConstraintZ = false;
}

void ABaseAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	ABot* Bot = Cast<ABot>(InPawn);
	if (Bot && Bot->BotBehaviorTree)
	{
		BlackboardComponent->InitializeBlackboard(*(Bot->BotBehaviorTree->BlackboardAsset));

		InitializeBlackboardKeys();

		TreeComponent->StartTree(*Bot->BotBehaviorTree);
	}
}

void ABaseAIController::InitializeBlackboardKeys()
{
	TargetKey = BlackboardComponent->GetKeyID("Target");
	DestinationKey = BlackboardComponent->GetKeyID("Destination");
}

void ABaseAIController::StopAI()
{
	/** Stop the Behaviour tree */
	TreeComponent->StopTree();

	/** Stop all movement */
	StopMovement();

	/** Remove the Target */
	SetTarget(nullptr);
}

void ABaseAIController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	StopAI();
}

void ABaseAIController::FindTarget()
{
	//Override this with custom target selection
}

FVector ABaseAIController::GetDestinationInRange(const float Minimum, const float Maximum)
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return FVector::ZeroVector;
	}

	AActor* Target = GetValue<AActor, UBlackboardKeyType_Object>(TargetKey);
	if (Target)
	{
		FVector Location = FVector::ZeroVector;
		const FVector SearchOrigin = Target->GetActorLocation();

		do
		{
			Location = UNavigationSystem::GetRandomReachablePointInRadius(this, SearchOrigin, Maximum);
		} while (Location.Size() < Minimum);

		return Location;
	}

	return FVector::ZeroVector;
}

void ABaseAIController::AttackOnSight(const bool bAutoRotate, const float RotationRate, const bool bUseLineOfSight)
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return;
	}

	AActor* Target = GetValue<AActor, UBlackboardKeyType_Object>(TargetKey);
	if (Target)
	{
		bool bCanSeeTarget = true;
		if (bUseLineOfSight)
		{
			bCanSeeTarget = CheckLineOfSight(MyPawn, Target);
		}

		if (bCanSeeTarget)
		{
			if (bAutoRotate)
			{
				FaceTarget(RotationRate);
			}
			FireWeapon();
		}
	}
}

bool ABaseAIController::CheckLineOfSight(AActor* Origin, AActor* Target)
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return false;
	}

	const FVector Start = Origin->GetActorLocation();
	const FVector End = Target->GetActorLocation();

	FName Tag = "LineOfSight";
	FCollisionQueryParams Params(Tag, false, MyPawn);

	FHitResult Hit(ForceInit);
	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjParams.AddObjectTypesToQuery(ECC_Pawn);

	bool Result = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjParams, Params);

	AActor* Other = Hit.GetActor();
	AForceField* Field = Cast<AForceField>(Other);
	if (Field)
	{
		Other = Field->GetShieldOwner();
	}

	if (Other == Target)
	{
		return true;
	}

	return false;
}

bool ABaseAIController::CheckLineOfSightAdvanced(AActor* Origin, AActor* Target)
{
	bool bHasSight = CheckLineOfSight(Origin, Target);
	if (bHasSight)
	{
		ABot* Bot = Cast<ABot>(Origin);
		if (!Bot)
		{
			return bHasSight;
		}

		const FVector Start = Origin->GetActorLocation();
		const FVector Right = Origin->GetActorRightVector();
		const FVector End = Target->GetActorLocation();
		const float Radius = Bot->GetCapsuleComponent()->GetScaledCapsuleRadius();
		
		FName Tag = "LineOfSight";
		FCollisionQueryParams Params(Tag, false, GetPawn());

		FHitResult Hit(ForceInit);
		FCollisionObjectQueryParams ObjParams;
		ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjParams.AddObjectTypesToQuery(ECC_Pawn);

		for (int32 c = 0; c < 2; c++)
		{
			FVector _Right = Right;
			if (c % 2 == 0)
			{
				_Right *= -1;
			}
			FVector Offset = Start + (Radius * _Right);

			bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, Offset, End, ObjParams, Params);

			AActor* Other = Hit.GetActor();
			AForceField* Field = Cast<AForceField>(Other);
			if (Field)
			{
				Other = Field->GetShieldOwner();
			}

			if (Other != Target)
			{
				bHasSight = false;
				break;
			}
		}
	}

	return bHasSight;
}

void ABaseAIController::FaceTarget(const float RotationRate)
{
	AActor* Target = GetValue<AActor, UBlackboardKeyType_Object>(TargetKey);
	if (Target)
	{
		FaceLocation(Target->GetActorLocation(), RotationRate);
	}
}

void ABaseAIController::FaceLocation(const FVector Location, const float RotationRate)
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return;
	}

	//Get the Locations
	FVector MyLoc = MyPawn->GetActorLocation();

	//Find the direction the Target is from this actors location 
	FVector Direction = Location - MyLoc;
	//Normalize and apply the rotation rate. Rotation rate allows for a smooth rotation at low values while at higher values the turret will snap to target
	Direction.Normalize();

	//Add the currect direction to the rotated direction
	FVector CurrentDirection = MyPawn->GetActorForwardVector();

	//If the Target and this class are perfectly aligned, offset the direction to ensure the pawn rotates correctly. 
	FVector Temp = Direction * -1;
	float Mod = 0.05f;//Add a small offset to the floats due to float inaccuracy
	if ((Temp.X > CurrentDirection.X - Mod && Temp.X < CurrentDirection.X + Mod) &&
		(Temp.Y > CurrentDirection.Y - Mod && Temp.Y < CurrentDirection.Y + Mod) &&
		(Temp.Z > CurrentDirection.Z - Mod && Temp.Z < CurrentDirection.Z + Mod))
	{
		Direction.X += Mod;
	}
	
	Direction *= RotationRate;
	FVector FinalDirection = CurrentDirection + Direction;

	FRotator Rotation = FinalDirection.Rotation();
	//Prevent the Pawn from moving along the Z Axis
	if (bConstraintZ)
	{
		Rotation.Pitch = MyPawn->GetActorRotation().Pitch;
	}

	MyPawn->SetActorRotation(Rotation);
}

void ABaseAIController::FireWeapon()
{
	ABot* MyBot = Cast<ABot>(GetPawn());
	if (!MyBot || MyBot->IsDead())
	{
		return;
	}

	MyBot->FireWeapon();
}

void ABaseAIController::SetTarget(AActor* InActor)
{
	//Ensure the new target isn't already the same actor 
	AActor* Target = GetTarget();
	if (Target && Target == InActor)
	{
		return;
	}

	if (BlackboardComponent)
	{
		BlackboardComponent->SetValue<UBlackboardKeyType_Object>(TargetKey, InActor);
		SetFocus(InActor);
	}
}

AActor* ABaseAIController::GetTarget()
{
	return GetValue<AActor, UBlackboardKeyType_Object>(TargetKey);
}

void ABaseAIController::SetDestination(FVector Destination)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValue<UBlackboardKeyType_Vector>(DestinationKey, Destination);
	}
}

template<typename Get, typename BBType>
Get* ABaseAIController::GetValue(int32 Key)
{
	if (BlackboardComponent)
	{
		return Cast<Get>(BlackboardComponent->GetValue<BBType>(Key));
	}

	return nullptr;
}

FCollisionObjectQueryParams ABaseAIController::GetObjectQuery()
{
	return FCollisionObjectQueryParams();
}

void ABaseAIController::BotDead(ASnakePlayerState* Killer)
{
	//Override this method with anything that needs to be destroyed separately. 
}