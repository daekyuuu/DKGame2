// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Weapons/PBWeapon.h"
#include "PBWeapLineTrace.generated.h"

/**
 * 
 */
extern int32 DebugLineNumber;
class APBWeapon;

UCLASS(DefaultToInstanced, EditInlineNew, meta = (DisplayName = "WeaponLineTrace"))
class PBGAME_API UPBWeapLineTrace : public UObject
{
	GENERATED_BODY()
	
public:
	UPBWeapLineTrace();

	virtual bool TraceHit(APBWeapon* Weapon, const FVector& StartTrace, const FVector& ShootDir, float Range, EAttackDirType AttackDirType, FHitResult& OutHit);
	virtual bool TraceHits(APBWeapon* Weapon, const FVector& StartTrace, const FVector& ShootDir, float Range, EAttackDirType AttackDirType, TArray<FHitResult>& OutHits);
	
	bool LineTraceHit(APBWeapon* Weapon, const FVector& StartTrace, const FVector& ShootDir, float Range, FHitResult& OutHit);
	bool LineTraceHit(APBWeapon* Weapon, const FVector& StartTrace, const FVector& EndTrace, FHitResult& OutHit);

	bool LineTraceHits(APBWeapon* Weapon, const FVector& StartTrace, const FVector& ShootDir, float Range, TArray<FHitResult>& OutHits);
	bool LineTraceHits(APBWeapon* Weapon, const FVector& StartTrace, const FVector& EndTrace, TArray<FHitResult>& OutHits);

protected:

	// 디버그 라인 표시해주는 플래그인데 이건 나중에 콘솔명령으로 변경 예정임.
	UPROPERTY(EditDefaultsOnly, Category = DebugConfig)
	bool bShowDebugLine;
};
