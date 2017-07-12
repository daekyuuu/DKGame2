#include "stdafx.h"
#include "Session.h"

CSession::CSession()
{
}

CSession::~CSession()
{
}

BOOL CSession::Create( UINT32 ui32SendDelayTimeMilli )
{
	return S2NetSession::Create( ui32SendDelayTimeMilli );;
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
	S2NetSession::OnDisconnect( bForceMainThread );

	SERVER_TYPE_INFO ServerTypeInfo;
	ServerTypeInfo.m_eType		= m_eServerType;
	ServerTypeInfo.m_ui32Idx	= m_i32ServerIdx;

	DLG_SENDMESSAGE( DLG_MESSAGE_SERVER_DESTROY, (LPARAM)&ServerTypeInfo );

	return TRUE;
}

BOOL CSession::SendPacket( char* pPacket )
{
	INT32 i32SendSize = ((T_PACKET_SIZE)*((T_PACKET_SIZE*)pPacket));
	return (BOOL)SendPacketMessage( pPacket, i32SendSize );
}

BOOL CSession::SendPacket( S2MOPacketBase* pPacket )
{
	char pBuffer[ S2MO_PACKET_CONTENT_SIZE ];
	pPacket->Packing_T( pBuffer, TRUE );
	return (BOOL)SendPacketMessage( pBuffer, pPacket->GetPacketSize() );
}

void CSession::SetPacketBuffer( S2RingBuffer* pMainPop )
{
	m_pMainPop		= pMainPop;
}

void CSession::SetServerInfo( SERVER_TYPE eType, INT32 i32Idx )
{
	m_eServerType		= eType;
	m_i32ServerIdx		= i32Idx;
}

INT32 CSession::PacketParsing( char * pPacket, INT32 i32Size )
{
	S2MOPacketBase PacketBase( PROTOCOL_VALUE_NONE );
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
			RING_IOCP_POP* pPop		= (RING_IOCP_POP*)m_pMainPop[ m_i32WorkThreadIdx ].PushPointer();
			pPop->m_eServerType		= m_eServerType;
			pPop->m_i32ServerIdx	= m_i32SessionIdx;
			pPop->m_pSession		= this;
			S2Memory::Copy( pPop->m_pBuffer, pPacket, TSize );
			m_pMainPop[ m_i32WorkThreadIdx ].PushPointerIdx();
		}
		break;
	}

	return TSize;
}
