// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/Attachment/PBWeapAttachment.h"
#include "PBWeapAttach_Dual.generated.h"

/**
 * 
 */
UCLASS(DefaultToInstanced, EditInlineNew, CustomConstructor, Within = PBWeapon, meta = (DisplayName = "Weapon Dual Setting"))
class PBGAME_API UPBWeapAttach_Dual : public UPBWeapAttachment
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	UPBWeapAttach_Dual();

	virtual void CreateMesh() override;
	virtual void Init() override;

	virtual void OnMeleeAttack() override;
	virtual void OnFiring() override;

	virtual void PlayWeaponFireEffects() override;
	virtual void PlayWeaponFireEffect(EWeaponPoint ePoint) override;

	virtual EWeaponPoint GetFiringPoint() override;

protected:
	virtual void SwitchFiringPoint();

	UPROPERTY(EditAnywhere, Category = Weapon, meta = (DisplayName = "FireMode"))
	EFireMode eFireMode;

	UPROPERTY(EditDefaultsOnly, Category = Weapon, meta = (DisplayName = "Left Hand Weapon  =============================="))
	FWeaponAttachmentData AttachmentData_Left;
};
