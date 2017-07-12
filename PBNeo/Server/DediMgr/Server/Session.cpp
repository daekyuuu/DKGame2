#include "stdafx.h"
#include "Session.h"

CSession::CSession()
{
}

CSession::~CSession()
{
}

BOOL CSession::Create()
{
	return TRUE;
}

void CSession::Destroy()
{
}

BOOL CSession::OnConnectInit()
{
	S2NetSession::OnConnectInit();

	PacketConnectAck Packet;
	Packet.m_TResult = NET_RESULT_SUCCESS;
	SendPacket( &Packet );
	return TRUE;
}

BOOL CSession::OnDisconnect( BOOL bForceMainThread )
{
	PacketLogoutReq Packet;

	Packet.SetSessionID( m_i32SessionIdx );
	Packet.SetUID( m_TUID );

	char pBuffer[ S2MO_PACKET_CONTENT_SIZE ];
	Packet.Packing_T( pBuffer, true );
	_PushRing( &m_pCenterPush[ m_i32WorkThreadIdx ], Packet.GetPacketSize(), pBuffer );

	return TRUE;
}

INT32 CSession::SendPacket( S2MOPacketBase* pPacket )
{
	char pBuffer[ S2MO_PACKET_CONTENT_SIZE ];
	pPacket->Packing_T( pBuffer );
	SendPacketMessage( pBuffer, pPacket->GetPacketSize() );

	return 0;
}

void CSession::SetPacketBuffer( S2RingBuffer* pMainPop, S2RingBuffer* pCenterPush )
{
	m_pMainPop		= pMainPop;
	m_pCenterPush	= pCenterPush;
}

INT32 CSession::PacketParsing( char * pPacket, INT32 i32Size )
{
	BOOL bResult = TRUE;
	S2MOPacketBase PacketBase;
	PacketBase.UnPacking_Head( pPacket );

	T_PACKET_SIZE TSize			= PacketBase.GetPacketSize();
	if( TSize < i32Size )		return 0;

	T_PACKET_PROTOCOL TProtocol	= PacketBase.GetProtocol();

	switch( TProtocol )
	{
	case PROTOCOL_BASE_HEARTBIT_REQ:
		{
			PacketHeartbitAck Packet;
			SendPacket( &Packet );
		}
		break;
	default:
		{
			char pBuffer[ S2MO_PACKET_CONTENT_SIZE ];
			T_PACKET_SIZE TSize = S2MOChangePacketCS( pPacket, pBuffer, m_TUID, m_i32SessionIdx );
			bResult = _PushRing( &m_pCenterPush[ m_i32WorkThreadIdx ], TSize, pBuffer );
		}
		break;
	}

	if( FALSE == bResult )
	{
		S2MOPacketBaseResultT<PROTOCOL_VALUE_NONE>	Result;
		Result.SetProtocol( PacketBase.GetProtocol() );
		Result.m_TResult = NET_RESULT_ERROR;
		SendPacket( &Result );
	}	

	return TSize;
}

BOOL CSession::_PushRing( S2RingBuffer* pRing, INT32 i32Size, char* pPacket )
{
	IOCP_RING_POP* pPop = (IOCP_RING_POP*)pRing->PushPointer();
	if( NULL == pPop )		return FALSE;
	
	S2Memory::Copy( pPop->m_pBuffer, pPacket, i32Size );
	pRing->PushPointerIdx();

	return TRUE;
}