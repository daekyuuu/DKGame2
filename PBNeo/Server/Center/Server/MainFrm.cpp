#include "stdafx.h"
#include "MainFrm.h"

#include "Config.h"

#include "RoomMgr.h"
#include "UserMgr.h"

#include "SessionMgr.h"

CMainFrm::CMainFrm()
{
	m_pUserMgr			= NULL;

	m_pRoomMgr			= NULL;

	m_pSessionMgr		= NULL;
	m_pServerContext	= NULL;
}

CMainFrm::~CMainFrm()
{
	Destroy();
}

BOOL CMainFrm::Create( CConfig* pConfig )
{
	m_i32IOCPThreadCount = pConfig->m_i32IOCPThreadCount;

	m_pUserMgr = new CUserMgr;
	if( (NULL == m_pUserMgr ) ||
		( FALSE == m_pUserMgr->Create( pConfig->m_ui32UserCount ) ) )
	{
		return FALSE;
	}
	
	m_pSessionMgr = new CSessionMgr;
	if( (NULL == m_pSessionMgr) ||
		(FALSE == m_pSessionMgr->Create( 100, pConfig->m_i32IOCPThreadCount, S2MO_PACKET_CONTENT_SIZE,  1024 ) ) )
	{
		return FALSE;
	}
	m_pSessionMgr->SetPacketBuffer();

	UINT8 ui8TimeOut = 5;

	m_pServerContext = new S2NetServerContext;
	if( (NULL == m_pServerContext) || 
		(FALSE == m_pServerContext->Create( 1, &pConfig->m_ui32IP, &pConfig->m_ui16Port, pConfig->m_i32IOCPThreadCount, m_pSessionMgr ) ))
	{
		//g_pLog->WriteLog( L"[FAIL] Server Context Create"  );
		return FALSE;
	}

	m_pRoomMgr = new CRoomMgr;
	if( (NULL == m_pRoomMgr) ||
		( FALSE == m_pRoomMgr->Create( pConfig->m_ui32RoomCount, m_pSessionMgr ) ) )
	{
		return FALSE;
	}

	return TRUE;
}

void CMainFrm::Destroy()
{
	SAFE_DELETE( m_pServerContext );
	SAFE_DELETE( m_pSessionMgr );

	SAFE_DELETE( m_pRoomMgr );

	SAFE_DELETE( m_pUserMgr );
}

void CMainFrm::OnUpdate()
{
	_CheckPacketBuffer();
	m_pRoomMgr->OnUpdate();
	m_pSessionMgr->Update();
}

void CMainFrm::SendNotice( S2MOPacketBase* pPacket )
{
	m_pSessionMgr->SendPacketServer( SERVER_TYPE_FRONT, pPacket );
}

void CMainFrm::_CheckPacketBuffer()
{
	RING_IOCP_POP* pPopRing;
	for( INT32 i = 0 ; i < m_i32IOCPThreadCount ; i++ )
	{
		pPopRing = m_pSessionMgr->GetMainPop( i );
		if( pPopRing )
		{
			_PacketParsing( pPopRing );
		}
		m_pSessionMgr->GetMainPopIdx( i );
	}
}

void CMainFrm::_PacketParsing( RING_IOCP_POP* pRing )
{
	S2MOPacketBase PacketBase;
	PacketBase.UnPacking_Head( pRing->m_pBuffer );

	T_RESULT TResult					= NET_RESULT_SUCCESS;
	// Return 용 패킷은 통상 패킷 + 1 입니다.
	T_PACKET_PROTOCOL TReturnProtocol	= PacketBase.GetProtocol() + 1;

	switch( PacketBase.GetProtocol() )
	{
	case PROTOCOL_BASE_CONNECT_AUTH_REQ:
		{
			PacketConnectAuthReq Packet;
			Packet.UnPacking_T( pRing->m_pBuffer, true );			

			m_pSessionMgr->SetServerType( (SERVER_TYPE)(UINT8)(Packet.m_ui8Type), Packet.m_ui32ServerIdx, pRing->m_pSession );
		}
		break;
	case PROTOCOL_LOGIN_INPUT_REQ:
		{
			PacketLoginReq Recv;
			Recv.UnPacking_T( pRing->m_pBuffer, true );
			TResult = _LoginReq( pRing );

			if( S2MO_IS_FAIL( TResult ) )
			{ 
				PacketLoginReq Recv;
				Recv.UnPacking_T( pRing->m_pBuffer, true );

				PacketLoginAck Packet;
				Packet.m_TResult		= TResult;
				Packet.SetSessionID( Recv.GetSessionID() );

				CSession* pSession	= m_pSessionMgr->GetServerType( pRing->m_eServerType, pRing->m_i32ServerIdx );
				pSession->SendPacket( &Packet );
			}
		}
		break;
	case PROTOCOL_LOGOUT_REQ:
		{
			_LogoutReq( pRing );
		}
		break;
	case PROTOCOL_LOGIN_INPUT_ACK:
		{
			_LoginInputAck( pRing );
		}
		break;
	case PROTOCOL_USER_INFO_LOAD_REQ:
		{
			_UserInfoLoadReq( pRing );
		}
		break;
	case PROTOCOL_USER_INFO_LOAD_ACK:
		{
			_UserInfoLoadAck( pRing );
		}
		break;
	case PROTOCOL_USER_CHANGE_NICK_REQ:
		{
			_UserChangeNick( pRing );
		}
		break;
	case PROTOCOL_ROOM_CREATE_REQ:
		{
			_RoomCreateReq( pRing );
		}
		break;
	case PROTOCOL_ROOM_ENTER_REQ:
		{
			_RoomEnterReq( pRing );
		}
		break;
	case PROTOCOL_ROOM_LEAVE_REQ:
		{
			_RoomLeaveReq( pRing );
		}
		break;
	case PROTOCOL_ROOM_QUICK_MATCH_REQ:
		{
			_RoomQuickMatch( pRing );
		}
		break;
	case PROTOCOL_ROOM_SLOT_INFO_REQ:
		{
		}
		break;
	case PROTOCOL_ROOM_OPTION_CHANGE_REQ:
		{
		}
		break;
	case PROTOCOL_ROOM_MAP_VOTE_REQ:
		{
			_RoomMapVote( pRing );
		}
		break;
	case PROTOCOL_ROOM_SLOT_READY_REQ:
		{
			_RoomSlotReady( pRing );
		}
		break;
	case PROTOCOL_ROOM_BATTLE_READY_REQ:
		{
			_RoomBattleReady( pRing );
		}
		break;
	case PROTOCOL_ROOM_BATTLE_END_REQ:
		{
			_RoomBattleEnd( pRing );
		}
		break;
	case PROTOCOL_ROOM_LIST_REQ:
		{
			_RoomList( pRing );
		}
		break;
	}

	//if( S2MO_IS_FAIL( TResult ) )
	//{
	//	S2MOPacketBaseResultT<PROTOCOL_VALUE_NONE>	Result;
	//	Result.SetProtocol( TReturnProtocol );
	//	Result.m_TResult	= TResult;
	//	CSession* pSession	= m_pSessionMgr->GetServerType( pRing->m_eServerType, pRing->m_i32ServerIdx );
	//	pSession->SendPacket( &Result );
	//}
}