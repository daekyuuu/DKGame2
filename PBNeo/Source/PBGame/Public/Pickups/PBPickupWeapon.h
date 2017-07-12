// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickups/PBPickup.h"
#include "PBPickupWeapon.generated.h"



/**
 * 
 */
UCLASS(HideDropdown)
class APBPickupWeapon : public APBPickup
{
	GENERATED_BODY()
	
public:

	APBPickupWeapon();

	FString GetPickupName();
	
	UFUNCTION(reliable, NetMulticast)
	void MulticastSetPickupContentsFromWeapon(class APBWeapon* InWeap);

	TSubclassOf<class APBWeapon> GetWeaponType() const { return WeaponType; }


	// if it's -1 then just set as new one.
	UPROPERTY()
		int32 CurrentAmmo;

	UPROPERTY()
		int32 CurrentAmmoInClip;

protected:

	/** give pickup */
	virtual void GivePickupTo(APBCharacter* Player) override;

	/** which weapon gets ammo? */
	UPROPERTY(EditDefaultsOnly, Category = Pickup)
	TSubclassOf<class APBWeapon> WeaponType;



};
