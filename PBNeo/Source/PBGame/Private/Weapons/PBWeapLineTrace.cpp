// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBWeapLineTrace.h"
#include "Weapons/PBWeapon.h"

int32 DebugLineNumber = 0;

UPBWeapLineTrace::UPBWeapLineTrace()
{
	bShowDebugLine = false;
	DebugLineNumber = 0;
}

bool UPBWeapLineTrace::TraceHit(APBWeapon* Weapon, const FVector& StartTrace, const FVector& ShootDir, float Range, EAttackDirType AttackDirType, FHitResult& OutHit)
{
	return LineTraceHit(Weapon, StartTrace, ShootDir, Range, OutHit);
}

bool UPBWeapLineTrace::TraceHits(APBWeapon* Weapon, const FVector& StartTrace, const FVector& ShootDir, float Range, EAttackDirType AttackDirType, TArray<FHitResult>& OutHits)
{
	return LineTraceHits(Weapon, StartTrace, ShootDir, Range, OutHits);
}

bool UPBWeapLineTrace::LineTraceHit(APBWeapon* Weapon, const FVector& StartTrace, const FVector& ShootDir, float Range, FHitResult& OutHit)
{
	FVector EndTrace = StartTrace + ShootDir * Range;
	LineTraceHit(Weapon, StartTrace, EndTrace, OutHit);

	return OutHit.bBlockingHit;
}

bool UPBWeapLineTrace::LineTraceHit(APBWeapon* Weapon, const FVector& StartTrace, const FVector& EndTrace, FHitResult& OutHit)
{
	if (Weapon)
	{
		OutHit = Weapon->WeaponTrace(StartTrace, EndTrace);

		// debug
		if (bShowDebugLine)
		{
			UPBGameplayStatics::ShowDebugWeaponTrace(this, StartTrace, EndTrace, FColor::Red, 30.f, 5.f, DebugLineNumber++);
		}
	}

	return OutHit.bBlockingHit;
}

bool UPBWeapLineTrace::LineTraceHits(APBWeapon* Weapon, const FVector& StartTrace, const FVector& ShootDir, float Range, TArray<FHitResult>& OutHits)
{
	FVector EndTrace = StartTrace + ShootDir * Range;
	LineTraceHits(Weapon, StartTrace, EndTrace, OutHits);
	
	bool bOutHits = (OutHits.Num() > 0) ? true : false;
	return bOutHits;
}

bool UPBWeapLineTrace::LineTraceHits(APBWeapon* Weapon, const FVector& StartTrace, const FVector& EndTrace, TArray<FHitResult>& OutHits)
{
	if (Weapon)
	{
		OutHits = Weapon->WeaponTraces(StartTrace, EndTrace);

		// debug
		if (bShowDebugLine)
		{
			UPBGameplayStatics::ShowDebugWeaponTrace(this, StartTrace, EndTrace, FColor::Red, 30.f, 5.f, DebugLineNumber++);
		}
	}

	bool bOutHits = (OutHits.Num() > 0) ? true : false;
	return bOutHits;
}
