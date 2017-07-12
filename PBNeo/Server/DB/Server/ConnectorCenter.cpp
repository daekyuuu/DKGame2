#include "stdafx.h"
#include "ConnectorCenter.h"

CConnectorCenter::CConnectorCenter()
{
}

CConnectorCenter::~CConnectorCenter()
{
	Destroy();
}

BOOL CConnectorCenter::Create( UINT32 ui32IP, UINT16 ui16Port, UINT32 ui32ServerIdx )
{
	m_ui32ServerIdx = ui32ServerIdx;

	if( FALSE == S2NetConnector::Create( ui32IP, ui16Port ) )
	{
		return FALSE;
	}
	
	m_dt32LastSendTime = S2Time::StandTime();
	m_dt32LastSendTime.AddTime( S2_DATE_ADD_TYPE_SECOND, SERVER_HEART_BIT_TIME_SEC );

	return TRUE;
}

void CConnectorCenter::Destroy()
{
	m_Thread.Destroy();
}

BOOL CConnectorCenter::OnRunning()
{
	BOOL bWorking = FALSE;

	// HeartBit
	DATE32 dt32Heartbit = m_dt32LastSendTime;
	dt32Heartbit.AddTime( S2_DATE_ADD_TYPE_MINUTE, 1 );
	if( dt32Heartbit < S2Time::StandTime() )
	{
		PacketHeartbitReq Packet;
		SendPacket( &Packet );

		bWorking = TRUE;
		m_dt32LastSendTime = S2Time::StandTime();
		m_dt32LastSendTime.AddTime( S2_DATE_ADD_TYPE_SECOND, SERVER_HEART_BIT_TIME_SEC );
	}

	S2RingBuffer* pRing;
	RING_CENTER_POP* pPop;
	for( INT32 i = 0 ; i < m_i32RingPushCount ; i++ )
	{
		pRing = &m_pPushRing[ i ];
		if( 0 < pRing->GetBufferCount() )
		{
			pPop = (RING_CENTER_POP*)pRing->Pop();
			
			S2MOPacketBase PacketBase( PROTOCOL_VALUE_NONE );
			PacketBase.UnPacking_Head( pPop->m_pBuffer );

			SendPacketMessage( pPop->m_pBuffer, PacketBase.GetPacketSize() );
			pRing->PopIdx();

			bWorking = TRUE;
		}
	}

	return bWorking;
}

INT32 CConnectorCenter::SendPacket( S2MOPacketBase* pPacket )
{
	char pBuffer[ S2MO_PACKET_CONTENT_SIZE ];
	pPacket->Packing_T( pBuffer, true );
	SendPacketMessage( pBuffer, pPacket->GetPacketSize() );

	m_dt32LastSendTime = S2Time::StandTime();
	m_dt32LastSendTime.AddTime( S2_DATE_ADD_TYPE_SECOND, SERVER_HEART_BIT_TIME_SEC );

	return 0;
}

INT32 CConnectorCenter::PacketParsing( char* pPacket, INT32 i32Size )
{
	S2MOPacketBase PacketBase( PROTOCOL_VALUE_NONE );
	PacketBase.UnPacking_Head( pPacket );

	T_PACKET_SIZE TSize			= PacketBase.GetPacketSize();
	if( TSize > i32Size )		return 0;
	
	T_PACKET_PROTOCOL TProtocol	= PacketBase.GetProtocol();

	char* pContents = pPacket + S2MO_PACKET_HEAD_SIZE;

	switch( TProtocol )
	{
	case PROTOCOL_BASE_HEARTBIT_ACK:
		{
		}
		break;
	case PROTOCOL_BASE_CONNECT_ACK:
		{
			PacketConnectAuthReq Packet;
			Packet.m_ui8Type		= THIS_SERVER_TYPE;
			Packet.m_ui32ServerIdx	= m_ui32ServerIdx;
			SendPacket( &Packet );
		}
		break;
	default:
		{
			RING_CENTER_POP* pPop = (RING_CENTER_POP*)m_pPopRing->PushPointer();
			S2Memory::Copy( pPop->m_pBuffer, pPacket, i32Size );
			m_pPopRing->PushPointerIdx();
		}
		break;
	}

	return TSize;
}