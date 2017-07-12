#include "stdafx.h"
#include "MainFrm.h"

#include "Config.h"
#include "ConnectorCenter.h"
#include "SessionMgr.h"
#include "Session.h"

CMainFrm::CMainFrm()
{
	m_pSessionMgr		= NULL;
	m_pServerContext	= NULL;

	m_pServerCenter		= NULL;
}

CMainFrm::~CMainFrm()
{
	Destroy();
}

BOOL CMainFrm::Create( CConfig* pConfig )
{
	m_i32IOCPThreadCount = ((CConfig*)pConfig)->m_i32IOCPThreadCount;

	m_pServerCenter = new CConnectorCenter;
	if( NULL == m_pServerCenter )
	{
		return FALSE;
	}
	if( FALSE == m_pServerCenter->Create( pConfig->m_ui32CenterIP, pConfig->m_ui16CenterPort, pConfig->m_ServerKey ) )
	{
		return FALSE;
	}
	if( FALSE == m_pServerCenter->CreateRingBuffer( m_i32IOCPThreadCount, sizeof(IOCP_RING_POP), 1024,
		1, sizeof(IOCP_RING_POP), 1024 ) )
	{
		return FALSE;
	}

	m_pSessionMgr = new CSessionMgr;
	if( (NULL == m_pSessionMgr) ||
		(FALSE == m_pSessionMgr->Create( 100, m_i32IOCPThreadCount, S2MO_PACKET_CONTENT_SIZE,  1024 ) ) )
	{
		return FALSE;
	}
	m_pSessionMgr->SetPacketBuffer( m_pServerCenter->GetPushRing() );

	UINT8 ui8TimeOut = 5;

	m_pServerContext = new S2NetServerContext;
	if( (NULL == m_pServerContext) || 
		(FALSE == m_pServerContext->Create( 1, &pConfig->m_ui32IP, &pConfig->m_ui16Port, pConfig->m_i32IOCPThreadCount, m_pSessionMgr ) ))
	{
		//g_pLog->WriteLog( L"[FAIL] Server Context Create"  );
		return FALSE;
	}

	return TRUE;
}

void CMainFrm::Destroy()
{
	SAFE_DELETE( m_pServerContext );
	SAFE_DELETE( m_pSessionMgr );
	SAFE_DELETE( m_pServerCenter );
}

void CMainFrm::OnUpdate()
{
	_CheckPacketBuffer();
	m_pSessionMgr->Update();
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

	for( INT32 i = 0 ; i < m_i32IOCPThreadCount ; i++ )
	{
		pRing = m_pSessionMgr->GetPopRing( i );
		while( 0 <  pRing->GetBufferCount() )
		{
			pPop = (IOCP_RING_POP*)pRing->Pop();
			if( NULL == pPop )	continue;

			_PacketParsing( pPop->m_pBuffer );
			pRing->PopIdx();
		}
	}
}

void CMainFrm::_PacketParsing( char* pPacket )
{
	CSession* pSession;
	S2MOPacketBase PacketBase;
	PacketBase.UnPacking_Head( pPacket );

	switch( PacketBase.GetProtocol() )
	{
	case PROTOCOL_BASE_CONNECT_ACK:
		{
		}
		break;
	case PROTOCOL_LOGIN_INPUT_ACK:
		{
			PacketLoginAck Recv;
			Recv.UnPacking_T( pPacket, true );

			pSession = (CSession*)m_pSessionMgr->GetSessionList( Recv.GetSessionID() );
			pSession->SendPacket( &Recv );
			pSession->m_TUID = Recv.m_TUID;
		}
		break;
	case PROTOCOL_BASE_NOTICE_ACK:
		{
			PacketNoticeAck Recv;
			Recv.UnPacking_T( pPacket, true );
			for( INT32 i = 0 ; i < m_pSessionMgr->GetSessionCount() ; i++ )
			{
				pSession = (CSession*)m_pSessionMgr->GetSessionList( i );
				if( pSession )
				{
					pSession->SendPacket( &Recv );
				}
			}
		}
		break;
	default:
		{
			T_UID		TUID;
			T_SESSIONID TSessionIdx;
			char pBuffer[ S2MO_PACKET_CONTENT_SIZE ];
			T_PACKET_SIZE TSize = S2MOChangePacketSC( pPacket, pBuffer, TUID, TSessionIdx );
			pSession = (CSession*)m_pSessionMgr->GetSessionList( TSessionIdx );
			pSession->SendPacketMessage( pBuffer, TSize );
		}
		break;
	}
}