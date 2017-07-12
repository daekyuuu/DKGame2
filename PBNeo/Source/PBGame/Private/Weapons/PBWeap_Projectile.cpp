// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWeap_Projectile.h"
#include "Weapons/Projectile/PBProjectile.h"
#include "Player/PBCharacter.h"

APBWeap_Projectile::APBWeap_Projectile()
{
}

void APBWeap_Projectile::FireWeapon(EWeaponPoint ePoint)
{
	FVector ShootDir = GetAdjustedAim();
	FVector Origin = GetMuzzleLocation();
	
	// trace from camera to check what's under crosshair
	const float ProjectileAdjustRange = 10000.0f;
	const FVector StartTrace = GetStartFireLoc(ShootDir);
	const FVector EndTrace = StartTrace + ShootDir * ProjectileAdjustRange;
	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
		
	// and adjust directions to hit that actor
	if (Impact.bBlockingHit)
	{
		const FVector AdjustedDir = (Impact.ImpactPoint - Origin).GetSafeNormal();
		bool bWeaponPenetration = false;
	
		const float DirectionDot = FVector::DotProduct(AdjustedDir, ShootDir);
		if (DirectionDot < 0.0f)
		{
			// shooting backwards = weapon is penetrating
			bWeaponPenetration = true;
		}
		else if (DirectionDot < 0.5f)
		{
			// check for weapon penetration if angle difference is big enough
			// raycast along weapon mesh to check if there's blocking hit
	
			FVector MuzzleStartTrace = Origin - GetMuzzleDirection() * 150.0f;
			FVector MuzzleEndTrace = Origin;
			FHitResult MuzzleImpact = WeaponTrace(MuzzleStartTrace, MuzzleEndTrace);
	
			if (MuzzleImpact.bBlockingHit)
			{
				bWeaponPenetration = true;
			}
		}
	
		if (bWeaponPenetration)
		{
			// spawn at crosshair position
			Origin = Impact.ImpactPoint - ShootDir * 10.0f;
		}
		else
		{
			// adjust direction to hit
			ShootDir = AdjustedDir;
		}
	}

	ServerFireProjectile(ePoint, Origin, ShootDir);
}

bool APBWeap_Projectile::ServerFireProjectile_Validate(EWeaponPoint ePoint, FVector Origin, FVector_NetQuantizeNormal ShootDir, float InitialSpeedProportion /* = 0.0f */)
{
	return true;
}

void APBWeap_Projectile::ServerFireProjectile_Implementation(EWeaponPoint ePoint, FVector Origin, FVector_NetQuantizeNormal ShootDir, float InitialSpeedProportion /* = 1.0f */)
{
	FTransform SpawnTM(ShootDir.Rotation(), Origin);

	APBProjectile* Projectile = Cast<APBProjectile>(SpawnBullet(ePoint, SpawnTM));
	if (Projectile)
	{
		auto MC = Projectile->GetMovementComp();
		if (MC && MC->IsValidLowLevel())
		{
			MC->InitialSpeed *= InitialSpeedProportion;
		}

		UPBGameplayStatics::FinishSpawningActor(Projectile, SpawnTM);

		// Notify the projectile of its intigator across network
		Projectile->Instigator = Instigator;
		Projectile->OnRep_Instigator();
	}
}
