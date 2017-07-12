// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/PBWeap_Projectile.h"
#include "PBWeap_Bomb.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API APBWeap_Bomb : public APBWeap_Projectile
{

	GENERATED_BODY()

public:

	APBWeap_Bomb();

	void PostInitializeComponents() override;
	
	void ServerFireProjectile_Implementation(EWeaponPoint ePoint, FVector Origin, FVector_NetQuantizeNormal ShootDir, float InitialSpeedProportion /* = 1.0f */) override;

	void AlertModeBombPlanted(class APBProj_Bomb *ProjBomb, const class APBTrigger_BombSite *BombSite);

	UFUNCTION(reliable, client)
	void ClientConfirmBombPlanted();
};
