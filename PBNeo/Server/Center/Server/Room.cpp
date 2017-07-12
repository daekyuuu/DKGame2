#include "stdafx.h"
#include "Room.h"

#include "Config.h"

CRoom::CRoom()
{
}

CRoom::~CRoom()
{
	Destroy();
}

BOOL CRoom::Create( UINT32 ui32Idx, CSessionMgr* pSessionMgr )
{
	m_ui32Idx		= ui32Idx;
	m_pSessionMgr	= pSessionMgr;

	_Clear();	

	return TRUE;
}

void CRoom::Destroy()
{
}

void CRoom::_Clear()
{
	m_Room.Clear();
	for( INT32 i = 0 ; i < NET_ROOM_USER_COUNT ; i++ )
	{
		m_User[i].m_eSlotState = NET_ROOM_SLOT_STATE_OPEN;
	}
	_BattleClear();
}

void CRoom::_BattleClear()
{
	for( INT32 i = 0 ; i < NET_ROOM_PLAYER_USER_COUNT ; i++ )
	{
		m_UserBattle[ i ].Reset();
	}
	for( INT32 i = 0 ; i < NET_ROOM_PLAYER_USER_COUNT ; i++ )
	{
		m_MapList[i].m_ui8VoteCount = 0;
	}	
}

UINT32 CRoom::_GetBestMap()
{
	return m_MapList[0].m_ui32MapIdx;
}

BOOL CRoom::_SendPacket( S2MOPacketBase* pPacket, USER_LIST* pUser )
{
	pPacket->SetSessionID( pUser->m_i32SessionIdx );
	pPacket->SetUID( pUser->m_TUID );

	return pUser->m_pFrontSession->SendPacket( pPacket );
}

BOOL CRoom::_SendPacketAll( UINT32 ui32State, S2MOPacketBase* pPacket )
{
	BOOL bResult = TRUE;
	for( INT32 i = 0 ; i < NET_ROOM_USER_COUNT ; i++ )
	{
		if( 0 == (ui32State & m_User[ i ].m_eSlotState) ) continue;

		if( FALSE == _SendPacket( pPacket, m_User[ i ].m_pUserList ) )
		{
			bResult = FALSE;
		}
	}
	return bResult;
}

void CRoom::OnUpdate()
{
	switch( m_Room.m_eRoomState	)
	{
	case NET_ROOM_STATE_CLOSE:
		{
		}
		break;
	case NET_ROOM_STATE_OPEN:
		{
			_OnUpdateOpen();
		}
		break;
	case NET_ROOM_STATE_READY:
		{
			_OnUpdateReady();
		}
		break;
	case NET_ROOM_STATE_COUNTDOWN:
		{
			_OnUpdateCountdown();
		}
		break;
	case NET_ROOM_STATE_PLAYING:
		{
			_OnUpdatePlaying();
		}
		break;
	}
}

void CRoom::_OnUpdateOpen()
{
	_SetStateChangeReady();
}

void CRoom::_OnUpdateReady()
{
	if( S2Time::StandTime() < m_dt32NextStartTime )	return;

	_SetStateChangeCountdown();
}

void CRoom::_OnUpdateCountdown()
{
	if( S2Time::StandTime() < m_dt32NextStartTime )	return;

	_SetStateChangePlaying();
}

void CRoom::_OnUpdatePlaying()
{
	SlotInfo* pSlotInfo;
	
	// 난입 유저 처리
	for( INT32 i = 0 ; i < NET_ROOM_USER_COUNT ; i++ )
	{
		pSlotInfo = &m_User[ i ];
		switch( pSlotInfo->m_eSlotState )
		{
			case NET_ROOM_SLOT_STATE_INTRUSION:
			{
				if( S2Time::StandTime() < pSlotInfo->m_dt32StartTime )			continue;

				pSlotInfo->m_eSlotState = NET_ROOM_SLOT_STATE_PLAYING;

				INT32 i32UserIdx = 0;
				PacketBattleStartAck RoomPacket;
				RoomPacket.m_ui32MapIdx	= _GetBestMap();
		
				for( INT32 j = 0 ; j < NET_ROOM_USER_COUNT ; j++ )
				{
					// 현재 유저가 존재한다면...
					if( NET_ROOM_SLOT_STATE_PLAYING != m_User[j].m_eSlotState ) continue;

					RoomPacket.m_StartSlot[i32UserIdx].m_i8SlotState	= m_User[j].m_eSlotState;
					RoomPacket.m_StartSlot[i32UserIdx].m_i8TeamIdx		= m_User[j].m_i8TeamIdx;
					RoomPacket.m_StartSlot[i32UserIdx].m_i8SlotIdx		= m_User[j].m_i8SlotIdx;

					i32UserIdx++;
				}
				_SendPacket( &RoomPacket, pSlotInfo->m_pUserList );

				PacketRoomSlotStateChangeAck SlotPacket;
				pSlotInfo->Copy( &SlotPacket.m_SlotBase[ 0 ] );
				_SendPacketAll( NET_ROOM_SLOT_STATE_ADD_ALLUSER, &SlotPacket );
			}
			break;
		}
	}
}

void CRoom::_SetStateChangeOpen()
{
	m_Room.m_eRoomState			= NET_ROOM_STATE_OPEN;

	PacketRoomStateChangeAck RoomPacket;
	RoomPacket.m_i8State			= (INT8)m_Room.m_eRoomState;
	RoomPacket.m_ui8ReminTimeSec	= (UINT8)0;
	_SendPacketAll( NET_ROOM_SLOT_STATE_ADD_ALLUSER, &RoomPacket );

	INT32 i32UserIdx = 0;
	PacketRoomSlotStateChangeAck SlotPacket;
	for( INT32 i = 0 ; i < NET_ROOM_USER_COUNT ; i++ )
	{
		if( NET_ROOM_SLOT_STATE_WAITING > m_User[ i ].m_eSlotState ) continue;

		m_User[ i ].m_eSlotState = NET_ROOM_SLOT_STATE_WAITING;
		m_User[ i ].Copy( &SlotPacket.m_SlotBase[ i32UserIdx ] );
		i32UserIdx++;
	}
	_SendPacketAll( NET_ROOM_SLOT_STATE_ADD_ALLUSER, &SlotPacket );

	DLG_SENDMESSAGE( DLG_MESSAGE_ROOM_UPDATE, (LPARAM)this );
}

void CRoom::_SetStateChangeReady()
{
	m_Room.m_eRoomState				= NET_ROOM_STATE_READY;

	m_dt32NextStartTime				= S2Time::StandTime();
	m_dt32NextStartTime.AddTime( S2_DATE_ADD_TYPE_SECOND, (ROOM_TIME_GAME_READY) );

	PacketRoomStateChangeAck RoomPacket;
	RoomPacket.m_i8State			= m_Room.m_eRoomState;
	RoomPacket.m_ui8ReminTimeSec	= _GetReminTime();
	_SendPacketAll( NET_ROOM_SLOT_STATE_ADD_ALLUSER, &RoomPacket );
	
	DLG_SENDMESSAGE( DLG_MESSAGE_ROOM_UPDATE, (LPARAM)this );
}

void CRoom::_SetStateChangeCountdown()
{
	m_Room.m_eRoomState = NET_ROOM_STATE_COUNTDOWN;

	m_dt32NextStartTime		= S2Time::StandTime();
	m_dt32NextStartTime.AddTime( S2_DATE_ADD_TYPE_SECOND, ROOM_TIME_GAME_COUNTDOWN );

	// Room 에 대한 패킷
	PacketRoomStateChangeAck RoomPacket;
	RoomPacket.m_i8State			= m_Room.m_eRoomState;
	RoomPacket.m_ui8ReminTimeSec	= _GetReminTime();
	_SendPacketAll( NET_ROOM_SLOT_STATE_ADD_READY, &RoomPacket );

	// Slot에 대한 패킷
	INT32 i32UserIdx = 0;
	PacketRoomSlotStateChangeAck SlotPacket;
	for( INT32 i = 0 ; i < NET_ROOM_USER_COUNT ; i++ )
	{
		// 현재 유저가 존재한다면...
		if( NET_ROOM_SLOT_STATE_WAITING > m_User[i].m_eSlotState ) continue;

		m_User[i].m_eSlotState = NET_ROOM_SLOT_STATE_READY;
		m_User[i].Copy( &SlotPacket.m_SlotBase[ i32UserIdx ] );
		i32UserIdx++;
	}
	_SendPacketAll( NET_ROOM_SLOT_STATE_ADD_ALLUSER, &SlotPacket );

	// Dialog 변경
	DLG_SENDMESSAGE( DLG_MESSAGE_ROOM_UPDATE, (LPARAM)this );
}

void CRoom::_SetStateChangePlaying()
{
	m_Room.m_eRoomState = NET_ROOM_STATE_PLAYING;

	INT32 i32UserIdx = 0;
	PacketBattleStartAck RoomPacket;
	RoomPacket.m_ui32MapIdx	= _GetBestMap();

	PacketRoomSlotStateChangeAck SlotPacket;
	for( INT32 i = 0 ; i < NET_ROOM_USER_COUNT ; i++ )
	{
		// 현재 유저가 존재한다면...
		if( NET_ROOM_SLOT_STATE_READY != m_User[i].m_eSlotState ) continue;

		m_User[i].m_eSlotState	= NET_ROOM_SLOT_STATE_PLAYING;

		RoomPacket.m_StartSlot[i32UserIdx].m_i8SlotState	= m_User[i].m_eSlotState;
		RoomPacket.m_StartSlot[i32UserIdx].m_i8TeamIdx		= m_User[i].m_i8TeamIdx;
		RoomPacket.m_StartSlot[i32UserIdx].m_i8SlotIdx		= m_User[i].m_i8SlotIdx;

		m_User[i].Copy( &SlotPacket.m_SlotBase[ i32UserIdx ] );

		i32UserIdx++;
	}
	_SendPacketAll( NET_ROOM_SLOT_STATE_PLAYING, &RoomPacket );
	_SendPacketAll( NET_ROOM_SLOT_STATE_ADD_ALLUSER, &SlotPacket );

	DLG_SENDMESSAGE( DLG_MESSAGE_ROOM_UPDATE, (LPARAM)this );
}

T_RESULT CRoom::RoomCreate( PacketRoomCreateReq* pRecv, USER_LIST* pUser )
{
	if( NET_ROOM_STATE_CLOSE != m_Room.m_eRoomState )				return NET_RESULT_ERROR_SYSTEM;

	if( FALSE == ChangeRoomOption( &pRecv->m_RoomCreateInfo ) )		return NET_RESULT_ERROR_PACKET;
	
	// RoomState 변경은 모든 예외처리가 끝난 마지막에 해주세요.
	m_Room.m_eRoomState		= NET_ROOM_STATE_OPEN;

	PacketRoomCreateAck Send;
	Send.m_TResult					= NET_RESULT_SUCCESS;
	Send.m_ui32RoomIdx				= m_ui32Idx;
	_SendPacket( &Send, pUser );

	m_dt32NextStartTime		= S2Time::StandTime();
	m_dt32NextStartTime.AddTime( S2_DATE_ADD_TYPE_SECOND, (ROOM_TIME_GAME_READY) );

	if( S2MO_IS_FAIL( UserEnter( pUser ) ) )
	{
		// 나오면 안되는 상황
		S2ASSERT(0);
	}
	
	DLG_SENDMESSAGE( DLG_MESSAGE_ROOM_CREATE, (LPARAM)this );

	return NET_RESULT_SUCCESS;
}

BOOL CRoom::RoomDestroy()
{
	for( INT32 i = 0 ; i < NET_ROOM_USER_COUNT ; i++ )
	{
		// 현재 유저가 존재한다면...
		if( NET_ROOM_SLOT_STATE_OPEN < m_User[i].m_eSlotState )
		{
			// 강제로 유저들을 내보낸다. 이런경우가 나오는건 말이 안됨..
		}
	}
	// 그리고 방을 닫는다.
	_Clear();
	DLG_SENDMESSAGE( DLG_MESSAGE_ROOM_DESTROY, (LPARAM)this );

	return TRUE;
}

T_RESULT CRoom::UserEnter( USER_LIST* pUser )
{
	// 유저가 들어올수 있는 최대 수를 넘었는지
	if( (m_Room.m_i8MemberCount + 1) > m_Room.m_i8MemberMaxCount )	return NET_RESULT_ERROR_SYSTEM;

	INT32 i32Idx = -1;
	for( INT32 i = 0 ; i < NET_ROOM_USER_COUNT ; i++ )
	{
		if( NET_ROOM_SLOT_STATE_OPEN != m_User[i].m_eSlotState )	continue;

		i32Idx = i;
		break;
	}

	// 남는 공간이 없습니다.
	if( -1 == i32Idx )	return NET_RESULT_ERROR_SYSTEM;

	pUser->m_i32RoomIdx			= m_ui32Idx;
	pUser->m_i32SlotIdx			= i32Idx;

	SlotInfo* pSlot = &m_User[ i32Idx ];

	S2String::Copy( pSlot->m_strNick, pUser->m_UserBase.m_strNickname, NET_NICK_LENGTH );
	pSlot->m_ui16Level				= pUser->m_UserBase.m_ui16Level;
	pSlot->m_i8TeamIdx				= i32Idx % m_ui8TeamCount;
	INT32 i32TeamIdxNormalize		= i32Idx - pSlot->m_i8TeamIdx;
	pSlot->m_i8SlotIdx				= i32TeamIdxNormalize / m_ui8TeamCount;
	pSlot->m_pUserList				= pUser;
	switch( m_Room.m_eRoomState )
	{
	case NET_ROOM_STATE_COUNTDOWN:		// 난입 상태
	case NET_ROOM_STATE_PLAYING:
		{
			pSlot->m_eSlotState		= NET_ROOM_SLOT_STATE_INTRUSION;
			pSlot->m_dt32StartTime	= S2Time::StandTime();
			pSlot->m_dt32StartTime.AddTime( S2_DATE_ADD_TYPE_SECOND, ROOM_TIME_GAME_COUNTDOWN );
		}
		break;
	default:
		{
			pSlot->m_eSlotState		= NET_ROOM_SLOT_STATE_WAITING;
		}
		break;;
	}

	m_Room.m_i8MemberCount++;

	PacketRoomEnterAck Send;
	Send.m_TResult					= NET_RESULT_SUCCESS;
	pSlot->Copy( &Send.m_SlotInfo );
	Send.m_ui8ReminTimeSec			= _GetReminTime( pSlot );
	m_Room.Copy( &Send.m_RoomInfo );
	Send.m_RoomInfo.m_ui8State		= m_Room.m_eRoomState;

	for( INT32 i = 0 ; i < NET_ROOM_MAP_LIST_COUNT ; i++ )
	{
		if( 0 < m_MapList[i].m_ui32MapIdx )
		{
			m_MapList[i].Copy( &Send.m_MapInfo[i] );
		}
	}
	_SendPacket( &Send, pSlot->m_pUserList );

	INT32 i32UserIdx = 0;
	PacketRoomSlotInfoAck SlotInfo;
	SlotInfo.m_TResult			= NET_RESULT_SUCCESS;

	PacketRoomEnterTransAck SendTrans;
	pSlot->Copy( &SendTrans.m_SlotInfo );
		
	for( INT32 i = 0 ; i < NET_ROOM_USER_COUNT ; i++ )
	{
		if( i32Idx == i )											continue;
		if( NET_ROOM_SLOT_STATE_WAITING > m_User[i].m_eSlotState )	continue;
		
		// 방에 있는 유저들의 정보를 얻는다.
		m_User[i].Copy( &SlotInfo.m_SlotInfo[ i32UserIdx ] );
		i32UserIdx++;
		
		// 다른 유저들에게 입장을 전달한다.
		_SendPacket( &SendTrans, m_User[i].m_pUserList );
	}

	// 유저들의 정보 전달
	if( 0 < i32UserIdx )
	{
		_SendPacket( &SlotInfo, pSlot->m_pUserList );
	}

	DLG_SENDMESSAGE( DLG_MESSAGE_ROOM_UPDATE, (LPARAM)this );

	return NET_RESULT_SUCCESS;
}

T_RESULT CRoom::UserLeave( UINT8 ui8SlotIdx )
{
	if( NET_ROOM_USER_COUNT <= ui8SlotIdx )									return NET_RESULT_ERROR_PACKET;
	if( NET_ROOM_SLOT_STATE_WAITING > m_User[ ui8SlotIdx ].m_eSlotState )	return NET_RESULT_ERROR_PACKET;

	m_User[ ui8SlotIdx ].m_eSlotState = NET_ROOM_SLOT_STATE_OPEN;	

	USER_LIST* pUser = m_User[ ui8SlotIdx ].m_pUserList;

	PacketRoomLeaveAck Send;
	Send.m_TResult			= NET_RESULT_SUCCESS;
	_SendPacket( &Send, pUser );

	m_Room.m_i8MemberCount--;
	if( 0 > m_Room.m_i8MemberCount )
	{ 
		S2ASSERT(0);
	}

	DLG_SENDMESSAGE( DLG_MESSAGE_ROOM_UPDATE, (LPARAM)this );

	if( 0 < m_Room.m_i8MemberCount )
	{
		PacketRoomLeaveTransAck SendTrans;
		SendTrans.m_SlotBase.m_i8TeamIdx			= ui8SlotIdx % m_ui8TeamCount;
		INT32 i32TeamIdxNormalize					= ui8SlotIdx - SendTrans.m_SlotBase.m_i8TeamIdx;
		SendTrans.m_SlotBase.m_i8SlotIdx			= i32TeamIdxNormalize / m_ui8TeamCount;		
		SendTrans.m_SlotBase.m_i8SlotState			= NET_ROOM_SLOT_STATE_OPEN;
		for( INT32 i = 0 ; i < NET_ROOM_USER_COUNT ; i++ )
		{
			if( NET_ROOM_SLOT_STATE_WAITING > m_User[i].m_eSlotState )	continue;
			
			// 다른 유저들에게 퇴장을 전달한다.
			_SendPacket( &SendTrans, m_User[i].m_pUserList );
		}
	}
	else
	{
		// 유저가 없기 때문에 방을 닫는다.
		_Clear();
		DLG_SENDMESSAGE( DLG_MESSAGE_ROOM_DESTROY, (LPARAM)this );
	}

	return NET_RESULT_SUCCESS;
}

T_RESULT CRoom::UserReady( USER_LIST* pUser )
{
	UINT8 ui8SlotIdx = pUser->m_i32SlotIdx;
	if( NET_ROOM_PLAYER_USER_COUNT <= ui8SlotIdx )							return NET_RESULT_ERROR_PACKET;
	if( NET_ROOM_STATE_READY != m_Room.m_eRoomState )						return NET_RESULT_ERROR_PACKET;
	if( NET_ROOM_SLOT_STATE_WAITING != m_User[ ui8SlotIdx ].m_eSlotState )	return NET_RESULT_ERROR_PACKET;

	m_User[ ui8SlotIdx ].m_eSlotState = NET_ROOM_SLOT_STATE_READY;

	PacketRoomSlotReadyAck Send;
	Send.m_TResult = NET_RESULT_SUCCESS;
	_SendPacket( &Send, m_User[ ui8SlotIdx ].m_pUserList );
	
	PacketRoomSlotStateChangeAck SendTrans;
	SendTrans.m_SlotBase[ 0 ].m_i8TeamIdx	= m_User[ ui8SlotIdx ].m_i8TeamIdx;
	SendTrans.m_SlotBase[ 0 ].m_i8SlotIdx	= m_User[ ui8SlotIdx ].m_i8SlotIdx;
	SendTrans.m_SlotBase[ 0 ].m_i8SlotState	= m_User[ ui8SlotIdx ].m_eSlotState;
	_SendPacketAll( NET_ROOM_SLOT_STATE_ADD_ALLUSER, &SendTrans );

	BOOL bFreePass = TRUE;
	for( INT32 i = 0 ; i < NET_ROOM_USER_COUNT ; i++ )
	{
		if( NET_ROOM_SLOT_STATE_WAITING > m_User[ i ].m_eSlotState ) continue;
		if( NET_ROOM_SLOT_STATE_READY != m_User[ i ].m_eSlotState )
		{
			bFreePass = FALSE;
			break;
		}
	}
	if( bFreePass )
	{
		// 곧바로 게임을 시작한다.
		_SetStateChangeCountdown();
	}
	
	return NET_RESULT_SUCCESS;
}

T_RESULT CRoom::MapVote( PacketRoomMapVoteReq* pRecv, UINT8 ui8SlotIdx )
{
	if( NET_ROOM_PLAYER_USER_COUNT <= ui8SlotIdx )							return NET_RESULT_ERROR_PACKET;
	if( NET_ROOM_STATE_READY != m_Room.m_eRoomState )						return NET_RESULT_ERROR_PACKET;
	if( NET_ROOM_SLOT_STATE_WAITING > m_User[ ui8SlotIdx ].m_eSlotState )	return NET_RESULT_ERROR_PACKET;
#ifndef _DEBUG
	if( 0 != m_UserBattle[ ui8SlotIdx ].m_ui32MapVote )						return NET_RESULT_ERROR_PACKET;
#endif

	// 투표 인증
	m_UserBattle[ ui8SlotIdx ].m_ui32MapVote = pRecv->m_ui32MapIdx;
	
	INT32 i32Idx = -1;
	for( INT32 i = 0 ; i < NET_ROOM_PLAYER_USER_COUNT ; i++ )
	{
		if( (0 == m_MapList[i].m_ui32MapIdx) ||
			(m_MapList[i].m_ui32MapIdx == pRecv->m_ui32MapIdx ) )
		{
			m_MapList[i].m_ui32MapIdx	= pRecv->m_ui32MapIdx;
			m_MapList[i].m_ui8VoteCount++;
			i32Idx = i;
			break;
		}
	}
	if( -1 == i32Idx )
	{
#ifdef _DEBUG
		S2ASSERT( 0 );
#else
		return NET_RESULT_ERROR_SYSTEM;
#endif // _DEBUG		
	}

	PacketRoomMapVoteAck Send;
	Send.m_TResult = NET_RESULT_SUCCESS;
	_SendPacket( &Send, m_User[ ui8SlotIdx ].m_pUserList );

	for( INT32 i = 0 ; i < i32Idx ; i++ )
	{
		if( m_MapList[i].m_ui8VoteCount < m_MapList[ i32Idx ].m_ui8VoteCount )
		{
			MapInfo VoteMap;
			S2Memory::Copy( &VoteMap,			&m_MapList[ i32Idx ],	sizeof(MapInfo) );
			S2Memory::Copy( &m_MapList[i+1],	&m_MapList[i],			sizeof(MapInfo)*(i32Idx-i) );
			S2Memory::Copy( &m_MapList[i],		&VoteMap,				sizeof(MapInfo) );
			break;
		}
	}

	PacketRoomMapVoteTransAck SendTrans;
	for( INT32 i = 0 ; i < NET_ROOM_MAP_LIST_COUNT ; i++ )
	{
		if( 0 < m_MapList[i].m_ui32MapIdx )
		{
			m_MapList[i].Copy( &SendTrans.m_MapInfo[i] );
		}
	}
	_SendPacketAll( NET_ROOM_SLOT_STATE_ADD_ALLUSER, &SendTrans );

	return NET_RESULT_SUCCESS;
}


T_RESULT CRoom::BattleReady( USER_LIST* pUser )
{
	return NET_RESULT_SUCCESS;
}

T_RESULT CRoom::BattleEnd( USER_LIST* pUser )
{
	UINT8 ui8SlotIdx = pUser->m_i32SlotIdx;
	if( NET_ROOM_PLAYER_USER_COUNT <= ui8SlotIdx )							return NET_RESULT_ERROR_PACKET;
	if( NET_ROOM_SLOT_STATE_PLAYING != m_User[ ui8SlotIdx ].m_eSlotState )	return NET_RESULT_ERROR_PACKET;

	PacketBattleEndAck Packet;
	Packet.m_TResult = NET_RESULT_SUCCESS;
	_SendPacketAll( NET_ROOM_SLOT_STATE_ADD_ALLUSER, &Packet );

	_BattleClear();
	_SetStateChangeOpen();

	return NET_RESULT_SUCCESS;
}

BOOL CRoom::ChangeRoomOption( NET_ROOM_CREATE_INFO* pRoomInfo )
{
	if( FALSE == _CheckRoomInfo( pRoomInfo ) )								return FALSE;

	pRoomInfo->m_strTitle.GetValue( m_Room.m_strTitle, NET_ROOM_TITLE_LENGTH );
	m_Room.m_eRoomMode					= (NET_ROOM_MODE)((INT8)pRoomInfo->m_i8RoomMode);
	m_Room.m_ui16TimeToRoundSec			= pRoomInfo->m_ui16TimeToRoundSec;
	m_Room.m_i8MemberMaxCount			= pRoomInfo->m_ui8MemberCountOfTeam * m_ui8TeamCount;

	S2Memory::FillZero( m_MapList,	sizeof(MapInfo)*NET_ROOM_PLAYER_USER_COUNT );
	//Map Default Data
	LOAD_MAP_INFO* pMapInfo =			&g_pConfig->m_pMapInfo[ m_Room.m_eRoomMode ];
	for( INT32 i = 0 ; (i < NET_ROOM_MAP_LIST_COUNT) && (i < pMapInfo->m_i32MapCount) ; i++ )
	{
		m_MapList[ i ].m_ui32MapIdx = pMapInfo->m_pMapID[ i ];
	}

	return TRUE;
}

BOOL CRoom::_CheckRoomInfo( NET_ROOM_CREATE_INFO* pRoomInfo )
{
	// Checking RoomMode
	if( NET_ROOM_MODE_NONE >= pRoomInfo->m_i8RoomMode )										return FALSE;
	if( NET_ROOM_MODE_MAX <= pRoomInfo->m_i8RoomMode )										return FALSE;

	// 모드에 따른 기본 세팅
	_SetRoomInfo( (NET_ROOM_MODE)(INT8)(pRoomInfo->m_i8RoomMode) );

	// 한 팀당 들어갈 수 있는 유저수 체크
	if( m_ui8MemberMaxCountOfTeam < pRoomInfo->m_ui8MemberCountOfTeam )						return FALSE;
	// 맴버가 이미 너무 많을땐 변경 실패
	if( (pRoomInfo->m_ui8MemberCountOfTeam * m_ui8TeamCount) < m_Room.m_i8MemberCount )		return FALSE;

	return TRUE;
}

void CRoom::_SetRoomInfo( NET_ROOM_MODE eRoomMode )
{
	switch( eRoomMode )
	{
	case NET_ROOM_MODE_DEATHMATCH:
	case NET_ROOM_MODE_BOMBMISSION:
		m_ui8TeamCount					= 2;
		m_ui8MemberMaxCountOfTeam		= 8;
		break;
	}
}