// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Mode/Mission/PBMission.h"
#include "PBMission_BombMissionAttack.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API APBMission_BombMissionAttack : public APBMission
{
	GENERATED_BODY()

public:

	APBMission_BombMissionAttack();

protected:
	void NotifyKill(APBPlayerState* Killer, APBPlayerState* Killed) override;

	void NotifyGlobalEvent(FName EventName) override;

public:
	// Call everywhere
	bool CanPlantBomb();
	
protected:
	UFUNCTION()
	void OnDisplayResult();

	UPROPERTY(Transient, Replicated)
	bool bBombPlanted;
	
private:
	FDelegateHandle OnDisplayResultHandle;
};
