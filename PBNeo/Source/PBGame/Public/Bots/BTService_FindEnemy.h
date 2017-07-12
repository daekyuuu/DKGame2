// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "BTService_FindEnemy.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, HideDropdown)
class PBGAME_API UBTService_FindEnemy : public UBTService_BlueprintBase
{
	GENERATED_BODY()

public:
	UBTService_FindEnemy();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxSightDistance;
	
	
};
