// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "PBWeaponTypes.h"
#include "Weapons/PBWeapon.h"
#include "PBWeapAction.generated.h"

class APBCharacter;

/**
 * 
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew, CustomConstructor, Within=PBWeapon)
class PBGAME_API UPBWeapAction : public UObject
{
	GENERATED_BODY()

public:
	UPBWeapAction();

	inline APBCharacter* GetOwnerPawn()
	{
		return GetOuterAPBWeapon()->GetOwnerPawn();
	}
	virtual UWorld* GetWorld() const override
	{
		return GetOuterAPBWeapon()->GetWorld();
	}
	virtual void BeginState(EFireTrigger eFireTrigger=EFireTrigger::Fire1);
	virtual void EndState(EFireTrigger eFireTrigger=EFireTrigger::Fire1);

};
