#include "stdafx.h"
#include "RoomMgr.h"

CRoomMgr::CRoomMgr()
{
}

CRoomMgr::~CRoomMgr()
{
	Destroy();
}

BOOL CRoomMgr::Create( UINT32 ui32RoomCount, CSessionMgr* pSessionMgr )
{
	m_ui32RoomCount = ui32RoomCount;

	m_ppRoom = new CRoom*[ m_ui32RoomCount ];
	if( NULL == m_ppRoom )											return FALSE;
	S2Memory::Fill( m_ppRoom, 0, sizeof(CRoom*)*m_ui32RoomCount );

	for( UINT32 i = 0 ; i < m_ui32RoomCount ; i++ )
	{
		m_ppRoom[i] = new CRoom;
		if( (NULL == m_ppRoom[i]) ||
			(FALSE == m_ppRoom[i]->Create( i, pSessionMgr )) )		return FALSE;
	}

	return TRUE;
}

void CRoomMgr::Destroy()
{
	if( m_ppRoom )
	{
		for( UINT32 i = 0 ; i < m_ui32RoomCount ; i++ )
		{
			SAFE_DELETE( m_ppRoom[i] );
		}
		SAFE_DELETE_ARRAY( m_ppRoom );
	}
}

void CRoomMgr::OnUpdate()
{
	for( UINT32 i = 0 ; i < m_ui32RoomCount ; i++ )
	{
		if( NET_ROOM_STATE_CLOSE == m_ppRoom[i]->GetRoomState() )	continue;

		m_ppRoom[i]->OnUpdate();
	}
}

T_RESULT CRoomMgr::RoomCreate( PacketRoomCreateReq* pRecv, USER_LIST* pUser )
{
	CRoom* pRoom = NULL;
	for( UINT32 i = 0 ; i < m_ui32RoomCount ; i++ )
	{
		if( NET_ROOM_STATE_CLOSE != m_ppRoom[i]->GetRoomState() )	continue;

		pRoom = m_ppRoom[i];
		break;
	}

	if( NULL == pRoom )
	{
		return NET_RESULT_ERROR_SYSTEM;
	}

	T_RESULT TResult = pRoom->RoomCreate( pRecv, pUser );
	if( S2MO_IS_FAIL( TResult )  )
	{
		return TResult;
	}

	return NET_RESULT_SUCCESS;
}

T_RESULT CRoomMgr::RoomUserEnter( PacketRoomEnterReq* pRecv, USER_LIST* pUser )
{
	if( USER_STATE_ROOM <= pUser->m_eUserState )	return NET_RESULT_ERROR_PACKET;
	if( pRecv->m_ui32RoomIdx > m_ui32RoomCount )	return NET_RESULT_ERROR_PACKET;
	return m_ppRoom[ pRecv->m_ui32RoomIdx ]->UserEnter( pUser );
}

T_RESULT CRoomMgr::RoomUserLeave( USER_LIST* pUser )
{
	if( pUser->m_i32RoomIdx > m_ui32RoomCount )	return NET_RESULT_ERROR_PACKET;
	return m_ppRoom[ pUser->m_i32RoomIdx ]->UserLeave( pUser->m_i32SlotIdx );
}

T_RESULT CRoomMgr::RoomUserReady( USER_LIST* pUser )
{
	if( pUser->m_i32RoomIdx > m_ui32RoomCount )	return NET_RESULT_ERROR_PACKET;
	return m_ppRoom[ pUser->m_i32RoomIdx ]->UserReady( pUser );
}

T_RESULT CRoomMgr::QuickMatch( PacketRoomQuickMatchReq* pRecv, USER_LIST* pUser )
{
	return TRUE;
}

T_RESULT CRoomMgr::MapVote( PacketRoomMapVoteReq* pRecv, USER_LIST* pUser )
{
	if( pUser->m_i32RoomIdx > m_ui32RoomCount )	return NET_RESULT_ERROR_PACKET;
	return m_ppRoom[ pUser->m_i32RoomIdx ]->MapVote( pRecv, pUser->m_i32SlotIdx );
}

T_RESULT CRoomMgr::RoomBattleReady( USER_LIST* pUser )
{
	if( pUser->m_i32RoomIdx > m_ui32RoomCount )	return NET_RESULT_ERROR_PACKET;
	return m_ppRoom[ pUser->m_i32RoomIdx ]->BattleReady( pUser );
}

T_RESULT CRoomMgr::RoomBattleEnd( USER_LIST* pUser )
{
	if( pUser->m_i32RoomIdx > m_ui32RoomCount )	return NET_RESULT_ERROR_PACKET;
	return m_ppRoom[ pUser->m_i32RoomIdx ]->BattleEnd( pUser );
}

T_RESULT CRoomMgr::RoomList( USER_LIST* pUser )
{
	PacketRoomListAck Send;
	Send.m_TResult			= NET_RESULT_SUCCESS;
	Send.SetSessionID( pUser->m_i32SessionIdx );
	Send.SetUID( pUser->m_TUID );
	INT32 i32RoomIdx = 0;
	for( UINT32 i = 0 ; i < m_ui32RoomCount ; i++ )
	{
		if( NET_ROOM_STATE_CLOSE == m_ppRoom[i]->GetRoomState() )	continue;

		m_ppRoom[i]->Copy( &Send.m_RoomList[ i32RoomIdx ] );
		i32RoomIdx++;
		if( NET_ROOM_LIST_PAGE_COUNT <= i32RoomIdx ) break;
	}
	pUser->m_pFrontSession->SendPacket( &Send );

	return NET_RESULT_SUCCESS;
}