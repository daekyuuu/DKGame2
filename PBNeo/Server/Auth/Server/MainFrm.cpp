#include "stdafx.h"
#include "MainFrm.h"

#include "Config.h"
#include "ConnectorCenter.h"
#include "LoginMgr.h"

CMainFrm::CMainFrm()
{
	m_pServerCenter		= NULL;

	m_pLogin			= NULL;
}

CMainFrm::~CMainFrm()
{
	Destroy();
}

BOOL CMainFrm::Create( CConfig* pConfig )
{
	m_pServerCenter = new CConnectorCenter;
	if( NULL == m_pServerCenter )
	{
		return FALSE;
	}
	if( FALSE == m_pServerCenter->Create( pConfig->m_ui32CenterIP, pConfig->m_ui16CenterPort, pConfig->m_ServerKey ) )
	{
		return FALSE;
	}
	if( FALSE == m_pServerCenter->CreateRingBuffer( 1, sizeof(IOCP_RING_POP), 1024,	1, sizeof(IOCP_RING_POP), 1024 ) )
	{
		return FALSE;
	}

	S2RING_INFO	RingDBPushInfo( (INT32)sizeof(RING_LOGIN_PUSH),	100 );
	S2RING_INFO	RingDBPopInfo( (INT32)sizeof(RING_LOGIN_POP),	100 );

	m_pLogin = new CLoginMgr;
	if( ( NULL == m_pLogin ) ||
		( FALSE == m_pLogin->Create( pConfig->m_i32DBCount, &pConfig->m_DBConnectInfo, &RingDBPushInfo, &RingDBPopInfo ) ) )
	{
		return FALSE;
	}

	return TRUE;
}

void CMainFrm::Destroy()
{
	SAFE_DELETE( m_pLogin );
	SAFE_DELETE( m_pServerCenter );
}

void CMainFrm::OnUpdate()
{
	_CheckPacketBuffer();
	_CheckLoginBuffer();
}

void CMainFrm::_CheckPacketBuffer()
{
	INT32 i32ServerIdx;
	char pBuffer[ S2_PACKET_MAX_SIZE ];

	S2RingBuffer* pRing;
	IOCP_RING_POP* pPop;

	pRing = m_pServerCenter->GetPopRing();
	while( 0 <  pRing->GetBufferCount() )
	{
		pPop = (IOCP_RING_POP*)pRing->Pop();
		if( NULL == pPop )	continue;

		_PacketParsing( pPop->m_pBuffer );
		pRing->PopIdx();
	}
}

void CMainFrm::_PacketParsing( char* pPacket )
{
	S2MOPacketBase PacketBase( PROTOCOL_VALUE_NONE );
	PacketBase.UnPacking_Head( pPacket );

	switch( PacketBase.GetProtocol() )
	{
	case PROTOCOL_BASE_CONNECT_ACK:
		{
		}
		break;
	case PROTOCOL_LOGIN_INPUT_REQ:
		{
			PacketLoginReq Recv;
			Recv.UnPacking_T( pPacket, true );

			RING_LOGIN_PUSH Push;
			Recv.m_strID.GetValue( Push.m_strID, NET_ID_LENGTH );
			Recv.m_strPW.GetValue( Push.m_strPW, NET_PW_LENGTH );
			Push.m_i32SessionIdx	= Recv.GetSessionID();

			m_pLogin->InsertUser( &Push );
		}
		break;
	}
}

void CMainFrm::_CheckLoginBuffer()
{
	S2RingBuffer* pRing;
	RING_LOGIN_POP* pPop;
	for( INT32 i = 0 ; i < m_pLogin->GetPopBufferCount() ; i++ )
	{
		pRing = m_pLogin->GetPopBuffer( i );

		if( 0 >= pRing->GetBufferCount() ) continue;
		pPop = (RING_LOGIN_POP*)pRing->Pop();
		
		pPop->m_i32SessionIdx;
		pPop->m_TUID;

		PacketLoginAck Packet;
		Packet.m_TResult		= pPop->m_TResult;
		Packet.SetSessionID( pPop->m_i32SessionIdx );
		Packet.SetUID( pPop->m_TUID );
		Packet.m_TUID			= pPop->m_TUID;

		m_pServerCenter->SendPacket( &Packet );

		pRing->PopIdx();
	}
}