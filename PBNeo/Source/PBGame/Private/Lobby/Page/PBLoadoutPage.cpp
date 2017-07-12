// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBLoadoutPage.h"
#include "PBGameUserSettings.h"
#include "PBItemTableManager.h"
#include "PBItemTable_Char.h"
#include "PBGameInstance.h"
#include "PBCharacterScene.h"
#include "PBCharacter.h"
#include "PBMVCharacter.h"
#include "PBPersistentUser.h"
#include "PBGameplayStatics.h"
#include "PBItemTable_Weap.h"
#include "PBTableManager.h"
#include "PBLobby.h"

void UPBLoadoutPage::Init(class APlayerController* OwningPlayer, TSubclassOf<class UPBMainWidget> HUDWidgetClass)
{
	Super::Init(OwningPlayer, HUDWidgetClass);
}


void UPBLoadoutPage::Enter()
{
	Super::Enter();
}

void UPBLoadoutPage::Exit()
{
	Super::Exit();
}

void UPBLoadoutPage::OnSoldierIsSelected(int32 Id)
{
	//print("UPBLoadoutPage::OnSoldierHasSelected: " + FString::FromInt(Id));

	SetCharacterIdToPersistanceUser(Id);
	GetPersistentUser()->SaveIfDirty();

	SoldierSelectEvt.Broadcast(Id);

}

void UPBLoadoutPage::OnWeapIsSelected(int32 Id)
{
	//print("UPBLoadoutPage::OnWeapHasSelected: " + FString::FromInt(Id));

	SetWeapIdToPersistanceUser(Id);
	GetPersistentUser()->SaveIfDirty();

	WeapSelectEvt.Broadcast(Id);

}

void UPBLoadoutPage::RequestSoldierList(EPBTeamType Type)
{
	// TODO: Request solider list to real server
	TArray<FPBItemBaseInfo> TempSoldierList;
	
	TArray<int32> CharIds;
	GetAllCharIds_Deprecated(Type, CharIds);
	
	FPBItemBaseInfo Info;
	int32 SavedChar = UPBGameplayStatics::GetSavedCharId_Deprecated(GetOwningLocalPlayer(), Type);

	for (auto Id : CharIds)
	{
		Info.ItemId = Id;
		Info.bEquipped = (SavedChar != 0 && SavedChar == Info.ItemId) ? true : false;
		TempSoldierList.Add(Info);
	}

	OnSoldierListIsReceived(Type, TempSoldierList);

}



void UPBLoadoutPage::OnSoldierListIsReceived(EPBTeamType Type, const TArray<FPBItemBaseInfo>& SoldierList)
{
	SoldierListIsReceivedEvt.Broadcast(Type, SoldierList);
}

void UPBLoadoutPage::RequestWeapList(EWeaponType Type)
{
	// TODO: Request Weap list to real server
	
	TArray<int32> WeapIds;
	GetAllWeapIds_Deprecated(Type, WeapIds);
	

	auto SavedWeapIds = UPBGameplayStatics::GetSavedWeapIds_Deprecated(LocalPlayer);
	TArray<FPBItemBaseInfo> TempWeapList;

	for (int32 Id : WeapIds)
	{
		FPBItemBaseInfo Info;

		Info.ItemId = Id;
		Info.bEquipped = IsThereSameId(SavedWeapIds, Info.ItemId) ? true : false;
		TempWeapList.Add(Info);
	}

	OnWeapListIsReceived(Type, TempWeapList);

}

void UPBLoadoutPage::OnWeapListIsReceived(EWeaponType Type, const TArray<FPBItemBaseInfo>& WeapList)
{
	WeapListIsReceivedEvt.Broadcast(Type, WeapList);
}


void UPBLoadoutPage::GetAllCharIds_Deprecated(EPBTeamType Type, TArray<int32>& OutCharIds)
{
	OutCharIds.Empty();
	auto TM = UPBGameplayStatics::GetItemTableManager(GetOwningLocalPlayer());
	if (nullptr == TM)
	{
		return;
	}

	auto CharTable = TM->GetTable(EPBTableType::Character);
	if (nullptr == CharTable)
	{
		return;
	}

	TArray<int32> ItemIds;
	CharTable->GetAllItemIds(ItemIds);

	for (auto id : ItemIds)
	{
		auto CharData = TM->GetCharacterTableData(id);
		if (CharData && CharData->BPClass)
		{
			if (CharData->TeamEnum == (int32)Type)
			{
				OutCharIds.Add(id);
			}
		}
	}
}

void UPBLoadoutPage::GetAllWeapIds_Deprecated(EWeaponType Type, TArray<int32>& OutWeapIds)
{
	OutWeapIds.Empty();
	auto TM = UPBGameplayStatics::GetItemTableManager(GetOwningLocalPlayer());
	if (nullptr == TM)
	{
		return;
	}

	auto WeapTable = TM->GetTable(EPBTableType::Weapon);
	if (nullptr == WeapTable)
	{
		return;
	}

	TArray<int32> ItemIds;
	WeapTable->GetAllItemIds(ItemIds);

	for (auto id : ItemIds)
	{
		auto WeapData = TM->GetWeaponTableData(id);
		if (WeapData && WeapData->BPClass)
		{
			if (WeapData->WeaponTypeEnum == (int32)Type)
			{
				OutWeapIds.Add(id);
			}
		}
	}
}




int32 UPBLoadoutPage::GetWeapSlotFromId(int32 Id)
{
	auto Mgr = UPBGameplayStatics::GetItemTableManager(GetOwningLocalPlayer());
	if (Mgr)
	{
		auto Data = Mgr->GetWeaponTableData(Id);
		if (Data)
		{
			return Data->WeaponSlotEnum;
		}
	}

	return -1;
}

void UPBLoadoutPage::SetCharacterIdToPersistanceUser(int32 Id)
{
	auto PBPersist = GetPersistentUser();

	if (PBPersist)
	{
		auto BaseUserInfo = PBPersist->GetBaseUserInfo();
		auto& CharItems = BaseUserInfo.EquippedCharItems;

		// Resize the array to prevent access violation.
		CharItems.SetNumZeroed((int32)EPBTeamType::Max);

		int32 TeamType = (int32)UPBGameplayStatics::GetTeamTypeFromCharId(GetOwningLocalPlayer(), Id);

		if (TeamType >= 0 &&
			TeamType < (int32)EPBTeamType::Max &&
			CharItems.IsValidIndex(TeamType)
			)
		{
			CharItems[TeamType] = Id;
			PBPersist->SetBaseUserInfo(BaseUserInfo);
		}
		else
		{
			UE_LOG(LogUI, Warning, TEXT("UPBLoadoutPage::SetCharacterIdToUserSetting: Invalid Team Enum from Id:%d"), Id);
		}
	}
}



void UPBLoadoutPage::SetWeapIdToPersistanceUser(int32 Id)
{
	auto PBPersist = GetPersistentUser();
	if (PBPersist)
	{
		auto BaseUserInfo = PBPersist->GetBaseUserInfo();
		auto& WeapItems = BaseUserInfo.EquippedWeapItems;

		WeapItems.SetNumZeroed((int32)EWeaponSlot::Max);

		int32 WeapSlot = GetWeapSlotFromId(Id);

		if (WeapSlot >= (int32)EWeaponSlot::Primary &&
			WeapSlot < (int32)EWeaponSlot::Max &&
			WeapItems.IsValidIndex(WeapSlot)
			)
		{
			WeapItems[WeapSlot] = Id;
			PBPersist->SetBaseUserInfo(BaseUserInfo);
		}
		else
		{
			UE_LOG(LogUI, Warning, TEXT("UPBLoadoutPage::SetWeapIdToUserSetting: Invalid WeapSlot index from Id:%d"), Id);
		}
	}
}



bool UPBLoadoutPage::IsThereSameId(const TArray<int32>& IdArray, int32 CompareId)
{
	int32 Size = IdArray.Num();

	if (0 == CompareId)
	{
		return false;
	}


	for (int i = 0; i < Size; ++i)
	{
		if (IdArray[i] == CompareId)
		{
			return true;
		}
	}

	return false;
}


/* ------------------------------------------------------------------------------- */
// NEW LOBBY
/* ------------------------------------------------------------------------------- */


TArray<int32> UPBLoadoutPage::GetPresetItemsAtSlot(int32 SlotIndex)
{

	// seungyoon.ji
	// TODO: Use the data acquired from the server.

	TArray<int32> ItemLists;
	ItemLists.SetNumZeroed((int32)EWeaponSlot::Max);
	switch (SlotIndex)
	{
	case 0:
		ItemLists[(int32)EWeaponSlot::Primary] = 30101000;
		ItemLists[(int32)EWeaponSlot::Secondary] = 30599000;
		ItemLists[(int32)EWeaponSlot::Grenade] = 30901000;
		ItemLists[(int32)EWeaponSlot::Special] = 31301000;

		break;

	case 1:
		ItemLists[(int32)EWeaponSlot::Primary] = 30102000;
		ItemLists[(int32)EWeaponSlot::Secondary] = 30501000;
		ItemLists[(int32)EWeaponSlot::Grenade] = 30902000;
		ItemLists[(int32)EWeaponSlot::Special] = 31301000;
		break;

	case 2:
		ItemLists[(int32)EWeaponSlot::Primary] = 30103000;
		ItemLists[(int32)EWeaponSlot::Secondary] = 30502000;
		ItemLists[(int32)EWeaponSlot::Grenade] = 30903000;
		ItemLists[(int32)EWeaponSlot::Special] = 31301000;
		break;

	case 3:
		ItemLists[(int32)EWeaponSlot::Primary] = 30104000;
		ItemLists[(int32)EWeaponSlot::Secondary] = 30503000;
		ItemLists[(int32)EWeaponSlot::Grenade] = 30901000;
		ItemLists[(int32)EWeaponSlot::Special] = 31301000;
		break;

	case 4:
		ItemLists[(int32)EWeaponSlot::Primary] = 30105000;
		ItemLists[(int32)EWeaponSlot::Secondary] = 30504000;
		ItemLists[(int32)EWeaponSlot::Grenade] = 30901000;
		ItemLists[(int32)EWeaponSlot::Special] = 31301000;
		break;

	default:
		ItemLists[(int32)EWeaponSlot::Primary] = 30101000;
		ItemLists[(int32)EWeaponSlot::Secondary] = 30599000;
		ItemLists[(int32)EWeaponSlot::Grenade] = 30901000;
		ItemLists[(int32)EWeaponSlot::Special] = 31301000;
		break;
	}

	return ItemLists;

}

TArray<int32> UPBLoadoutPage::GetWeaponListHasSameTypeFromId(int32 ItemId)
{
	TArray<int32> OutWeapList;

	 auto TM = UPBGameplayStatics::GetItemTableManager(GetOwningLocalPlayer());
	 if (TM)
	 {
		 auto WeaponData = TM->GetWeaponTableData(ItemId);
		 if (WeaponData)
		 {
			 auto WeaponType = (EWeaponType)WeaponData->WeaponTypeEnum;
			 GetAllWeapIds_Deprecated(WeaponType, OutWeapList);
		 }
	 }

	 return OutWeapList;

}

TArray<int32> UPBLoadoutPage::GetWeaponListHasSameType(EWeaponType Type)
{
	TArray<int32> OutWeapList;
	GetAllWeapIds_Deprecated(Type, OutWeapList);

	return OutWeapList;
}

void UPBLoadoutPage::GetWeaponTypeAndSlotFromId(int32 ItemId, EWeaponType& OutWeaponType, EWeaponSlot& OutWeaponSlot)
{
	auto TM = UPBGameplayStatics::GetItemTableManager(GetOwningLocalPlayer());
	if (TM)
	{
		auto WeaponData = TM->GetWeaponTableData(ItemId);
		if (WeaponData)
		{
			OutWeaponType = (EWeaponType)WeaponData->WeaponTypeEnum;
			OutWeaponSlot = (EWeaponSlot)WeaponData->WeaponSlotEnum;

		}
	}

}
