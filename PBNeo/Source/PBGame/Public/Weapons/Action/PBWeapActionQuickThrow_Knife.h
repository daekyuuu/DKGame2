// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/Action/PBWeapActionQuickThrow.h"
#include "PBWeapActionQuickThrow_Knife.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "QuickThrowingKnife"))
class PBGAME_API UPBWeapActionQuickThrow_Knife : public UPBWeapActionQuickThrow
{
	GENERATED_BODY()
	
public:
	UPBWeapActionQuickThrow_Knife();

	virtual void StartThrow() override;
	virtual void EndThrow() override;
	
	
};
