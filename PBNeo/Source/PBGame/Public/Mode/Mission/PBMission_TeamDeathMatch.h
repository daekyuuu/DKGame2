// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Mode/Mission/PBMission.h"
#include "PBMission_TeamDeathMatch.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API APBMission_TeamDeathMatch : public APBMission
{
	GENERATED_BODY()

public:

	APBMission_TeamDeathMatch();
	
	UPROPERTY(config)
	int32 KillMatchPoint;

	UPROPERTY(config)
	int32 DeathMatchPoint;

	UPROPERTY(config)
	int32 TargetMatchPoint;

	void NotifyKill(APBPlayerState* Killer, APBPlayerState* Killed) override;	
};