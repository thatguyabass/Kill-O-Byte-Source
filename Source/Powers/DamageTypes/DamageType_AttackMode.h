// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Utility/Utility.h"
#include "GameFramework/DamageType.h"
#include "DamageType_AttackMode.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE_PROJECT_API UDamageType_AttackMode : public UDamageType
{
	GENERATED_BODY()
	
public:
	UDamageType_AttackMode(const class FObjectInitializer& PCIP);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageType")
	EAttackMode AttackMode;

};
