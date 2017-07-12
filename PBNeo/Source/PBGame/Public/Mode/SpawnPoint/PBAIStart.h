// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Mode/SpawnPoint/PBPlayerStart.h"
#include "PBAIStart.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API APBAIStart : public APBPlayerStart
{
	GENERATED_BODY()
	
	
public:
	APBAIStart();

	virtual bool IsAllowed(AController* Player) override;
	
};
