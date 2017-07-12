// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBPickupWeapon.h"
#include "PBWeapon.h"
#include "PBItemTableManager.h"
#include "PBWeaponData.h"
#include "PBWeapAttachment.h"

APBPickupWeapon::APBPickupWeapon()
{
	CurrentAmmo = -1;
	CurrentAmmoInClip = -1;
}

FString APBPickupWeapon::GetPickupName()
{
	FString PickupName;
	if (WeaponType)
	{
		auto Weap = Cast<APBWeapon>(WeaponType->GetDefaultObject());
		if (Weap)
		{
			auto TM = UPBGameplayStatics::GetItemTableManager(this);
			if (TM)
			{
				auto TD = TM->GetWeaponTableData(Weap->GetItemID());
				if (TD)
				{
					PickupName = TD->Name;
				}
			}
		}
	}

	return PickupName;
}


void APBPickupWeapon::MulticastSetPickupContentsFromWeapon_Implementation(class APBWeapon* InWeap)
{
	if (InWeap)
	{
		WeaponType = InWeap->GetClass();

		auto Mesh = InWeap->GetWeaponMesh(EWeaponPoint::Point_R);
		if (Mesh)
		{
			if (PickupMesh)
			{
				PickupMesh->SetSkeletalMesh(Mesh->SkeletalMesh);
			}
		}
	}
}

// called server only
void APBPickupWeapon::GivePickupTo(APBCharacter* Player)
{
	if (Player)
	{
		if (CurrentAmmo != -1)
		{
			Player->PickedUpWeapon(WeaponType, CurrentAmmo, CurrentAmmoInClip);
		}
		else
		{
			Player->PickedUpWeapon(WeaponType);

		}
	}	
}
