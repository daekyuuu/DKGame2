// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "PBItemTable_Base.h"
#include "PBItemTable_Weap.generated.h"

class APBWeapon;
struct FPBWeaponTableData;
/**
 * 
 */

USTRUCT(BlueprintType)
struct FPBWeaponTableData : public FPBTableDataBase
{
	GENERATED_USTRUCT_BODY()

public:
	FPBWeaponTableData()
	{
		TableType = EPBTableType::Weapon;
	}
	void LoadDefaultObject() override;
	void ApplyDataToDefaultObject(int32 ID) override;

	// int32 ItemID; Key값 (ID 필드는 자동으로 key 값으로 들어가고 있음)
	// 네이밍은 .CSV 필드명과 동일해야함

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		FString Memo;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		int32 NameStringId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		TSubclassOf<class APBWeapon> BPClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		int32 WeaponTypeEnum;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		int32 WeaponSlotEnum;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		int32 WeaponModeWeaponID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		int32 NeedLevel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		int32 NeedGold;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		int32 DescriptionStringId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		FString IconResShop;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		FString IconResKilllog;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		FString IconResInfo;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float MovementSpeedModifierBase;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float MovementSpeedModifierOnFire;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float MovementSpeedModifierOnZoom;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		int32 MagazineCapacity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		int32 TotalAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float BulletFireInterval;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float RateOfFire;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float ReloadDuration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float EquipDuration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float BulletRange;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float BulletDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float BulletDamageMin;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float BulletDamageSlope;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float BulletDamageSlopeMinRange;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float MeleeAttackRange;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float MeleeAttackDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float MeleeAttackInterval;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float ExplosionDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float ExplosionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float ZoomFOV;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float ZoomInterpDelay;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float CriticalChance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float CriticalAttackPointRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float RecoilHorzBeginatShot;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float RecoilHorzAngle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float RecoilHorzMaxAngle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float RecoilVertMinAddition;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float RecoilVertAngle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float RecoilVertMaxAngle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float RecoilReturnDelayFiring;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float RecoilReturnDelayNormal;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float CamRecoilRatio;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float CamSyncDelay;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float DeviationBeginatShot;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float BaseDeviation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float MaxDeviation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float ZoomDeviationModifier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float DeviationAddRateFire;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float DeviationReturnDelayFiring;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float DeviationReturnDelayNormal;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float DeviationRateWalk;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float DeviationRateSprint;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float DeviationRateCrouch;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float DeviationRateJump;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float DeviationAddRateHit;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float ExpansionBeginatShot;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float BaseExpansion;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float MaxExpansion;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float ZoomExpansionModifier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float ExpansionAddRateFire;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float ExpansionAddRateHit;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float ExpansionReturnDelayFiring;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float ExpansionReturnDelayNormal;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float ExpansionRateWalk;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float ExpansionRateSprint;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float ExpansionRateCrouch;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float ExpansionRateJump;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float Penetrate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponData)
		float PiercingPower;
};

UCLASS()
class UPBItemTable_Weap : public UPBItemTable_Base
{
	GENERATED_BODY()

public:
	UPBItemTable_Weap(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get())
		: Super(ObjectInitializer)
	{
		TableType = EPBTableType::Weapon;
	}

	FPBWeaponTableData* GetData(int32 ItemID);
	void ApplyDataTo(APBWeapon* Weapon, int32 ItemID);
};