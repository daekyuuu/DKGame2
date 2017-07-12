// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/Action/PBWeapAction.h"
#include "PBWeapActionMeleeAttack.generated.h"

/**
 * ���Ӹ��� ����, ��� �� ������ü�� �����Ѵ�.
 */
UCLASS(meta = (DisplayName = "MeleeAttack"))
class PBGAME_API UPBWeapActionMeleeAttack : public UPBWeapAction
{
	GENERATED_BODY()
	
public:
	UPBWeapActionMeleeAttack();

	virtual void BeginState(EFireTrigger eFireTrigger = EFireTrigger::Fire1) override;
	virtual void EndState(EFireTrigger eFireTrigger = EFireTrigger::Fire1) override;
	
	
};
