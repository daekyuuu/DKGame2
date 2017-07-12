// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Mode/Mission/PBMission.h"
#include "PBMission_BombMissionDefense.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API APBMission_BombMissionDefense : public APBMission
{
	GENERATED_BODY()

public:

	APBMission_BombMissionDefense();

	void NotifyKill(APBPlayerState* Killer, APBPlayerState* Killed) override;

	void NotifyGlobalEvent(FName EventName) override;

	virtual void NotifyMatchTimeup();

	// Call everywhere
	bool CanDefuseBomb();

protected:

	bool bBombPlanted;
	
};
