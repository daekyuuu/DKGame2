#include "stdafx.h"
#include "GameServerConnManager.h"
#include "Net\GameServerNetPacket.h"

GameServerConnManager::GameServerConnManager(SOCKET Listener, HANDLE IoPort)
	:
	ConnectionManager(Listener, IoPort)
{
	myState = State::WaitingForConnection;
	ConnectedServerListenPort = 0; // 0 is not a valid port

	SpawnGameServer();
}

GameServerConnManager::~GameServerConnManager()
{
}

void GameServerConnManager::OnConnAccepted()
{
	if (myState == State::WaitingForConnection)
	{
		GsPacketConnectAck MsgToSend;
		MsgToSend.m_TResult = GS_NET_RESULT_SUCCESS;
		SendMsg(&MsgToSend);

		myState = State::WaitingForLogin;
	}
}

void GameServerConnManager::OnMsgReceived(T_PACKET_PROTOCOL Protocol, char* Buffer, UINT32 BufferSize)
{
	switch (myState)
	{
	case State::WaitingForLogin:
	{
		if (Protocol == GS_PROTOCOL_LOGIN_LOGIN_REQ)
		{
			GsPacketLoginReq MsgRecved;
			MsgRecved.UnPacking_T(Buffer);
			ConnectedServerListenPort = MsgRecved.m_ui16ListenPort;

			GsPacketLoginAck MsgToSend;
			MsgToSend.m_TResult = GS_NET_RESULT_SUCCESS;
			SendMsg(&MsgToSend);

			myState = State::Ready;
		}
		return;
	}

	case State::WaitingForLoadMapAck:
	{
		if (Protocol == GS_PROTOCOL_CONTROL_LOADMAP_ACK)
		{
			GsPacketLoadMapAck MsgRecved;
			MsgRecved.UnPacking_T(Buffer);
			if (MsgRecved.m_TResult == GS_NET_RESULT_SUCCESS)
			{
				// Load map succeeded
				myState = State::Ready;
			}
			else
			{
				// Load map failed
				myState = State::Ready;
			}
		}
		return;
	}

	case State::WaitingForShutdownAck:
	{
		if (Protocol == GS_PROTOCOL_CONTROL_SHUTDOWN_ACK)
		{
			GsPacketShutdownAck MsgRecved;
			MsgRecved.UnPacking_T(Buffer);
			if (MsgRecved.m_TResult == GS_NET_RESULT_SUCCESS)
			{
				GsPacketKillReq MsgToSend;
				SendMsg(&MsgToSend);

				myState = State::Closed;
			}
		}
		return;
	}

	default:
		return;
	}
}

void GameServerConnManager::OnConnReset()
{
	myState = State::WaitingForConnection;
}

void GameServerConnManager::LoadMap(INT32 MapTableIndex)
{
	if (myState == State::Ready)
	{
		GsPacketLoadMapReq MsgToSend;
		MsgToSend.m_i32MapTableIndex = MapTableIndex;
		SendMsg(&MsgToSend);

		myState = State::WaitingForLoadMapAck;
	}
}

void GameServerConnManager::Shutdown()
{
	if (myState == State::Ready)
	{
		GsPacketShutdownReq MsgToSend;
		SendMsg(&MsgToSend);

		myState = State::WaitingForShutdownAck;
	}
}

bool GameServerConnManager::ReadyToBeDestroyed()
{
	return (myState == State::Closed);
}
