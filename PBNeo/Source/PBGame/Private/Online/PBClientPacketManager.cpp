// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBClientPacketManager.h"

bool UPBClientPacketManager::SendEvent(S2MOPacketBase* pPacket)
{
	if (pPacket->GetProtocol() == PROTOCOL_BASE_CONNECT_REQ)
	{
		PacketConnectReq* pSend = (PacketConnectReq*)pPacket;

		FString TargetAddrString;
		S2MOStringToUnrealString(pSend->m_strIP, TargetAddrString);

		if (false == ConnectTo(TargetAddrString, pSend->m_ui16Port))
		{
			char pBuffer[S2MO_PACKET_CONTENT_SIZE];
			PacketConnectAck Recv;
			Recv.m_TResult = NET_RESULT_ERROR;
			Recv.Packing_T(pBuffer);

			FPacketRecvEvent Event;
			Event.TProtocol = Recv.GetProtocol();
			memcpy(Event.Buffer, pBuffer, Recv.GetPacketSize());
			OnRecvEvent.Broadcast(Event);
		}
		return true;
	}
	else
	{
		return Super::SendEvent(pPacket);
	}
}

void UPBClientPacketManager::OnDisconnected()
{
	char pBuffer[S2MO_PACKET_CONTENT_SIZE];
	PacketDisConnectAck Recv;
	Recv.m_TResult = NET_RESULT_SUCCESS;
	Recv.Packing_T(pBuffer);

	FPacketRecvEvent Event;
	Event.TProtocol = Recv.GetProtocol();
	memcpy(Event.Buffer, pBuffer, Recv.GetPacketSize());
	OnRecvEvent.Broadcast(Event);
}

void UPBClientPacketManager::Test(int TestCode)
{
	switch (TestCode)
	{
	case 0:					// Connect
	{
		PacketConnectReq Send;
		UnrealStringToS2MOString(FString("192.168.2.21"), Send.m_strIP);
		Send.m_ui16Port = 36100;
		SendEvent(&Send);
	}
	break;
	case 1:
	{
		PacketLoginReq Send;
		UnrealStringToS2MOString(FString("Test01"), Send.m_strID);
		UnrealStringToS2MOString(FString(""), Send.m_strPW);
		SendEvent(&Send);
	}
	break;
	case 2:
	{
		PacketUserInfoLoadReq Send;
		SendEvent(&Send);
	}
	break;
	}
}
