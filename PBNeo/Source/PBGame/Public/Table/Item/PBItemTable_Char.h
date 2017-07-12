// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "PBItemTable_Base.h"
#include "PBItemTable_Char.generated.h"

class APBCharacter;
struct FPBCharacterTableData;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FPBCharacterTableData : public FPBTableDataBase
{
	GENERATED_USTRUCT_BODY()

public:
	FPBCharacterTableData()
	{
		TableType = EPBTableType::Character;
	}

	virtual void LoadDefaultObject() override;
	virtual void ApplyDataToDefaultObject(int32 ID) override;

	// int32 ItemID; Key값 (ID 필드는 자동으로 key 값으로 들어가고 있음)
	// 네이밍은 .CSV 필드명과 동일해야함
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	FString Memo;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	int32 NameStringId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	FString ModelName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	TSubclassOf<class APBCharacter> BPClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	int32 TeamEnum;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	int32 Price;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	int32 DescriptionStringId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	int32 GenderEnum;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	FString IconResShop;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	int32 MaxHP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float RunSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float DirectionSpeedRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float RunAcceleration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float RunDeceleration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float WalkSpeedRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float SitWalkSpeedRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float ZoomTime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float DamageRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float RunSpeedRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float HelmetProtectionRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float ProtectionTimeAdd;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float PenetrateRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float ReloadTimeRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float WeaponSwapTimeRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float DefenseRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float MaxHpRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float RateOfFireRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float TotalAmmoRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float MagazineAmmoRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float BulletRangeRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float ImpactRadiusRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float GetExpRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float ZoomTimeRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float RecoilReduceRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterData)
	float DeviationReduceRate;
};

UCLASS()
class UPBItemTable_Char : public UPBItemTable_Base
{
	GENERATED_BODY()

public:
	UPBItemTable_Char(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get())
		: Super(ObjectInitializer)
	{
		TableType = EPBTableType::Character;
	}

	FPBCharacterTableData* GetData(int32 ItemID);
	void ApplyDataTo(APBCharacter* Pawn, int32 ItemID);
};