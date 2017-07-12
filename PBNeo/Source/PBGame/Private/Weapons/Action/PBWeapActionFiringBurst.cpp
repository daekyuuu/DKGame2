// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWeapActionFiringBurst.h"


UPBWeapActionFiringBurst::UPBWeapActionFiringBurst()
{
	SemiAutoBurstCount = 3;
	BurstMode = EWeapBurstMode::Auto;
}

bool UPBWeapActionFiringBurst::StartFire(EFireTrigger eFireTrigger)
{
	APBWeapon* Weapon = GetOuterAPBWeapon();
	if (Weapon)
	{
		//3점사 설정
		int32 BurstCount = (BurstMode == EWeapBurstMode::Auto) ? -1 : SemiAutoBurstCount;

		Weapon->SetReFiring(BurstCount != 1);
		Weapon->StartFire(eFireTrigger, BurstCount);
		return true;
	}
	return false;
}

void UPBWeapActionFiringBurst::EndFire(EFireTrigger eFireTrigger)
{
	APBWeapon* Weapon = GetOuterAPBWeapon();
	if (Weapon)
	{
		Weapon->EndFire(eFireTrigger);
	}
}
