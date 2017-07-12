// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBItemTableManager.h"
#include "PBItemTable_Base.h"
#include "PBItemTable_Weap.h"
#include "PBItemTable_Char.h"
#include "PBItemType.h"

UPBItemTableManager::UPBItemTableManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	LoadTable(EPBTableType::Weapon, TEXT("/Game/Tables/tbWeapons"));
	LoadTable(EPBTableType::Character, TEXT("/Game/Tables/tbCharacters"));
}

UPBItemTableManager* UPBItemTableManager::Get(UObject* WorldContextObject)
{
	return UPBGameplayStatics::GetItemTableManager(WorldContextObject);
}

UPBTable* UPBItemTableManager::CreateInstance(EPBTableType Type)
{
	UPBTable* Instance = nullptr;

	switch (Type)
	{
	case EPBTableType::Character:
		Instance = CreateDefaultSubobject<UPBItemTable_Char>(TEXT("CharacterTable"));
		break;
	case EPBTableType::Weapon:
		Instance = CreateDefaultSubobject<UPBItemTable_Weap>(TEXT("WeaponTable"));
		break;
	}

	return Instance;
}

void UPBItemTableManager::ApplyItemData(AActor* AppliedActor, int32 ItemID)
{
	EPBItemType ItemType = GetItemTypeBy(ItemID);

	switch (ItemType)
	{
	case EPBItemType::Character:
		ApplyItemDataToPawn(Cast<APBCharacter>(AppliedActor), ItemID);
		break;
	case EPBItemType::Weapon:
		ApplyItemDataToWeapon(Cast<APBWeapon>(AppliedActor), ItemID);
		break;
	}
}

void UPBItemTableManager::ApplyItemDataToPawn(APBCharacter* Pawn, int32 ItemID)
{
	UPBItemTable_Char* Table = Cast<UPBItemTable_Char>(GetTable(EPBTableType::Character));
	if (Table)
	{
		Table->ApplyDataTo(Pawn, ItemID);
	}
}

void UPBItemTableManager::ApplyItemDataToWeapon(APBWeapon* Weapon, int32 ItemID)
{
	UPBItemTable_Weap* Table = Cast<UPBItemTable_Weap>(GetTable(EPBTableType::Weapon));
	if (Table)
	{
		Table->ApplyDataTo(Weapon, ItemID);
	}
}

bool UPBItemTableManager::ApplyWeapItemToPawn(APBCharacter* Pawn, int32 ItemID, bool bImmediatelyEquip, int32 WantsSlotIdx)
{
	UPBItemTable_Weap* Table = Cast<UPBItemTable_Weap>(GetTable(EPBTableType::Weapon));
	if (Table)
	{
		FPBWeaponTableData* Data = Table->GetData(ItemID);
		if (Data)
		{
			//////////////////////////////////////////////////////////////////////////
			// Note :
			// Function key 를 누르면 무기가 변신한다. 이를 위해 무기 두개를 한쌍으로 미리 로드해 놓는다.
			//////////////////////////////////////////////////////////////////////////

			// Weapon Mode1 class
			FWeaponModeSetClass WeapModeSetClass;
			WeapModeSetClass.Classes.Add(Data->BPClass);

			// Weapon Mode2 class
			if (Data->WeaponModeWeaponID > 0)
			{
				FPBWeaponTableData* Data2 = Table->GetData(Data->WeaponModeWeaponID);
				if (Data2)
				{
					WeapModeSetClass.Classes.Add(Data2->BPClass);
				}
			}	

			Pawn->AddWeaponByModeSetClass(EWeaponSlot(Data->WeaponSlotEnum), WeapModeSetClass, WantsSlotIdx);

			if (bImmediatelyEquip)
			{
				Pawn->SwitchWeapon(EWeaponSlot(Data->WeaponSlotEnum), WantsSlotIdx);
			}
			return true;
		}
	}
	return false;
}

FPBWeaponTableData* UPBItemTableManager::GetWeaponTableData(int32 ItemID)
{
	if (ensure(GetItemTypeBy(ItemID) == EPBItemType::Weapon))
	{
		UPBItemTable_Weap* Table = Cast<UPBItemTable_Weap>(GetTable(EPBTableType::Weapon));
		return Table ? Table->GetData(ItemID) : nullptr;
	}
	return nullptr;
}

FPBCharacterTableData* UPBItemTableManager::GetCharacterTableData(int32 ItemID)
{
	if (ensure(GetItemTypeBy(ItemID) == EPBItemType::Character))
	{
		UPBItemTable_Char* Table = Cast<UPBItemTable_Char>(GetTable(EPBTableType::Character));
		return Table ? Table->GetData(ItemID) : nullptr;
	}
	return nullptr;
}

void UPBItemTableManager::LoadDefaultObjects()
{
	UPBTable* Table = GetTable(EPBTableType::Weapon);
	if (Table)
	{
		Table->LoadDefaultObjects();
	}
}

EPBItemType UPBItemTableManager::GetItemTypeBy(int32 ItemID)
{
	int32 ItemType = GET_ITEM_TYPE(ItemID);
	//print(FString::Printf(TEXT("ItemType:%d"), ItemType));

	return EPBItemType(ItemType);
}

EPBItemSubType UPBItemTableManager::GetItemSubTypeBy(int32 ItemID)
{
	int32 ItemSubType = GET_ITEM_SUBTYPE(ItemID);
	//print(FString::Printf(TEXT("ItemSubType:%d"), ItemSubType));

	return EPBItemSubType(ItemSubType);
}

EPBTableType UPBItemTableManager::GetTableTypeFrom(int32 ItemID)
{
	EPBItemType ItemType = GetItemTypeBy(ItemID);

	switch (ItemType)
	{
	case EPBItemType::Character:	return EPBTableType::Character;
	case EPBItemType::Weapon:		return EPBTableType::Weapon;
	}

	return EPBTableType::None;
}
