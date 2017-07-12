// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/PBWeap_Projectile.h"
#include "PBWeap_Throw.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class PBGAME_API APBWeap_Throw : public APBWeap_Projectile
{
	GENERATED_BODY()
	
public:
	APBWeap_Throw();

	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const override;

	virtual bool CanEquip() const override;
	virtual bool CanFire() const override;
	virtual void OnEquip(const APBWeapon* LastWeapon) override;
	virtual void OnUnEquip() override;

	virtual void StartFire(EFireTrigger eFireTrigger, int32 BurstCount = -1) override;
	virtual void EndFire(EFireTrigger eFireTrigger) override;
	virtual void OnFireStarted(int32 BurstCount);

	virtual void FireReady(EFireTrigger eFireTrigger) override;
	virtual void Firing(EWeaponPoint ePoint) override;
	virtual void PostFiring(EWeaponPoint ePoint) override;
	virtual void PostFiringEnd() override;

	virtual void FireWeapon(EWeaponPoint ePoint) override;
	virtual void StopReload() override;

	void SetQuickThrowing();

protected:
	FTimerHandle TimerHandle_Emit;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	float FireStartOffsetDistance;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	float FireStartOffsetHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Config)
	UCurveFloat* DistanceToThrowSpeedRatio;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	float FireEmitDelay;

	bool bQuickThrowing;
};
