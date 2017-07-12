// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/Action/PBWeapAction.h"
#include "PBWeapActionZooming.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Zooming"))
class PBGAME_API UPBWeapActionZooming : public UPBWeapAction
{
	GENERATED_BODY()
	
public:
	UPBWeapActionZooming();

	virtual void BeginState(EFireTrigger eFireTrigger) override;
	virtual void EndState(EFireTrigger eFireTrigger) override;
	
	
};
