// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PBWeapon.h"
#include "PBWeap_Instant.generated.h"

class APBInst_Bullet;
class APBImpactEffect;
struct FPBWeaponTableData;

// 이 구조체는 조만간 사라질듯 싶다.
USTRUCT()
struct FInstantWeaponData
{
	GENERATED_USTRUCT_BODY()

	/** hit verification: scale for bounding box of hit actor */
	UPROPERTY(EditDefaultsOnly, Category=HitVerification)
	float ClientSideHitLeeway;

	/** hit verification: threshold for dot product between view direction and hit direction */
	UPROPERTY(EditDefaultsOnly, Category=HitVerification)
	float AllowedViewDotHitDir;

	/** defaults */
	FInstantWeaponData()
	{
		ClientSideHitLeeway = 200.0f;
		AllowedViewDotHitDir = -1.0f;
	}
};

// A weapon where the damage impact occurs instantly upon firing
UCLASS(Abstract)
class APBWeap_Instant : public APBWeapon
{
	GENERATED_UCLASS_BODY()

public:
	virtual void OnFireFinished() override;


public:
	const FInstantWeaponData& GetWeapInstData() const;

protected:

	virtual void CreateBullets() override;
	APBInst_Bullet* GetBullet() const;

	/** weapon config */
	UPROPERTY(EditDefaultsOnly, Category=Config)
	FInstantWeaponData InstantConfig;

	//////////////////////////////////////////////////////////////////////////
	// Weapon usage

	/** server notified of hit from client to verify */
	UFUNCTION(reliable, server, WithValidation)
	void ServerNotifyHit(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir, float ReticleSpread);

	UFUNCTION(reliable, server, WithValidation)
	void ServerNotifyPierceHit(const FPierceHitResult& PierceImpact, FVector_NetQuantizeNormal ShootDir, float ReticleSpread, float PiercingPower);

	/** server notified of miss to show trail FX */
	UFUNCTION(unreliable, server, WithValidation)
	void ServerNotifyMiss(FVector_NetQuantizeNormal ShootDir);

	/** process the instant hit and notify the server if necessary */
	void ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, float ReticleSpread);
	float ProcessInstantHit(const FPierceHitResult& PierceImpacts, const FVector& Origin, const FVector& ShootDir, float ReticleSpread, float PiercingPower);

	/** continue processing the instant hit, as if it has been confirmed by the server */
	void ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir);
	float ProcessInstantHit_Confirmed(const FPierceHitResult& PierceImpact, const FVector& Origin, const FVector& ShootDir, float PiercingPower);
	
	/** [local] weapon specific fire implementation */
	virtual void FireWeapon(EWeaponPoint ePoint) override;
};
