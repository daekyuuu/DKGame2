// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "Online/PBRoomInfo.h"
#include "Online/PBUserInfo.h"
#include "PBRoomUserInfoManager.h"
#include "PBGameplayStatics.h"


UPBRoomUserInfoManager* UPBRoomUserInfoManager::Get(UObject* WorldContextObject)
{
	return UPBGameplayStatics::GetRoomUserInfoManager(WorldContextObject);
}

void UPBRoomUserInfoManager::EmptyUserSlots()
{
	RoomUserSlots.Empty();
}

void UPBRoomUserInfoManager::SetMySlotInfo(FPBRoomSlotInfo& SlotInfo)
{
	SlotInfo.bMine = true;
	SetUserSlotInfo(SlotInfo);
}

void UPBRoomUserInfoManager::SetUserSlotInfo(FPBRoomSlotInfo& SlotInfo)
{
	if (SlotInfo.UserBaseInfo.EquippedCharItems.Num() == 0)
	{
		SetUserCharItemsAsPersistent(SlotInfo);
		UE_LOG(LogRoomSlot, Warning, TEXT("The character item is empty. So I set it as Persistent"));
	}
		
	if (SlotInfo.UserBaseInfo.EquippedWeapItems.Num() == 0)
	{
		SetUserWeapItemsAsPersistent(SlotInfo);
		UE_LOG(LogRoomSlot, Warning, TEXT("The weapon item is empty. So I set it as Persistent"));
	}	

	RoomUserSlots.FindOrAdd(SlotInfo.TeamType).FindOrAdd(SlotInfo.TeamSlotIdx) = SlotInfo;
}

FPBRoomSlotInfo& UPBRoomUserInfoManager::GetUserSlotInfo(const FPBRoomSlotInfo& InSlotInfo)
{
	return GetUserSlotInfoByTeam(InSlotInfo.TeamType, InSlotInfo.TeamSlotIdx);
}

FPBRoomSlotInfo& UPBRoomUserInfoManager::GetUserSlotInfoByTeam(EPBTeamType TeamType, int32 TeamSlotIdx)
{
	return RoomUserSlots.FindOrAdd(TeamType).FindOrAdd(TeamSlotIdx);
}

FPBRoomSlotInfo& UPBRoomUserInfoManager::GetUserSlotInfoByName(const FString& NickName)
{
	for (auto& TeamSlots : RoomUserSlots)
	{
		for (auto& SlotData : TeamSlots.Value)
		{
			FPBRoomSlotInfo& SlotInfo = SlotData.Value;
			if (SlotInfo.UserBaseInfo.NickName == NickName)
			{
				return SlotInfo;
			}
		}
	}

	static FPBRoomSlotInfo EmptySlotInfo;
	return EmptySlotInfo;
}

TMap<uint8, FPBRoomSlotInfo>& UPBRoomUserInfoManager::GetUserTeamSlots(EPBTeamType TeamType)
{
	return RoomUserSlots.FindOrAdd(TeamType);
}

FPBRoomSlotInfo& UPBRoomUserInfoManager::GetMyUserSlotInfo()
{
	for (auto& TeamSlots : RoomUserSlots)
	{
		for (auto& SlotData : TeamSlots.Value)
		{
			FPBRoomSlotInfo& SlotInfo = SlotData.Value;
			if (SlotInfo.bMine)
			{
				return SlotInfo;
			}
		}
	}

	static FPBRoomSlotInfo EmptySlotInfo;
	return EmptySlotInfo;
}

void UPBRoomUserInfoManager::SetUserBaseInfoAsPersistent(FPBRoomSlotInfo& InSlotInfo)
{
	// My Persistent
	UPBGameInstance* GI = GetOuterUPBGameInstance();
	UPBPersistentUser* Persistent = UPBGameplayStatics::GetPersistentUserInfo(GI->GetFirstLocalPlayerController());

	if (Persistent)
	{
		InSlotInfo.UserBaseInfo = Persistent->GetBaseUserInfo();
	}
}

void UPBRoomUserInfoManager::SetUserCharItemsAsPersistent(FPBRoomSlotInfo& InSlotInfo)
{
	// My Persistent
	UPBGameInstance* GI = GetOuterUPBGameInstance();
	UPBPersistentUser* Persistent = UPBGameplayStatics::GetPersistentUserInfo(GI->GetFirstLocalPlayerController());

	if (Persistent)
	{
		InSlotInfo.UserBaseInfo.EquippedCharItems = Persistent->GetBaseUserInfo().EquippedCharItems;
	}
}

void UPBRoomUserInfoManager::SetUserWeapItemsAsPersistent(FPBRoomSlotInfo& InSlotInfo)
{
	// My Persistent
	UPBGameInstance* GI = GetOuterUPBGameInstance();
	UPBPersistentUser* Persistent = UPBGameplayStatics::GetPersistentUserInfo(GI->GetFirstLocalPlayerController());

	if (Persistent)
	{
		InSlotInfo.UserBaseInfo.EquippedWeapItems = Persistent->GetBaseUserInfo().EquippedWeapItems;
	}
}

void UPBRoomUserInfoManager::DebugSetUserCharItemsByTable(FPBRoomSlotInfo& InSlotInfo)
{
	UPBGameInstance* GI = GetOuterUPBGameInstance();	
	TArray<FPBCharacterTableData*> TableDataList;
	TArray<int32> ATeamItems;
	TArray<int32> BTeamItems;

	auto TM = GI->GetItemTableManager();
	if (TM)
	{
		TM->GetTableDataList(EPBTableType::Character, TableDataList);		

		for (auto Data : TableDataList)
		{
			if (Data && Data->BPClass)
			{
				auto Character = Cast<APBCharacter>(Data->BPClass->GetDefaultObject());
				if (Character && Data->TeamEnum == (int32)EPBTeamType::Alpha)
				{
					ATeamItems.Add(Character->GetItemID());
				}
				if (Character && Data->TeamEnum == (int32)EPBTeamType::Bravo)
				{
					BTeamItems.Add(Character->GetItemID());
				}
			}
		}

		if (ATeamItems.Num() > 0 && BTeamItems.Num() > 0)
		{
			InSlotInfo.UserBaseInfo.EquippedCharItems.Empty();

			int32 ItemID = ATeamItems[FMath::Rand() % ATeamItems.Num()];
			InSlotInfo.UserBaseInfo.EquippedCharItems.AddUnique(ItemID);

			ItemID = BTeamItems[FMath::Rand() % BTeamItems.Num()];
			InSlotInfo.UserBaseInfo.EquippedCharItems.AddUnique(ItemID);
		}
	}
}

void UPBRoomUserInfoManager::DebugSetUserWeapItemsByTable(FPBRoomSlotInfo& InSlotInfo)
{
	UPBGameInstance* GI = GetOuterUPBGameInstance();
	TArray<FPBWeaponTableData*> TableDataList;
	TArray<int32> Items;

	auto TM = GI->GetItemTableManager();
	if (TM)
	{
		TM->GetTableDataList(EPBTableType::Weapon, TableDataList);		

		for (auto Data : TableDataList)
		{
			if (Data && Data->BPClass)
			{
				auto Weapon = Cast<APBWeapon>(Data->BPClass->GetDefaultObject());
				if (Weapon && Data->WeaponSlotEnum == (int32)EWeaponSlot::Primary)
				{
					Items.Add(Weapon->GetItemID());
				}
			}
		}

		if (Items.Num() > 0)
		{
			InSlotInfo.UserBaseInfo.EquippedWeapItems.Empty();

			int32 ItemID = Items[FMath::Rand() % Items.Num()];
			InSlotInfo.UserBaseInfo.EquippedWeapItems.AddUnique(ItemID);
		}
	}
}

void UPBRoomUserInfoManager::DebugAddRoomTeamSlots(int32 MaxSlotNum)
{
	MaxSlotNum = FMath::Clamp(MaxSlotNum, 1, 8);

	// My slot info
	FPBRoomSlotInfo& MyRoomSlotInfo = GetMyUserSlotInfo();
	int32 MySlotIdx = MyRoomSlotInfo.IsValidBase() ? MyRoomSlotInfo.TeamSlotIdx : -1;
	EPBTeamType TeamType = MyRoomSlotInfo.IsValidBase() ? MyRoomSlotInfo.TeamType : EPBTeamType::Alpha;
	
	// My Persistent
	UPBGameInstance* GI = GetOuterUPBGameInstance();
	UPBPersistentUser* Persistent = UPBGameplayStatics::GetPersistentUserInfo(GI->GetFirstLocalPlayerController());

	// Add slot info
	for (int32 TeamSlotIdx = 0; TeamSlotIdx < MaxSlotNum; TeamSlotIdx++)
	{
		if (MySlotIdx == TeamSlotIdx) continue;
		FPBRoomSlotInfo& ExistUserInfo = GetUserSlotInfoByTeam(TeamType, TeamSlotIdx);
		if (ExistUserInfo.IsValidBase()) continue;
		
		FPBRoomSlotInfo RoomSlotInfo;
		RoomSlotInfo.SlotState = EPBRoomSlotState::Waiting;
		RoomSlotInfo.TeamSlotIdx = TeamSlotIdx;
		RoomSlotInfo.TeamType = TeamType;

		if (MyRoomSlotInfo.IsValidBase())
		{
			RoomSlotInfo.UserBaseInfo = MyRoomSlotInfo.UserBaseInfo;
			RoomSlotInfo.UserBaseInfo.NickName = FString::Printf(TEXT("%s_%d"), *MyRoomSlotInfo.UserBaseInfo.NickName, TeamSlotIdx);
		}
		else
		{
			FPBUserBaseInfo& PersistentUserBaseInfo = Persistent->GetBaseUserInfo();
			RoomSlotInfo.UserBaseInfo = PersistentUserBaseInfo;
			RoomSlotInfo.UserBaseInfo.NickName = FString::Printf(TEXT("%s_%d"), *PersistentUserBaseInfo.NickName, TeamSlotIdx);
		}

		DebugSetUserCharItemsByTable(RoomSlotInfo);
		DebugSetUserWeapItemsByTable(RoomSlotInfo);

		SetUserSlotInfo(RoomSlotInfo);
	}	

	OnCurrentRoomDataUpdatedEvt.Broadcast();
}
