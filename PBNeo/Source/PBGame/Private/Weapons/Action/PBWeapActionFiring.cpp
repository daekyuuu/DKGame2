// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWeapActionFiring.h"
#include "Weapons/PBWeapon.h"


UPBWeapActionFiring::UPBWeapActionFiring()
{

}

void UPBWeapActionFiring::BeginState(EFireTrigger eFireTrigger)
{
	Super::BeginState(eFireTrigger);
	StartFire(eFireTrigger);
}

void UPBWeapActionFiring::EndState(EFireTrigger eFireTrigger)
{
	Super::EndState(eFireTrigger);
	EndFire(eFireTrigger);
}

bool UPBWeapActionFiring::StartFire(EFireTrigger eFireTrigger)
{
	return true;
}

void UPBWeapActionFiring::EndFire(EFireTrigger eFireTrigger)
{

}
