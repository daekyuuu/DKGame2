// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/Action/PBWeapActionQuickThrow.h"
#include "PBWeapActionQuickThrow_Grenade.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "QuickThrowingGrenade"))
class PBGAME_API UPBWeapActionQuickThrow_Grenade : public UPBWeapActionQuickThrow
{
	GENERATED_BODY()
	
public:
	UPBWeapActionQuickThrow_Grenade();

	virtual void StartThrow() override;
	virtual void EndThrow() override;
	
	
};
