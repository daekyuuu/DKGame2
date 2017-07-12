// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/Bullet/PBInst_Bullet.h"
#include "PBInst_MeleeBullet.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class PBGAME_API APBInst_MeleeBullet : public APBInst_Bullet
{
	GENERATED_BODY()
	
public:

	APBInst_MeleeBullet();

	virtual bool ApplyTableData(const struct FPBWeaponTableData* Data) override;
	virtual void PointHit_Confirm(const FHitResult& Impact, const FVector& ShootPoint, const FVector& ShootDir) override;
	virtual void OnRep_HitNotify() override;
};
