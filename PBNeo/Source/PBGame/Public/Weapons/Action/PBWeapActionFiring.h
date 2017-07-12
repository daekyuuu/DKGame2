// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/Action/PBWeapAction.h"
#include "PBWeapActionFiring.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class PBGAME_API UPBWeapActionFiring : public UPBWeapAction
{
	GENERATED_BODY()
	
public:
	UPBWeapActionFiring();

	virtual void BeginState(EFireTrigger eFireTrigger) override;
	virtual void EndState(EFireTrigger eFireTrigger) override;

	virtual bool StartFire(EFireTrigger eFireTrigger);
	virtual void EndFire(EFireTrigger eFireTrigger);
	
};
