// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "PBTableManager.h"
#include "Table/Item/PBItemType.h"
#include "Table/Item/PBItemTable_Base.h"
#include "Table/Item/PBItemTable_Weap.h"
#include "Table/Item/PBItemTable_Char.h"
#include "PBItemTableManager.generated.h"

class APBWeapon;
class APBCharacter;
class UPBItemTable_Base;

/**
 * 인벤토리, 상점에 들어가는 아이템 데이타를 관리한다.
 */

UCLASS()
class PBGAME_API UPBItemTableManager : public UPBTableManager
{
	GENERATED_BODY()

public:
	UPBItemTableManager(const FObjectInitializer& ObjectInitializer);

	static UPBItemTableManager* Get(UObject* WorldContextObject);
public:

	void ApplyItemData(AActor* AppliedActor, int32 ItemID);
	void ApplyItemDataToWeapon(APBWeapon* Weapon, int32 ItemID);
	void ApplyItemDataToPawn(APBCharacter* Pawn, int32 ItemID);

	bool ApplyWeapItemToPawn(APBCharacter* Pawn, int32 ItemID, bool bImmediatelyEquip=false, int32 WantsSlotIdx=-1);

	EPBItemType GetItemTypeBy(int32 ItemID);
	EPBItemSubType GetItemSubTypeBy(int32 ItemID);

	EPBTableType GetTableTypeFrom(int32 ItemID);
	
	struct FPBWeaponTableData* GetWeaponTableData(int32 ItemID);
	struct FPBCharacterTableData* GetCharacterTableData(int32 ItemID);

	void LoadDefaultObjects();

protected:

	virtual UPBTable* CreateInstance(EPBTableType Type) override;

};