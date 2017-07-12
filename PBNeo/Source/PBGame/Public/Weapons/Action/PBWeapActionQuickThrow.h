// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/Action/PBWeapAction.h"
#include "PBWeapActionQuickThrow.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class PBGAME_API UPBWeapActionQuickThrow : public UPBWeapAction
{
	GENERATED_BODY()
	
public:
	UPBWeapActionQuickThrow();

	virtual void BeginState(EFireTrigger eFireTrigger = EFireTrigger::Fire1) override;
	virtual void EndState(EFireTrigger eFireTrigger = EFireTrigger::Fire1) override;

	virtual void StartThrow();
	virtual void EndThrow();

public:
	EWeaponType ThrowingWeapType;

protected:
	bool bThrowActivated;
};
