// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBRoomInfo.h"


void FPBRoomInfo::FromNetRoomInfo(NET_ROOM_INFO& NetRoomInfo)
{
	RoomIdx = NetRoomInfo.m_ui32RoomIdx;
	UPBPacketManager::S2MOStringToUnrealString(NetRoomInfo.m_strTitle, Title);
	GameMode = (EPBGameModeType)(int8)NetRoomInfo.m_i8RoomMode;
	CurrentUserNum = NetRoomInfo.m_ui8UserCountCur;
	MaxUserNum = NetRoomInfo.m_ui8UserCountMax;
	RoomState = (EPBRoomState)(uint8)NetRoomInfo.m_ui8State;
}

void FPBRoomInfo::ToNetRoomInfo(NET_ROOM_INFO& NetRoomInfo)
{
	NetRoomInfo.m_ui32RoomIdx = RoomIdx;
	UPBPacketManager::UnrealStringToS2MOString(Title, NetRoomInfo.m_strTitle);
	NetRoomInfo.m_i8RoomMode = (int8)GameMode;
	NetRoomInfo.m_ui8UserCountCur = CurrentUserNum;
	NetRoomInfo.m_ui8UserCountMax = MaxUserNum;
	NetRoomInfo.m_ui8State = (uint8)RoomState;
}
