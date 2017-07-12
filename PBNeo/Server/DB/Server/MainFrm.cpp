#include "stdafx.h"
#include "MainFrm.h"

#include "Config.h"
#include "ConnectorCenter.h"
#include "DatabaseMgr.h"

CMainFrm::CMainFrm()
{
	m_pServerCenter		= NULL;

	m_pDB				= NULL;
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
	if( FALSE == m_pServerCenter->CreateRingBuffer( 1, sizeof(RING_CENTER_POP), 1024,
		1, sizeof(RING_CENTER_POP), 1024 ) )
	{
		return FALSE;
	}


	S2RING_INFO	RingDBPushInfo( (INT32)sizeof(RING_DB_PUSH),	100 );
	S2RING_INFO	RingDBPopInfo( (INT32)sizeof(RING_DB_POP),	100 );

	m_pDB = new CDatabaseMgr;
	if( ( NULL == m_pDB ) ||
		( FALSE == m_pDB->Create( pConfig->m_i32DBCount, &pConfig->m_DBConnectInfo, &RingDBPushInfo, &RingDBPopInfo ) ) )
	{
		return FALSE;
	}
	return TRUE;
}

void CMainFrm::Destroy()
{
	SAFE_DELETE( m_pServerCenter );
}

void CMainFrm::OnUpdate()
{
	_CheckPacketBuffer();
	_CheckDBBuffer();
}

void CMainFrm::_CheckPacketBuffer()
{
	INT32 i32ServerIdx;
	char pBuffer[ S2_PACKET_MAX_SIZE ];

	S2RingBuffer* pRing;
	RING_CENTER_POP* pPop;

	pRing = m_pServerCenter->GetPopRing();
	while( 0 < pRing->GetBufferCount() )
	{
		pPop = (RING_CENTER_POP*)pRing->Pop();
		if( pPop )
		{
			_PacketParsing( pPop->m_pBuffer );
			pRing->PopIdx();
		}
	}
}

void CMainFrm::_CheckDBBuffer()
{
	S2RingBuffer* pRing;
	RING_DB_POP* pPop;
	for( INT32 i = 0 ; i < m_pDB->GetPopBufferCount() ; i++ )
	{
		pRing = m_pDB->GetPopBuffer( i );

		if( 0 >= pRing->GetBufferCount() ) continue;
		pPop = (RING_DB_POP*)pRing->Pop();
		
		switch( pPop->m_DBType )
		{
		case DB_TYPE_USERINFO_LOAD:
			{
				_GetUserInfo( pPop );				
			}
			break;
			
		}

		pRing->PopIdx();
	}
}

void CMainFrm::_PacketParsing( char* pPacket )
{
	S2MOPacketBase PacketBase;
	PacketBase.UnPacking_Head( pPacket );

	switch( PacketBase.GetProtocol() )
	{
	case PROTOCOL_BASE_CONNECT_ACK:
		{
		}
		break;
	case PROTOCOL_USER_INFO_LOAD_REQ:
		{
			PacketUserInfoLoadReq Recv;
			Recv.UnPacking_T( pPacket, true );

			RING_DB_PUSH Push;
			Push.m_DBType			= DB_TYPE_USERINFO_LOAD;
			Push.m_i32SessionIdx	= Recv.GetSessionID();
			Push.m_TUID				= Recv.GetUID();

			m_pDB->Push( &Push );
		}
		break;
	case PROTOCOL_USER_INFO_SAVE_REQ:
		{
			PacketUserInfoSaveReq Recv;
			Recv.UnPacking_T( pPacket, true );

			RING_DB_PUSH Push;
			Push.m_DBType			= DB_TYPE_USERINFO_SAVE;
			Push.m_TUID				= Recv.m_TUID;
			Push.m_Base				= Recv.m_Base;
			Push.m_Info				= Recv.m_Info;
			Push.m_Record			= Recv.m_Record;
			Push.m_Eequipment		= Recv.m_Eequipment;

			m_pDB->Push( &Push );
		}
		break;
	}
}