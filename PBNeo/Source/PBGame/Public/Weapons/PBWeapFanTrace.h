// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/PBWeapLineTrace.h"
#include "PBWeapFanTrace.generated.h"
class APBWeapon;

/**
 * 
 */
UCLASS(DefaultToInstanced, EditInlineNew, meta = (DisplayName = "WeaponFanTrace"))
class PBGAME_API UPBWeapFanTrace : public UPBWeapLineTrace
{
	GENERATED_BODY()
	
public:
	UPBWeapFanTrace();

	virtual bool TraceHit(APBWeapon* Weapon, const FVector& StartTrace, const FVector& ShootDir, float Range, EAttackDirType AttackDirType, FHitResult& OutHit);

	bool FanTraceHit(APBWeapon* Weapon, const FVector& StartTrace, const FVector& ShootDir, float Range, EAttackDirType AttackDirType, FHitResult& OutHit);

protected:

	static void AdjustTraceDir(const FVector& InShootDir, float InRotateZ, float InRotateX, FVector& OutAdjustedDir);
	static void GetCalcTraceRotate(EAttackDirType AttackDirType, float AngleStart, float AngleDeviation, float& OutRotateZ, float& OutRotateX);

	// 부채꼴의 삼각형 개수 (홀수로 해야 가운데 방향 Trace 중복 없다)
	UPROPERTY(EditDefaultsOnly, Category = Config)
	int32 TraceAngleSegment;

	// 부채꼴의 최대 각
	UPROPERTY(EditDefaultsOnly, Category = Config)
	int32 TraceAngleRange;
};
