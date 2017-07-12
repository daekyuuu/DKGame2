// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PBWeapon.h"
#include "PBWeap_Projectile.generated.h"


/**
*
*/
UCLASS(Abstract)
class PBGAME_API APBWeap_Projectile : public APBWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APBWeap_Projectile();

protected:

	/** [local] weapon specific fire implementation */
	virtual void FireWeapon(EWeaponPoint ePoint) override;

	/** spawn projectile on server */
	UFUNCTION(reliable, server, WithValidation)
	void ServerFireProjectile(EWeaponPoint ePoint, FVector Origin, FVector_NetQuantizeNormal ShootDir, float InitialSpeedProportion = 1.0f);

};
