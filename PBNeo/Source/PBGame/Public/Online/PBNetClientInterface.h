// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "UObject/NoExportTypes.h"
#include "Object.h"
#include "Net/S2MODefine.h"
#include "Online/PBRoomInfo.h"
#include "Online/PBUserInfo.h"
#include "PBNetClientInterface.generated.h"

enum class EPBNetClientState : uint8
{
	NotConnected,
	Connected,
	LoggedIn,
};

class UPBGameInstance;

UCLASS(config = Game, Within = PBGameInstance)
class PBGAME_API UPBNetClientInterface : public UObject
{
	GENERATED_BODY()

public:	

	static UPBNetClientInterface* Get(UObject* WorldContextObject);

	UPBNetClientInterface();
	void Init();
	FTimerHandle SlowTick_Handle;
	void SlowTick();
	void ShutDown();

	EPBNetClientState GetState();
	FPBRoomInfo& GetCurrentRoomInfo();
	FPBRoomVoteBoardInfo& GetCurrentRoomVoteBoardInfo();
	EPBTeamType GetOwningPlayerTeam();
	uint8 GetOwningPlayerSlotIdx();
	TMap<EPBTeamType, TMap<uint8, FPBRoomSlotInfo>>& GetCurrentRoomSlotsInfo();

	bool ConnectToServer(FString IP, uint16 port);
	bool Login(FString ID, FString Password);
	bool GetUserInfo();
	bool UpdateNickname(FString NickName);

	bool CreateRoom(FString RoomTitle, EPBGameModeType GameMode, uint16 RoundTimeSec, uint8 MaxMemberPerTeam);
	bool FindRoom();
	bool EnterRoom(uint32 RoomIdx);
	bool VoteMap(uint32 MapIdx);
	bool DeclareSelfReady();
	bool TravelToGame(TArray<FPBRoomSlotInfo> StartingPlayers, EPBTeamType OwnerTeam, uint32 MapIdx, FString DediAddr);
	bool LeaveCurrentRoom();
	bool BattleEnd();

	DECLARE_EVENT_TwoParams(UPBNetClientInterface, FOnStateChangedEvt, EPBNetClientState /* OldState */, EPBNetClientState /* NewState */)
	FOnStateChangedEvt OnStateChangedEvt;

	// TODO: Show more user info...
	DECLARE_EVENT_OneParam(UPBNetClientInterface, FOnGotUserInfoEvt, bool /*NeedUpdateNickname*/)
	FOnGotUserInfoEvt OnGotUserInfoEvt;

	DECLARE_EVENT_OneParam(UPBNetClientInterface, FOnRoomCreatedEvt, uint32 /* RoomIdx */)
	FOnRoomCreatedEvt OnRoomCreatedEvt;

	DECLARE_EVENT_OneParam(UPBNetClientInterface, FOnRoomListEvt, TArray<FPBRoomInfo> /* Rooms */)
	FOnRoomListEvt OnRoomListEvt;

	DECLARE_EVENT_OneParam(UPBNetClientInterface, FOnEnterRoomEvt, bool /*Success*/)
	FOnEnterRoomEvt OnEnterRoomEvt;

	DECLARE_EVENT(UPBNetClientInterface, FOnCurrentRoomUpdatedEvt)
	FOnCurrentRoomUpdatedEvt OnCurrentRoomDataUpdatedEvt;

	DECLARE_EVENT(UPBNetClientInterface, FBattleStartCountdownEvt)
	FBattleStartCountdownEvt OnBattleStartCountdownEvt;

	DECLARE_EVENT_FourParams(UPBNetClientInterface, FOnBattleStartEvt, TArray<FPBRoomSlotInfo> /*StartingPlayers*/, EPBTeamType /*MyTeam*/, uint32/* MapIdx*/, FString /*DediAddr*/)
	FOnBattleStartEvt OnBattleStartEvt;

	DECLARE_EVENT(UPBNetClientInterface, FOnLeaveRoomEvt)
	FOnLeaveRoomEvt OnLeaveRoomEvt;

	DECLARE_EVENT_OneParam(UPBNetClientInterface, FOnNoticeEvt, const FString & /*Mesasge*/)
	FOnNoticeEvt OnNoticeEvt;
protected:

	FDelegateHandle HandleRecvEvent_Handle;
	void HandleRecvEvent(struct FPacketRecvEvent RecvEvent);

	EPBNetClientState ClientState;

	FPBRoomInfo CurrentRoomInfo;
	FPBRoomVoteBoardInfo CurrentVoteBoardInfo;

	EPBTeamType OwningPlayerTeam;
	uint8 OwningPlayerIdx;

	// Temp
	UPROPERTY(config)
	FString TempDediAddress;
};
