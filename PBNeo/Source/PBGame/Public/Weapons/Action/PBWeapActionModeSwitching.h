// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/Action/PBWeapAction.h"
#include "PBWeapActionModeSwitching.generated.h"

/**
 * 무기의 모드를 변경한다 ( 크리스의 경우 싱글 <--> 듀얼 모드로 스위칭)
 */
UCLASS(meta = (DisplayName = "WeapModeSwitching"))
class PBGAME_API UPBWeapActionModeSwitching : public UPBWeapAction
{
	GENERATED_BODY()
	
public:
	UPBWeapActionModeSwitching();

	virtual void BeginState(EFireTrigger eFireTrigger = EFireTrigger::Fire1) override;
	virtual void EndState(EFireTrigger eFireTrigger = EFireTrigger::Fire1) override;
	
	
};
