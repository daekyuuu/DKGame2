// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBNetServerInterface.h"
#include "PBServerPacketManager.h"

UPBNetServerInterface* UPBNetServerInterface::Get(UObject* WorldContextObject)
{
	return UPBGameplayStatics::GetNetServerInterface(WorldContextObject);
}

void UPBNetServerInterface::Init()
{
	UPBPacketManager* PM = UPBPacketManager::Get(GetOuter());
	if (PM)
	{
		HandleRecvEvent_Handle = PM->OnRecvEvent.AddUObject(this, &UPBNetServerInterface::HandleRecvEvent);
	}
}

void UPBNetServerInterface::ShutDown()
{
	UPBPacketManager* PM = UPBPacketManager::Get(GetOuter());
	if (PM)
	{
		if (HandleRecvEvent_Handle.IsValid())
		{
			PM->OnRecvEvent.Remove(HandleRecvEvent_Handle);
		}
	}
}

EPBNetServerState UPBNetServerInterface::GetState()
{
	return ServerState;
}

bool UPBNetServerInterface::ConnectToServer(FString IP, uint16 port)
{
	UPBPacketManager* PM = UPBPacketManager::Get(GetOuter());
	if (PM)
	{
		GsPacketConnectReq Send;
		UPBPacketManager::UnrealStringToS2MOString(IP, Send.m_strIP);
		Send.m_ui16Port = port;
		return PM->SendEvent(&Send);
	}

	return false;
}

bool UPBNetServerInterface::Login(uint16 myListenPort, int32 RoomIdx)
{
	UPBPacketManager* PM = UPBPacketManager::Get(GetOuter());
	if (PM)
	{
		PacketConnectAck Send;
		//Send.m_ui16ListenPort = myListenPort;
		return PM->SendEvent(&Send);
	}

	return false;
}

bool UPBNetServerInterface::AckLoadMapSignal(bool Success)
{
	UPBPacketManager* PM = UPBPacketManager::Get(GetOuter());
	if (PM)
	{
		GsPacketLoadMapAck Send;
		if (Success)
		{
			Send.m_TResult = GS_NET_RESULT_SUCCESS;
		}
		else
		{
			Send.m_TResult = GS_NET_RESULT_ERROR;
		}
		return PM->SendEvent(&Send);
	}

	return false;
}

bool UPBNetServerInterface::AckShutdownSignal()
{
	UPBPacketManager* PM = UPBPacketManager::Get(GetOuter());
	if (PM)
	{
		GsPacketShutdownAck Send;
		Send.m_TResult = GS_NET_RESULT_SUCCESS;
		return PM->SendEvent(&Send);
	}

	return false;
}

void UPBNetServerInterface::HandleRecvEvent(struct FPacketRecvEvent RecvEvent)
{
	switch (RecvEvent.TProtocol)
	{
	case GS_PROTOCOL_BASE_CONNECT_ACK:
	{
		PacketConnectAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);
		if (Ack.m_TResult == GS_NET_RESULT_SUCCESS)
		{
			EPBNetServerState OldServerState = ServerState;
			ServerState = EPBNetServerState::Connected;
			if (OldServerState != ServerState)
			{
				OnStateChangedEvt.Broadcast(OldServerState, ServerState);
			}
		}
		break;
	}

	case GS_PROTOCOL_LOGIN_LOGIN_ACK:
	{
		GsPacketLoginAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);
		if (Ack.m_TResult == GS_NET_RESULT_SUCCESS)
		{
			EPBNetServerState OldServerState = ServerState;
			ServerState = EPBNetServerState::LoggedIn;
			if (OldServerState != ServerState)
			{
				OnStateChangedEvt.Broadcast(OldServerState, ServerState);
			}
		}
		break;
	}

	case GS_PROTOCOL_BASE_DISCONNECT_ACK:
	{
		GsPacketDisConnectAck Ack;
		Ack.UnPacking_T(RecvEvent.Buffer);
		if (Ack.m_TResult == GS_NET_RESULT_SUCCESS)
		{
			EPBNetServerState OldServerState = ServerState;
			ServerState = EPBNetServerState::NotConnected;
			if (OldServerState != ServerState)
			{
				OnStateChangedEvt.Broadcast(OldServerState, ServerState);
			}
		}
		break;
	}

	case GS_PROTOCOL_CONTROL_LOADMAP_REQ:
	{
		GsPacketLoadMapReq Req;
		Req.UnPacking_T(RecvEvent.Buffer);

		OnLoadMapSignalEvt.Broadcast((int32)Req.m_i32MapTableIndex);
		break;
	}

	case GS_PROTOCOL_CONTROL_SHUTDOWN_REQ:
	{
		OnShutdownSignalEvt.Broadcast();
		break;
	}

	case GS_PROTOCOL_CONTROL_KILL_REQ:
	{
		OnKillSignalEvt.Broadcast();
		break;
	}

	default:
		return;
	}
}
