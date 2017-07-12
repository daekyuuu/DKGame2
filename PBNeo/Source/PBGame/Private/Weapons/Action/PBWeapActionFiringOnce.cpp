// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWeapActionFiringOnce.h"



bool UPBWeapActionFiringOnce::StartFire(EFireTrigger eFireTrigger)
{
	APBWeapon* Weapon = GetOuterAPBWeapon();
	if (Weapon)
	{
		Weapon->SetReFiring(false);
		Weapon->StartFire(eFireTrigger, 1);
		return true;
	}
	return false;
}

void UPBWeapActionFiringOnce::EndFire(EFireTrigger eFireTrigger)
{
	APBWeapon* Weapon = GetOuterAPBWeapon();
	if (Weapon)
	{
		Weapon->EndFire(eFireTrigger);
	}
}
