// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UMG/PBSubWidget.h"
#include "PBWeaponInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class PBGAME_API UPBWeaponInfoWidget : public UPBSubWidget
{
	GENERATED_BODY()
public:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, category = "HUD")
		int32 GetCurrentAmmoInClip() const;

	UFUNCTION(BlueprintCallable, category = "HUD")
		int32 GetTotalAmmo() const;

	UFUNCTION(BlueprintCallable, category = "HUD")
		float GetHealthPercentage() const;

	UFUNCTION(BlueprintCallable, category = "HUD")
		int32 GetCeiledHealth() const;

	UFUNCTION(BlueprintCallable, category = "HUD")
		UObject* GetWeaponIcon() const;

protected:
	// Temp function for test.
	UFUNCTION(BlueprintCallable, category = "HUD")
		EWeaponType GetCurrWeapType();


	// Temp function for test.
	UFUNCTION(BlueprintImplementableEvent, category = "HUD")
		void UpdateLayout();

	UFUNCTION(BlueprintCallable, category = "HUD")
		FText GetGrenadeAmmo() const;

	UFUNCTION(BlueprintCallable, category = "HUD")
		FText GetSpecialAmmo() const;

	UFUNCTION(BlueprintCallable, category = "HUD")
		class APBWeapon* GetCurrentWeapon() const;


	UFUNCTION(BlueprintCallable, category = "HUD")
		FText GetWeapClassName() const;

private:

	void NotifyWeaponChange();
	
};
