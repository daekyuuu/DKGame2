// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Net/S2MODefine.h"


enum class EPBRoomState : uint8
{
	Close, // Null state
	Open, // Room waiting for player to enter
	Ready, // Triggered as someone enters room, count down from 60s to 10s
	Countdown, // Count down from 10s, no one can do anything
	Playing, // Playing game
};

struct NET_ROOM_INFO;
struct FPBRoomInfo
{
	uint32 RoomIdx;
	FString Title;
	EPBGameModeType GameMode;
	uint8 CurrentUserNum;
	uint8 MaxUserNum;
	EPBRoomState RoomState;

	FPBRoomInfo()
	{
		RoomIdx = 0;
		Title = FString("Room-Null");
		GameMode = EPBGameModeType::Max;
		CurrentUserNum = 0;
		MaxUserNum = 0;
		RoomState = EPBRoomState::Close;
	}

	void FromNetRoomInfo(NET_ROOM_INFO& NetRoomInfo);
	void ToNetRoomInfo(NET_ROOM_INFO& NetRoomInfo);
};

struct FPBRoomVoteBoardInfo
{
	int8 SecondsTillGameStart;
	TMap<uint32, uint8> MapIdxToVoteCount;

	FPBRoomVoteBoardInfo()
	{
		SecondsTillGameStart = 0;
	}
};
