// Fill out your copyright notice in the Description page of Project Settings.

#include "PBGame.h"
#include "PBServerPacketManager.h"

bool UPBServerPacketManager::SendEvent(S2MOPacketBase* pPacket)
{
	if (pPacket->GetProtocol() == GS_PROTOCOL_BASE_CONNECT_REQ)
	{
		GsPacketConnectReq* pSend = (GsPacketConnectReq*)pPacket;

		FString TargetAddrString;
		S2MOStringToUnrealString(pSend->m_strIP, TargetAddrString);

		if (false == ConnectTo(TargetAddrString, pSend->m_ui16Port))
		{
			char pBuffer[S2MO_PACKET_CONTENT_SIZE];
			GsPacketConnectAck Recv;
			Recv.m_TResult = GS_NET_RESULT_ERROR;
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

void UPBServerPacketManager::OnDisconnected()
{
	char pBuffer[S2MO_PACKET_CONTENT_SIZE];
	GsPacketDisConnectAck Recv;
	Recv.m_TResult = GS_NET_RESULT_SUCCESS;
	Recv.Packing_T(pBuffer);

	FPacketRecvEvent Event;
	Event.TProtocol = Recv.GetProtocol();
	memcpy(Event.Buffer, pBuffer, Recv.GetPacketSize());
	OnRecvEvent.Broadcast(Event);
}
