// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PBBulletBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class PBGAME_API APBBulletBase : public AActor
{
	GENERATED_BODY()
	
public:
	APBBulletBase();

	UFUNCTION(BlueprintCallable, Category = Hit)
	virtual void OnPointHit(const FHitResult& HitResult);
	virtual void ApplyImpulse(const FHitResult& HitResult);
	
	virtual bool ApplyTableData(const struct FPBWeaponTableData* Data);
	APBCharacter* GetOwnerPawn() const;
	bool GetViewerIsFirstPerson() const;
protected:	

	/** check if weapon should deal damage to actor */
	virtual bool ShouldDealDamage(AActor* TestActor) const;

	/** handle damage */
	virtual void DealDamage(const FHitResult& Impact, const FVector& ShootDir);

	/** spawn effects for impact */
	virtual void SpawnImpactEffects(const FHitResult& Impact);

public:
	UPROPERTY()
	int32 WeaponId;

	EWeaponPoint BulletPoint;
};
