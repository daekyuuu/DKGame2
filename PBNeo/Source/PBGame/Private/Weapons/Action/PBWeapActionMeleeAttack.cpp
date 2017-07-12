// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWeapActionMeleeAttack.h"




UPBWeapActionMeleeAttack::UPBWeapActionMeleeAttack()
{

}

void UPBWeapActionMeleeAttack::BeginState(EFireTrigger eFireTrigger /*= EFireTrigger::Fire1*/)
{
	Super::BeginState(eFireTrigger);
	
	APBWeapon* Weapon = GetOuterAPBWeapon();
	if (Weapon)
	{		
		Weapon->SetWeaponFireState(eFireTrigger, false);
		Weapon->SetWeaponAttackState(eFireTrigger, true);
	}
}

void UPBWeapActionMeleeAttack::EndState(EFireTrigger eFireTrigger /*= EFireTrigger::Fire1*/)
{
	Super::EndState(eFireTrigger);
	
	APBWeapon* Weapon = GetOuterAPBWeapon();
	if (Weapon)
	{
		Weapon->SetWeaponAttackState(eFireTrigger, false);
	}
}
