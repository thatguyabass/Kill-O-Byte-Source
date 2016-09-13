// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BaseAIController.generated.h"

class UBlackboardComponent;
class UBehaviorTreeComponent;

UCLASS(config = Game)
class SNAKE_PROJECT_API ABaseAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ABaseAIController(const class FObjectInitializer& PCIP);
	
	UPROPERTY(Transient)
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(Transient)
	UBehaviorTreeComponent* TreeComponent;

	virtual void Possess(APawn* InPawn);
	/** Stop the Behavior tree, movement and remove the target */
	void StopAI();
	virtual void GameHasEnded(AActor* EndGameFocus = nullptr, bool bIsWinner = false) override;

	/** Initialize Blackboard Keys */
	virtual void InitializeBlackboardKeys();

	/** Search the area using a sphere overlap to find a target */
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	virtual void FindTarget();

	/** Turn the Pawn actor to face the Target */
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	void FaceTarget(const float RotationRate);

	/** Turn the Pawn Actor to face the Provided Vector */
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	void FaceLocation(const FVector Location, const float RotationRate);

	UPROPERTY(EditAnywhere, Category = "Behavior")
	bool bConstraintZ;

	/** Fire the Bots Weapon */
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	void FireWeapon();

	/** Fire Weapon upon seeing target */
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	void AttackOnSight(const bool bAutoRotate = true, const float RotationRate = 0.25f, const bool bUseLineOfSight = true);

	/** Line Trace from the Origin to the Target. If True, the Line Trace colleded with the Target */
	bool CheckLineOfSight(AActor* Origin, AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Behavior")
	bool CheckLineOfSightAdvanced(AActor* Origin, AActor* Target);

	FVector GetDestinationInRange(const float Minimum, const float Maximum);

	/** Simpler Getter for blackboard values */
	template<typename Get, typename BBType>
	Get* GetValue(int32 Key);

	/** Set the AI's target */
	virtual void SetTarget(AActor* InActor);

	UFUNCTION(BLueprintPure, Category = "Behavior")
	AActor* GetTarget();

	/** Set Destination */
	void SetDestination(FVector Destination);

	/** Create a Object Query. Can be overrriden for more flexability */
	virtual FCollisionObjectQueryParams GetObjectQuery();

	/** Event notification when the bot has died. Clean up any data that wouldn't be cleaned during garbage collection */
	virtual void BotDead(class ASnakePlayerState* Killer);

protected:
	int32 TargetKey;
	int32 DestinationKey;
	
public:
	/** Returns BlackboardComponent subobject **/
	FORCEINLINE UBlackboardComponent* GetBlackboardComponent() const { return BlackboardComponent; }

};
