// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBPickupAmmo.h"
#include "Weapons/PBWeapon.h"
#include "Player/PBCharacter.h"

APBPickupAmmo::APBPickupAmmo()
{
	Ammo = 100;
}

bool APBPickupAmmo::IsForWeapon(UClass* WeaponClass)
{
	return WeaponType->IsChildOf(WeaponClass);
}

void APBPickupAmmo::GivePickupTo(APBCharacter* Player)
{
	if (Player == nullptr)
	{
		APBWeapon* Weapon = Player->FindWeaponByType(WeaponType);
		if (Weapon)
		{
			Weapon->GiveAmmo(Ammo);
		}
	}
}
