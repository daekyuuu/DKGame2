// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWeapActionModeSwitching.h"




UPBWeapActionModeSwitching::UPBWeapActionModeSwitching()
{

}

void UPBWeapActionModeSwitching::BeginState(EFireTrigger eFireTrigger /*= EFireTrigger::Fire1*/)
{
	Super::BeginState(eFireTrigger);
	
	APBCharacter* Pawn = GetOwnerPawn();
	if (Pawn)
	{
		Pawn->ToggleSwitchWeaponMode();
	}
}

void UPBWeapActionModeSwitching::EndState(EFireTrigger eFireTrigger /*= EFireTrigger::Fire1*/)
{
	Super::EndState(eFireTrigger);
}
