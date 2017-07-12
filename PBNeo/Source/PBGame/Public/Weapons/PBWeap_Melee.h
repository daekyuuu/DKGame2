// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/PBWeap_Instant.h"
#include "PBWeap_Melee.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class PBGAME_API APBWeap_Melee : public APBWeap_Instant
{
	GENERATED_BODY()

public:
	APBWeap_Melee();

	virtual void Firing(EWeaponPoint ePoint) override;
	
	virtual void ServerFiring_Implementation(EWeaponPoint ePoint) override;
};
