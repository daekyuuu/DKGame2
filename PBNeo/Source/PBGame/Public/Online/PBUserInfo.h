// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PBUserInfo.generated.h"

//////////////////////////////////////////////////////////////////////////
// User's Base Info
//////////////////////////////////////////////////////////////////////////
USTRUCT()
struct FPBUserBaseInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString NickName;
	UPROPERTY()
	int32 Exp;							// Experience point
	UPROPERTY()
	int32 Rank;							// Ladder Rank
	UPROPERTY()
	int32 Level;						// User's Level

	UPROPERTY() //Used RPC to server, don't delete 'UPROPERTY()'
	TArray<int32> EquippedCharItems;	// Character Item ID list of Teams
	UPROPERTY() //Used RPC to server, don't delete 'UPROPERTY()'
	TArray<int32> EquippedWeapItems;	// Primary Weapon slot Item ID list

	int32 GetCharItemID(EPBTeamType TeamType);
	int32 GetWeapItemID(EWeaponSlot Slot);

	void Clear()
	{
		Exp = 0;
		Rank = 0;
		Level = 0;
		EquippedCharItems.Empty();
		EquippedWeapItems.Empty();
	}

	bool operator ==(const FPBUserBaseInfo& InSrc);
	bool operator !=(const FPBUserBaseInfo& InSrc);
};

//////////////////////////////////////////////////////////////////////////
// User's Room Info
//////////////////////////////////////////////////////////////////////////

//enum NET_ROOM_SLOT_STATE
enum class EPBRoomSlotState
{
	Close, // Null state
	Open, // Waiting for player to enter slot
	Waiting, // Player entered slot
	Ready, // Player pressed Ready button, cannot interact with ui anymore
	Intrusion,
	Playing, // Playing game
};

enum class EPBRoomUserType
{
	Player,		// Normal player
	Bot,		// AI
	QA,			// QA 
	GM,			// Game master
	Dev,		// Developer
};

USTRUCT()
struct FPBRoomSlotBase
{
	GENERATED_BODY()

	EPBTeamType TeamType;
	int32 TeamSlotIdx;
	EPBRoomSlotState SlotState;

	FPBRoomSlotBase()
	{
		TeamType = EPBTeamType::Max;
		TeamSlotIdx = 0;
		SlotState = EPBRoomSlotState::Close;
	}

	bool IsEmptyBase() { return SlotState == EPBRoomSlotState::Close; }
	bool IsValidBase() { return ((TeamType >= EPBTeamType::Alpha && TeamType < EPBTeamType::Max) && !IsEmptyBase()); }

	void FromNetRoomSlotBase(struct NET_ROOM_SLOT_BASE& NetRoomSlotBase);
	void ToNetRoomSlotBase(struct NET_ROOM_SLOT_BASE& NetRoomSlotBase);
};

USTRUCT()
struct FPBRoomSlotInfo : public FPBRoomSlotBase
{	
	GENERATED_BODY()

	FPBUserBaseInfo UserBaseInfo;
	EPBRoomUserType UserType;

	bool bRoomMaster;
	bool bSpectator;
	bool bMine;

	FPBRoomSlotInfo()
	{
		UserType = EPBRoomUserType::Player;
		bRoomMaster = false;
		bSpectator = false;
		bMine = false;
	}

	void FromNetRoomSlotInfo(struct NET_ROOM_SLOT_INFO& NetRoomSlotInfo);
	void ToNetRoomSlotInfo(struct NET_ROOM_SLOT_INFO& NetRoomSlotInfo);
};
