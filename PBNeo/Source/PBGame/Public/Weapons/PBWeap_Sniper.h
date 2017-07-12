// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/PBWeap_Instant.h"
#include "PBWeap_Sniper.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class PBGAME_API APBWeap_Sniper : public APBWeap_Instant
{
	GENERATED_BODY()
	
public:
	APBWeap_Sniper();
	
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnUnEquip() override;

	virtual void PostFiring(EWeaponPoint ePoint) override;

	virtual void OnFireFinished() override;

	virtual bool CanZooming() const override;
	virtual void SetZooming(bool bZoom) override;

protected:
	void UpdateZooming();
	void PostFiringZoomOut();

	UPROPERTY(EditDefaultsOnly, Category = Config)
	float PostFiringZoomOutDelay;

	FTimerHandle TimerHandle_PostFiringZoomOut;
};
