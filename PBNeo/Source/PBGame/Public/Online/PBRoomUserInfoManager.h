// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Online/PBUserInfo.h"
#include "PBTypes.h"
#include "PBRoomUserInfoManager.generated.h"

UCLASS(Within = PBGameInstance)
class PBGAME_API UPBRoomUserInfoManager : public UObject
{
	GENERATED_BODY()

public:

	static UPBRoomUserInfoManager* Get(UObject* WorldContextObject);

	typedef TMap<EPBTeamType, TMap<uint8, FPBRoomSlotInfo>> ROOM_SLOT_INFOS;

	void EmptyUserSlots();

	void SetMySlotInfo(FPBRoomSlotInfo& SlotInfo);
	void SetUserSlotInfo(FPBRoomSlotInfo& SlotInfo);

	FPBRoomSlotInfo& GetMyUserSlotInfo();

	FPBRoomSlotInfo& GetUserSlotInfo(const FPBRoomSlotInfo& InSlotInfo);
	FPBRoomSlotInfo& GetUserSlotInfoByTeam(EPBTeamType TeamType, int32 TeamSlotIdx);
	FPBRoomSlotInfo& GetUserSlotInfoByName(const FString& NickName);

	TMap<uint8, FPBRoomSlotInfo>& GetUserTeamSlots(EPBTeamType TeamType);

	ROOM_SLOT_INFOS& GetUserSlots() { return RoomUserSlots; }
	int32 GetUserSlotsNum() { return RoomUserSlots.Num(); }

public:

	void SetUserBaseInfoAsPersistent(FPBRoomSlotInfo& InSlotInfo);
	void SetUserCharItemsAsPersistent(FPBRoomSlotInfo& InSlotInfo);
	void SetUserWeapItemsAsPersistent(FPBRoomSlotInfo& InSlotInfo);

	void DebugSetUserCharItemsByTable(FPBRoomSlotInfo& InSlotInfo);
	void DebugSetUserWeapItemsByTable(FPBRoomSlotInfo& InSlotInfo);

	void DebugAddRoomTeamSlots(int32 MaxSlotNum);
	
public:
	DECLARE_EVENT(UPBRoomUserInfoManager, FOnCurrentRoomUpdatedEvt)
	FOnCurrentRoomUpdatedEvt OnCurrentRoomDataUpdatedEvt;

private:

	ROOM_SLOT_INFOS RoomUserSlots;
};
