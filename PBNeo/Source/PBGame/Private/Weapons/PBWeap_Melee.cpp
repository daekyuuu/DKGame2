// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWeap_Melee.h"
#include "PBGameplayStatics.h"
#include "Weapons/PBWeapFanTrace.h"

APBWeap_Melee::APBWeap_Melee()
{
	WeaponType = EWeaponType::KNIFE;
}

void APBWeap_Melee::Firing(EWeaponPoint ePoint)
{
	UpdateFirstFiringFlag();

	//clear
	PreFiring(ePoint);

	if (GetNetMode() != NM_DedicatedServer)
	{
		PlayWeaponFireEffect(ePoint);
	}

	//attack animation / sound
	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		//UseAmmo();

		// replicated for sync firing
		FiringSync.BurstCounter++;
	}

	//refiring
	PostFiring(ePoint);
}

void APBWeap_Melee::ServerFiring_Implementation(EWeaponPoint ePoint)
{
	const bool bShouldUpdateAmmo = false; //(CurrentAmmoInClip > 0 && CanFire());

	Firing(ePoint);

	if (bShouldUpdateAmmo)
	{
		// update ammo (던졌을 경우만)
		UseAmmo();
	}
	FiringSync.BurstCounter++;
}

