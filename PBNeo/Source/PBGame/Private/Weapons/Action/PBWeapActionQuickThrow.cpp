// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWeapActionQuickThrow.h"

#include "Weapons/PBWeap_Throw.h"


UPBWeapActionQuickThrow::UPBWeapActionQuickThrow()
{
	ThrowingWeapType = EWeaponType::GRENADE;
}

void UPBWeapActionQuickThrow::BeginState(EFireTrigger eFireTrigger /*= EFireTrigger::Fire1*/)
{
	Super::BeginState(eFireTrigger);	

	StartThrow();
}

void UPBWeapActionQuickThrow::EndState(EFireTrigger eFireTrigger /*= EFireTrigger::Fire1*/)
{
	Super::EndState(eFireTrigger);

	EndThrow();
}

void UPBWeapActionQuickThrow::StartThrow()
{

}

void UPBWeapActionQuickThrow::EndThrow()
{
	
}
