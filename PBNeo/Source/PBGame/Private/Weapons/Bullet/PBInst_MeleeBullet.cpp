// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBInst_MeleeBullet.h"
#include "Weapons/PBWeapFanTrace.h"
#include "Weapons/PBWeapLineTrace.h"

APBInst_MeleeBullet::APBInst_MeleeBullet()
{
	BulletData.BulletRange = 150.f;
	BulletData.BulletHitDamage = 60;
}

bool APBInst_MeleeBullet::ApplyTableData(const struct FPBWeaponTableData* Data)
{
	bool bIsDirty = false;

	if (Data)
	{
		bIsDirty = BulletData.ImportTableData(Data, bIsDirty, true);
	}

	return bIsDirty;
}

void APBInst_MeleeBullet::PointHit_Confirm(const FHitResult& Impact, const FVector& ShootPoint, const FVector& ShootDir)
{
	// play FX on remote clients
	if (Role == ROLE_Authority)
	{
		HitNotify.Origin = ShootPoint;
		HitNotify.ShootDir = ShootDir;
	}

	// handle damage
	if (ShouldDealDamage(Impact.GetActor()))
	{
		DealDamage(Impact, ShootDir);
	}

	// play FX locally
	if (GetNetMode() != NM_DedicatedServer)
	{
		SpawnImpactEffects(Impact);
	}
}

void APBInst_MeleeBullet::OnRep_HitNotify()
{
	FHitResult Impact;

	if (HitTrace)
	{
		const FVector StartTrace = HitNotify.Origin;
		const FVector EndTrace = StartTrace + HitNotify.ShootDir * BulletData.BulletRange;
		HitTrace->LineTraceHit(Cast<APBWeapon>(GetOwner()), StartTrace, EndTrace, Impact);
	}

	if (Impact.bBlockingHit)
	{
		SpawnImpactEffects(Impact);
	}
}
