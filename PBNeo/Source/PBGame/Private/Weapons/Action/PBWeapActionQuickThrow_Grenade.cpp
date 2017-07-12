// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWeapActionQuickThrow_Grenade.h"




UPBWeapActionQuickThrow_Grenade::UPBWeapActionQuickThrow_Grenade()
{

}

void UPBWeapActionQuickThrow_Grenade::StartThrow()
{
	APBCharacter* MyPawn = GetOwnerPawn();
	if (MyPawn)
	{
		APBWeap_Throw* ThrowWeapon = nullptr;
		if (ThrowingWeapType == EWeaponType::GRENADE)
		{
			ThrowWeapon = Cast<APBWeap_Throw>(MyPawn->GetGrenadeWeapon());
		}
		else if (ThrowingWeapType == EWeaponType::SPECIAL)
		{
			ThrowWeapon = Cast<APBWeap_Throw>(MyPawn->GetSpecialWeapon());
		}
		
		bThrowActivated = MyPawn->StartQuickThrow(ThrowWeapon);
	}	
}

void UPBWeapActionQuickThrow_Grenade::EndThrow()
{
	APBCharacter* MyPawn = GetOwnerPawn();
	if (MyPawn)
	{
		if (bThrowActivated)
		{
			APBWeap_Throw* ThrowWeapon = Cast<APBWeap_Throw>(MyPawn->GetCurrentQuickWeapon());
			MyPawn->StopQuickThrow(ThrowWeapon);
			bThrowActivated = false;
		}		
	}
}
