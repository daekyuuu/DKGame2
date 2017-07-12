// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/Action/PBWeapAction.h"
#include "PBWeapActionThrow.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class PBGAME_API UPBWeapActionThrow : public UPBWeapAction
{
	GENERATED_BODY()
	
public:
	UPBWeapActionThrow();

	virtual void BeginState(EFireTrigger eFireTrigger = EFireTrigger::Fire1) override;
	virtual void EndState(EFireTrigger eFireTrigger = EFireTrigger::Fire1) override;

	virtual void StartThrow();
	virtual void EndThrow();
	
	
};
