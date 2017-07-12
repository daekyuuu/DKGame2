// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBNetClientInterface.h"
#include "PBClientPacketManager.h"
#include "Online/PBRoomUserInfoManager.h"

UPBNetClientInterface* UPBNetClientInterface::Get(UObject* WorldContextObject)
{
	return UPBGameplayStatics::GetNetClientInterface(WorldContextObject);
}

UPBNetClientInterface::UPBNetClientInterface()
{
	ClientState = EPBNetClientState::NotConnected;
}

void UPBNetClientInterface::Init()
{
	UPBPacketManager* PM = UPBPacketManager::Get(GetOuter());
	if (PM)
	{
		HandleRecvEvent_Handle = PM->OnRecvEvent.AddUObject(this, &UPBNetClientInterface::HandleRecvEvent);
	}

	if (GetOuter() && GetOuter()->GetWorld())
	{
		GetOuter()->GetWorld()->GetTimerManager().SetTimer(SlowTick_Handle, this, &UPBNetClientInterface::SlowTick, 1.f, true);
	}
}

void UPBNetClientInterface::SlowTick()
{
	if (CurrentRoomInfo.RoomState == EPBRoomState::Ready || CurrentRoomInfo.RoomState == EPBRoomState::Countdown)
	{
		if (CurrentVoteBoardInfo.SecondsTillGameStart > 0)
		{
			--CurrentVoteBoardInfo.SecondsTillGameStart;
			OnCurrentRoomDataUpdatedEvt.Broadcast();
		}
	}
}

void UPBNetClientInterface::ShutDown()
{
	if (GetOuter() && GetOuter()->GetWorld())
	{
		GetOuter()->GetWorld()->GetTimerManager().ClearTimer(SlowTick_Handle);
	}

	UPBPacketManager* PM = UPBPacketManager::Get(GetOuter());
	if (PM)
	{
		if (HandleRecvEvent_Handle.IsValid())
		{
			PM->OnRecvEvent.Remove(HandleRecvEvent_Handle);
		}
	}
}

EPBNetClientState UPBNetClientInterface::GetState()
{
	return ClientState;
}

FPBRoomInfo& UPBNetClientInterface::GetCurrentRoomInfo()
{
	return CurrentRoomInfo;
}

FPBRoomVoteBoardInfo& UPBNetClientInterface::GetCurrentRoomVoteBoardInfo()
{
	return CurrentVoteBoardInfo;
}

EPBTeamType UPBNetClientInterface::GetOwningPlayerTeam()
{
	return OwningPlayerTeam;
}

uint8 UPBNetClientInterface::GetOwningPlayerSlotIdx()
{
	return OwningPlayerIdx;
}

UPBRoomUserInfoManager::ROOM_SLOT_INFOS& UPBNetClientInterface::GetCurrentRoomSlotsInfo()
{
	auto RoomUserManager = UPBRoomUserInfoManager::Get(GetOuter());

	static UPBRoomUserInfoManager::ROOM_SLOT_INFOS EmptySlots;
	return RoomUserManager ? RoomUserManager->GetUserSlots() : EmptySlots;
}

bool UPBNetClientInterface::ConnectToServer(FString IP, uint16 port)
{
	UPBPacketManager* PM = UPBPacketManager::Get(GetOuter());
	if (PM)
	{
		PacketConnectReq Send;
		UPBPacketManager::UnrealStringToS2MOString(IP, Send.m_strIP);
		Send.m_ui16Port = port;
		return PM->SendEvent(&Send);
	}

	return false;
}

bool UPBNetClientInterface::Login(FString ID, FString Password)
{
	UPBPacketManager* PM = UPBPacketManager::Get(GetOuter());
	if (PM)
	{
		PacketLoginReq Send;
		UPBPacketManager::UnrealStringToS2MOString(ID, Send.m_strID);
		UPBPacketManager::UnrealStringToS2MOString(Password, Send.m_strPW);
		return PM->SendEvent(&Send);
	}

	return false;
}

bool UPBNetClientInterface::GetUserInfo()
{
	UPBPacketManager* PM = UPBPacketManager::Get(GetOuter());
	if (PM)
	{
		PacketUserInfoLoadReq Send;
		return PM->SendEvent(&Send);
	}

	return false;
}

bool UPBNetClientInterface::UpdateNickname(FString NickName)
{
	UPBPacketManager* PM = UPBPacketManager::Get(GetOuter());
	if (PM)
	{
		PacketUserChangeNickReq Send;
		UPBPacketManager::UnrealStringToS2MOString(NickName, Send.m_strNickname);
		return PM->SendEvent(&Send);
	}

	return false;
}

bool UPBNetClientInterface::CreateRoom(FString RoomTitle, EPBGameModeType GameMode, uint16 RoundTimeSec, uint8 MaxMemberPerTeam)
{
	UPBPacketManager* PM = UPBPacketManager::Get(GetOuter());
	if (PM)
	{
		PacketRoomCreateReq Send;
		UPBPacketManager::UnrealStringToS2MOString(RoomTitle, Send.m_RoomCreateInfo.m_strTitle);
		Send.m_RoomCreateInfo.m_i8RoomMode = (uint8)GameMode;
		Send.m_RoomCreateInfo.m_ui16TimeToRoundSec = RoundTimeSec;
		Send.m_RoomCreateInfo.m_ui8MemberCountOfTeam = MaxMemberPerTeam;
		return PM->SendEvent(&Send);
	}

	return false;
}

bool UPBNetClientInterface::FindRoom()
{
	UPBPacketManager* PM = UPBPacketManager::Get(GetOuter());
	if (PM)
	{
		PacketRoomListReq Send;
		return PM->SendEvent(&Send);
	}

	return false;
}

bool UPBNetClientInterface::EnterRoom(uint32 RoomIdx)
{
	UPBPacketManager* PM = UPBPacketManager::Get(GetOuter());
	if (PM)
	{
		PacketRoomEnterReq Send;
		Send.m_ui32RoomIdx = RoomIdx;
		return PM->SendEvent(&Send);
	}

	return false;
}

bool UPBNetClientInterface::VoteMap(uint32 MapIdx)
{
	UPBPacketManager* PM = UPBPacketManager::Get(GetOuter());
	if (PM)
	{
		PacketRoomMapVoteReq Send;
		Send.m_ui32MapIdx = MapIdx;
		return PM->SendEvent(&Send);
	}

	return false;
}

bool UPBNetClientInterface::DeclareSelfReady()
{
	UPBPacketManager* PM = UPBPacketManager::Get(GetOuter());
	if (PM)
	{
		PacketRoomSlotReadyReq Send;
		return PM->SendEvent(&Send);
	}

	return false;
}

bool UPBNetClientInterface::TravelToGame(TArray<FPBRoomSlotInfo> StartingPlayers, EPBTeamType OwnerTeam, uint32 MapIdx, FString DediAddr)
{
	UPBGameInstance* GI = GetOuterUPBGameInstance();
	if (GI)
	{
		FString NickName;

		auto UserManager = UPBRoomUserInfoManager::Get(GetOuter());
		if (UserManager)
		{
			FPBRoomSlotInfo& MySlotInfo = UserManager->GetMyUserSlotInfo();
			if (MySlotInfo.IsValidBase())
			{
				NickName = MySlotInfo.UserBaseInfo.NickName;
			}
		}

		if (OwningPlayerTeam == EPBTeamType::Alpha || OwningPlayerTeam == EPBTeamType::Bravo)
		{
			FString TargetURL = DediAddr + FString(" ?") + APBGameMode::GetTeamOptionName() + FString("=") + FString::FromInt((uint8)OwnerTeam);
			TargetURL = TargetURL + FString(" ?") + FString("Name=") + NickName;

			return GI->EnterGame(TargetURL, MapIdx);
		}

		return GI->EnterGame(DediAddr, MapIdx);
	}

	return false;
}

bool UPBNetClientInterface::LeaveCurrentRoom()
{
	UPBPacketManager* PM = UPBPacketManager::Get(GetOuter());
	if (PM)
	{
		PacketRoomLeaveReq Send;
		return PM->SendEvent(&Send);
	}

	return false;
}

bool UPBNetClientInterface::BattleEnd()
{
	UPBPacketManager* PM = UPBPacketManager::Get(GetOuter());
	if (PM)
	{
		PacketBattleEndReq Send;
		return PM->SendEvent(&Send);
	}

	return false;
}

void UPBNetClientInterface::HandleRecvEvent(FPacketRecvEvent RecvEvent)
{
	switch (RecvEvent.TProtocol)
	{
	case PROTOCOL_BASE_CONNECT_ACK:
	{
		PacketConnectAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);
		if (Ack.m_TResult == NET_RESULT_SUCCESS)
		{
			EPBNetClientState OldClientState = ClientState;
			ClientState = EPBNetClientState::Connected;
			if (OldClientState != ClientState)
			{
				OnStateChangedEvt.Broadcast(OldClientState, ClientState);
			}
		}
		break;
	}

	case PROTOCOL_LOGIN_INPUT_ACK:
	{
		PacketLoginAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);
		if (Ack.m_TResult == NET_RESULT_SUCCESS)
		{
			EPBNetClientState OldClientState = ClientState;
			ClientState = EPBNetClientState::LoggedIn;
			if (OldClientState != ClientState)
			{
				OnStateChangedEvt.Broadcast(OldClientState, ClientState);
			}
		}
		break;
	}

	case PROTOCOL_USER_INFO_LOAD_ACK:
	{
		PacketDisConnectAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);
		if (Ack.m_TResult == NET_RESULT_SUCCESS)
		{
			OnGotUserInfoEvt.Broadcast(false);
		}
		else if (Ack.m_TResult == NET_RESULT_SUCCESS_FIRST_CONNECT)
		{
			OnGotUserInfoEvt.Broadcast(true);
		}
		break;
	}

	case PROTOCOL_USER_CHANGE_NICK_ACK:
	{
		PacketDisConnectAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);
		if (Ack.m_TResult == NET_RESULT_SUCCESS)
		{
			// Nothing to do
		}
		break;
	}

	case PROTOCOL_BASE_DISCONNECT_ACK:
	{
		PacketDisConnectAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);
		if (Ack.m_TResult == NET_RESULT_SUCCESS)
		{
			EPBNetClientState OldClientState = ClientState;
			ClientState = EPBNetClientState::NotConnected;
			if (OldClientState != ClientState)
			{
				OnStateChangedEvt.Broadcast(OldClientState, ClientState);
			}
		}
		break;
	}

	case PROTOCOL_BASE_NOTICE_ACK:
	{
		PacketNoticeAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);
		FString RecvMessage;
		UPBPacketManager::S2MOStringToUnrealString(Ack.m_strNotice, RecvMessage);
		OnNoticeEvt.Broadcast(RecvMessage);
	} break;

	case PROTOCOL_ROOM_CREATE_ACK:
	{
		PacketRoomCreateAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);
		if (Ack.m_TResult == NET_RESULT_SUCCESS)
		{
			OnRoomCreatedEvt.Broadcast(Ack.m_ui32RoomIdx);
		}
		break;
	}

	case PROTOCOL_ROOM_LIST_ACK:
	{
		PacketRoomListAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);
		if (Ack.m_TResult == NET_RESULT_SUCCESS)
		{
			TArray<FPBRoomInfo> RoomList;
			RoomList.AddDefaulted(Ack.m_RoomList.GetCount());
			for (int32 i = 0; i < Ack.m_RoomList.GetCount(); ++i)
			{
				RoomList[i].FromNetRoomInfo(Ack.m_RoomList[i]);
			}
			OnRoomListEvt.Broadcast(RoomList);
		}
		break;
	}

	case PROTOCOL_ROOM_ENTER_ACK:
	{
		PacketRoomEnterAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);
		if (Ack.m_TResult == NET_RESULT_SUCCESS)
		{
			CurrentRoomInfo.FromNetRoomInfo(Ack.m_RoomInfo);

			CurrentVoteBoardInfo.SecondsTillGameStart = FMath::Max((uint8)Ack.m_ui8ReminTimeSec, (uint8)0);
			for (int32 i = 0; i < Ack.m_MapInfo.GetCount(); ++i)
			{
				CurrentVoteBoardInfo.MapIdxToVoteCount.Add(Ack.m_MapInfo[i].m_ui32MapIdx, Ack.m_MapInfo[i].m_ui8VoteCount);
			}

			FPBRoomSlotInfo RoomSlotInfo;
			RoomSlotInfo.FromNetRoomSlotInfo(Ack.m_SlotInfo);

			auto RoomUserManager = UPBRoomUserInfoManager::Get(GetOuter());
			if (RoomUserManager)
			{
				RoomUserManager->SetMySlotInfo(RoomSlotInfo);
			}

			OwningPlayerTeam = RoomSlotInfo.TeamType;
			OwningPlayerIdx = RoomSlotInfo.TeamSlotIdx;

			OnEnterRoomEvt.Broadcast(true);
			OnCurrentRoomDataUpdatedEvt.Broadcast();
		}
		else
		{
			OnEnterRoomEvt.Broadcast(false);
		}
		break;
	}

	case PROTOCOL_ROOM_SLOT_INFO_ACK:
	{
		PacketRoomSlotInfoAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);

		if (Ack.m_TResult == NET_RESULT_SUCCESS)
		{
			for (int32 i = 0; i < Ack.m_SlotInfo.GetCount(); ++i)
			{
				FPBRoomSlotInfo RoomSlotInfo;
				RoomSlotInfo.FromNetRoomSlotInfo(Ack.m_SlotInfo[i]);

				auto RoomUserManager = UPBRoomUserInfoManager::Get(GetOuter());
				if (RoomUserManager)
				{
					RoomUserManager->SetUserSlotInfo(RoomSlotInfo);
				}				
			}

			OnCurrentRoomDataUpdatedEvt.Broadcast();
		}
		break;
	}

	case PROTOCOL_ROOM_MAP_VOTE_ACK:
	{
		PacketRoomMapVoteAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);

		if (Ack.m_TResult == NET_RESULT_SUCCESS)
		{
			// Nothing to do
		}

		break;
	}

	case PROTOCOL_ROOM_SLOT_READY_ACK:
	{
		PacketRoomSlotReadyAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);

		if (Ack.m_TResult == NET_RESULT_SUCCESS)
		{
			// Nothing to do
		}
		break;
	}

	case PROTOCOL_ROOM_ENTER_TRANS_ACK:
	{
		PacketRoomEnterTransAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);

		{
			FPBRoomSlotInfo RoomSlotInfo;
			RoomSlotInfo.FromNetRoomSlotInfo(Ack.m_SlotInfo);

			auto RoomUserManager = UPBRoomUserInfoManager::Get(GetOuter());
			if (RoomUserManager)
			{
				RoomUserManager->SetUserSlotInfo(RoomSlotInfo);				
			}

			OnCurrentRoomDataUpdatedEvt.Broadcast();
		}
		break;
	}

	case PROTOCOL_ROOM_LEAVE_TRANS_ACK:
	{
		PacketRoomLeaveTransAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);

		{
			FPBRoomSlotBase RoomSlotBase;
			RoomSlotBase.FromNetRoomSlotBase(Ack.m_SlotBase);

			auto RoomUserManager = UPBRoomUserInfoManager::Get(GetOuter());
			if (RoomUserManager)
			{
				FPBRoomSlotInfo& CurrentRoomSlotInfo = RoomUserManager->GetUserSlotInfoByTeam(RoomSlotBase.TeamType, RoomSlotBase.TeamSlotIdx);
				CurrentRoomSlotInfo.SlotState = RoomSlotBase.SlotState;				
			}	

			OnCurrentRoomDataUpdatedEvt.Broadcast();
		}
		break;
	}

	case PROTOCOL_ROOM_MAP_VOTE_TRANS_ACK:
	{
		PacketRoomMapVoteTransAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);

		{
			for (int32 i = 0; i < Ack.m_MapInfo.GetCount(); ++i)
			{
				CurrentVoteBoardInfo.MapIdxToVoteCount.Add(Ack.m_MapInfo[i].m_ui32MapIdx, Ack.m_MapInfo[i].m_ui8VoteCount);
			}

			OnCurrentRoomDataUpdatedEvt.Broadcast();
		}
		break;
	}

	case PROTOCOL_ROOM_SLOT_STATE_CHANGE_ACK:
	{
		PacketRoomSlotStateChangeAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);

		{
			for (int32 i = 0; i < Ack.m_SlotBase.GetCount(); ++i)
			{
				FPBRoomSlotBase RoomSlotBase;
				RoomSlotBase.FromNetRoomSlotBase(Ack.m_SlotBase[i]);

				auto RoomUserManager = UPBRoomUserInfoManager::Get(GetOuter());
				if (RoomUserManager)
				{
					FPBRoomSlotInfo& CurrentRoomSlotInfo = RoomUserManager->GetUserSlotInfoByTeam(RoomSlotBase.TeamType, RoomSlotBase.TeamSlotIdx);
					CurrentRoomSlotInfo.SlotState = RoomSlotBase.SlotState;
				}
			}

			OnCurrentRoomDataUpdatedEvt.Broadcast();
		}
		break;
	}

	case PROTOCOL_ROOM_STATE_CHANGE_ACK:
	{
		PacketRoomStateChangeAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);

		{
			CurrentRoomInfo.RoomState = (EPBRoomState)(int8)Ack.m_i8State;
			CurrentVoteBoardInfo.SecondsTillGameStart = FMath::Max((uint8)Ack.m_ui8ReminTimeSec, (uint8)0);

			OnCurrentRoomDataUpdatedEvt.Broadcast();

			if (CurrentRoomInfo.RoomState == EPBRoomState::Countdown)
			{
				OnBattleStartCountdownEvt.Broadcast();
			}
		}
		break;
	}

	case PROTOCOL_ROOM_BATTLE_START_ACK:
	{
		PacketBattleStartAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);

		{
			TArray<FPBRoomSlotInfo> StartingPlayers;
			for (int32 i = 0; i < Ack.m_StartSlot.GetCount(); ++i)
			{
				FPBRoomSlotBase RoomSlotBase;
				RoomSlotBase.FromNetRoomSlotBase(Ack.m_StartSlot[i]);

				auto RoomUserManager = UPBRoomUserInfoManager::Get(GetOuter());
				if (RoomUserManager)
				{
					FPBRoomSlotInfo& CurrentRoomSlotInfo = RoomUserManager->GetUserSlotInfoByTeam(RoomSlotBase.TeamType, RoomSlotBase.TeamSlotIdx);
					StartingPlayers.Add(CurrentRoomSlotInfo);
				}
			}

			OnBattleStartEvt.Broadcast(StartingPlayers, OwningPlayerTeam, (uint32)Ack.m_ui32MapIdx,TempDediAddress);
		}

		break;
	}

	case PROTOCOL_ROOM_LEAVE_ACK:
	{
		PacketRoomLeaveAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);
		if (Ack.m_TResult == NET_RESULT_SUCCESS)
		{
			CurrentRoomInfo = FPBRoomInfo();

			CurrentVoteBoardInfo = FPBRoomVoteBoardInfo();

			auto RoomUserManager = UPBRoomUserInfoManager::Get(GetOuter());
			if (RoomUserManager)
			{
				RoomUserManager->EmptyUserSlots();
			}

			OnCurrentRoomDataUpdatedEvt.Broadcast();
			OnLeaveRoomEvt.Broadcast();
		}
		break;
	}

	case PROTOCOL_ROOM_BATTLE_END_ACK:
	{
		PacketBattleEndAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);
		if (Ack.m_TResult == NET_RESULT_SUCCESS)
		{
			// Nothing to do
		}
		break;
	}

	default:
		return;
	}
}
