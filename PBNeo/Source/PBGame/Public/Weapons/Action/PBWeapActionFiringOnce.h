// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/Action/PBWeapActionFiring.h"
#include "PBWeapActionFiringOnce.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "FiringOnce"))
class PBGAME_API UPBWeapActionFiringOnce : public UPBWeapActionFiring
{
	GENERATED_BODY()
	
public:

	virtual bool StartFire(EFireTrigger eFireTrigger) override;
	virtual void EndFire(EFireTrigger eFireTrigger) override;
	
};
