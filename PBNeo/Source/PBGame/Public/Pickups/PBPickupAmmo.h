// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickups/PBPickup.h"
#include "PBPickupAmmo.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class APBPickupAmmo : public APBPickup
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	APBPickupAmmo();
	
	bool IsForWeapon(UClass* WeaponClass);

protected:

	virtual void GivePickupTo(APBCharacter* Player) override;

	UPROPERTY(EditDefaultsOnly, Category = Pickup)
	int32 Ammo;

	/** which weapon gets ammo? */
	UPROPERTY(EditDefaultsOnly, Category = Pickup)
	TSubclassOf<class APBWeapon> WeaponType;
};
