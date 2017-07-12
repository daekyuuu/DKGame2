// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBUserInfo.h"

bool FPBUserBaseInfo::operator ==(const FPBUserBaseInfo& InSrc)
{
	if (this->Rank != InSrc.Rank) return false;
	if (this->NickName != InSrc.NickName) return false;
	if (this->Level != InSrc.Level) return false;
	if (this->Exp != InSrc.Exp) return false;
	if (this->EquippedCharItems != InSrc.EquippedCharItems) return false;
	if (this->EquippedWeapItems != InSrc.EquippedWeapItems) return false;
	
	return true;
}

bool FPBUserBaseInfo::operator !=(const FPBUserBaseInfo& InSrc)
{
	return !(*this == InSrc);
}

int32 FPBUserBaseInfo::GetCharItemID(EPBTeamType TeamType)
{
	if (EquippedCharItems.IsValidIndex((int32)TeamType))
	{
		return EquippedCharItems[(int32)TeamType];
	}
	return 0;
}

int32 FPBUserBaseInfo::GetWeapItemID(EWeaponSlot Slot)
{
	if (EquippedWeapItems.IsValidIndex((int32)Slot))
	{
		return EquippedWeapItems[(int32)Slot];
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////

void FPBRoomSlotBase::FromNetRoomSlotBase(NET_ROOM_SLOT_BASE& NetRoomSlotBase)
{
	TeamType = (EPBTeamType)(int8)NetRoomSlotBase.m_i8TeamIdx;
	TeamSlotIdx = (int32)NetRoomSlotBase.m_i8SlotIdx;
	SlotState = (EPBRoomSlotState)(int8)NetRoomSlotBase.m_i8SlotState;
}

void FPBRoomSlotBase::ToNetRoomSlotBase(NET_ROOM_SLOT_BASE& NetRoomSlotBase)
{
	NetRoomSlotBase.m_i8TeamIdx = (int8)TeamType;
	NetRoomSlotBase.m_i8SlotIdx = (int8)TeamSlotIdx;
	NetRoomSlotBase.m_i8SlotState = (int8)SlotState;
}

void FPBRoomSlotInfo::FromNetRoomSlotInfo(NET_ROOM_SLOT_INFO& NetRoomSlotInfo)
{
	TeamType = (EPBTeamType)(int8)NetRoomSlotInfo.m_i8TeamIdx;
	TeamSlotIdx = (int32)NetRoomSlotInfo.m_i8SlotIdx;
	SlotState = (EPBRoomSlotState)(int8)NetRoomSlotInfo.m_i8SlotState;
	UPBPacketManager::S2MOStringToUnrealString(NetRoomSlotInfo.m_strNickname, UserBaseInfo.NickName);
	UserBaseInfo.Level = (int32)NetRoomSlotInfo.m_ui16Level;
}

void FPBRoomSlotInfo::ToNetRoomSlotInfo(NET_ROOM_SLOT_INFO& NetRoomSlotInfo)
{
	NetRoomSlotInfo.m_i8TeamIdx = (int8)TeamType;
	NetRoomSlotInfo.m_i8SlotIdx = (int8)TeamSlotIdx;
	NetRoomSlotInfo.m_i8SlotState = (int8)SlotState;
	UPBPacketManager::UnrealStringToS2MOString(UserBaseInfo.NickName, NetRoomSlotInfo.m_strNickname);
	NetRoomSlotInfo.m_ui16Level = (uint16)UserBaseInfo.Level;
}
