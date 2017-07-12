#ifndef __ROOM_H__
#define __ROOM_H__

#include "UserMgr.h"
#include "SessionMgr.h"

#ifdef _DEBUG
#define ROOM_TIME_GAME_READY			200
#define ROOM_TIME_GAME_COUNTDOWN		3
#else
#define ROOM_TIME_GAME_READY			50
#define ROOM_TIME_GAME_COUNTDOWN		10
#endif

struct RoomInfo
{
	NET_ROOM_STATE			m_eRoomState;
	NET_ROOM_MODE			m_eRoomMode;

	UINT16					m_ui16TimeToRoundSec;

	INT8					m_i8MemberCount;
	INT8					m_i8MemberMaxCount;
	UINT8					m_ui8MemberCountOfTeam;

	TCHAR					m_strTitle[ NET_ROOM_TITLE_LENGTH ];
	INT8					m_i8Passworld[ NET_ROOM_PASSWORLD_LENGTH ];

	void Clear()
	{
		m_eRoomState		= NET_ROOM_STATE_CLOSE;
		m_eRoomMode			= NET_ROOM_MODE_NONE;
		m_strTitle[0]		= '\0';
		m_i8MemberCount		= 0;
	};

	void Copy( NET_ROOM_INFO* pRoomInfo )
	{
		pRoomInfo->m_strTitle				= m_strTitle;
		pRoomInfo->m_i8RoomMode				= m_eRoomMode;
		pRoomInfo->m_ui8UserCountCur		= m_i8MemberCount;
		pRoomInfo->m_ui8UserCountMax		= m_i8MemberMaxCount;
		pRoomInfo->m_ui8State				= m_eRoomState;
	};
};

struct SlotInfo
{
	NET_ROOM_SLOT_STATE		m_eSlotState;
	INT8					m_i8SlotIdx;
	INT8					m_i8TeamIdx;
	TCHAR					m_strNick[ NET_NICK_LENGTH ];
	UINT16					m_ui16Level;
	USER_LIST*				m_pUserList;
	DATE32					m_dt32StartTime;
	
	void Copy( NET_ROOM_SLOT_INFO* pSlotInfo )
	{
		pSlotInfo->m_i8SlotIdx				= m_i8SlotIdx;
		pSlotInfo->m_i8TeamIdx				= m_i8TeamIdx;
		pSlotInfo->m_i8SlotState			= m_eSlotState;
		pSlotInfo->m_strNickname			= m_strNick;
		pSlotInfo->m_ui16Level				= m_ui16Level;
	}
	void Copy( NET_ROOM_SLOT_BASE* pSlotInfo )
	{
		pSlotInfo->m_i8SlotIdx				= m_i8SlotIdx;
		pSlotInfo->m_i8TeamIdx				= m_i8TeamIdx;
		pSlotInfo->m_i8SlotState			= m_eSlotState;
	}
};

struct BattleSlotInfo
{
	UINT32					m_ui32MapVote;

	void Reset()
	{
		m_ui32MapVote		= 0;
	}
};

struct MapInfo
{
	UINT32					m_ui32MapIdx;
	UINT8					m_ui8VoteCount;

	void Copy( NET_ROOM_MAP_INFO* pInfo )
	{
		pInfo->m_ui32MapIdx		= m_ui32MapIdx;
		pInfo->m_ui8VoteCount	= m_ui8VoteCount;
	}
};

class  CRoom
{
	CSessionMgr*			m_pSessionMgr;

	UINT32					m_ui32Idx;
	RoomInfo				m_Room;
	SlotInfo				m_User[ NET_ROOM_USER_COUNT ];
	BattleSlotInfo			m_UserBattle[ NET_ROOM_PLAYER_USER_COUNT ];

	DATE32					m_dt32NextStartTime;
	UINT8					m_ui8TeamCount;
		
	UINT8					m_ui8MemberMaxCountOfTeam;

	MapInfo					m_MapList[ NET_ROOM_PLAYER_USER_COUNT ];

protected:
	void					_Clear();
	void					_BattleClear();
	BOOL					_CheckRoomInfo( NET_ROOM_CREATE_INFO* pRoomInfo );
	void					_SetRoomInfo( NET_ROOM_MODE eRoomMode );
	BOOL					_SendPacket( S2MOPacketBase* pPacket, USER_LIST* pUser );
	BOOL					_SendPacketAll( UINT32 ui32State, S2MOPacketBase* pPacket );

	void					_OnUpdateOpen();
	void					_OnUpdateReady();
	void					_OnUpdateCountdown();
	void					_OnUpdatePlaying();

	void					_SetStateChangeOpen();
	void					_SetStateChangeReady();
	void					_SetStateChangeCountdown();
	void					_SetStateChangePlaying();

	UINT8					_GetReminTime( SlotInfo* pSlot = NULL )
	{
		if( pSlot )
		{
			return (UINT8)(pSlot->m_dt32StartTime.DiffTime( S2Time::StandTime() ));
		}
		else
		{
			switch( m_Room.m_eRoomState )
			{
			case NET_ROOM_STATE_READY:
				return (UINT8)(m_dt32NextStartTime.DiffTime( S2Time::StandTime() ) + ROOM_TIME_GAME_COUNTDOWN);
			case NET_ROOM_STATE_COUNTDOWN:
				return (UINT8)(m_dt32NextStartTime.DiffTime( S2Time::StandTime() ));
			}
			return ROOM_TIME_GAME_READY + ROOM_TIME_GAME_COUNTDOWN;
		}
	}

	NET_ROOM_STATE			_GetRoomState()
	{
		switch( m_Room.m_eRoomState )
		{
		case NET_ROOM_STATE_PLAYING:
			return NET_ROOM_STATE_COUNTDOWN;
		default:
			return m_Room.m_eRoomState;
		}
	}

	inline UINT32			_GetBestMap();

public:
	CRoom();
	~CRoom();

	BOOL					Create( UINT32 ui32Idx, CSessionMgr* pSessionMgr );
	void					Destroy();

	void					OnUpdate();

	T_RESULT				RoomCreate( PacketRoomCreateReq* pRecv, USER_LIST* pUser );
	BOOL					RoomDestroy();

	T_RESULT				UserEnter( USER_LIST* pUser );
	T_RESULT				UserLeave( UINT8 ui8SlotIdx );
	T_RESULT				UserReady( USER_LIST* pUser );

	T_RESULT				MapVote( PacketRoomMapVoteReq* pRecv, UINT8 ui8SlotIdx );
	T_RESULT				BattleReady( USER_LIST* pUser );
	T_RESULT				BattleEnd( USER_LIST* pUser );
	BOOL					ChangeRoomOption( NET_ROOM_CREATE_INFO* pRoomInfo );

	NET_ROOM_STATE			GetRoomState()					{	return m_Room.m_eRoomState;											}

	UINT32					GetIdx()						{	return m_ui32Idx;													}
	RoomInfo*				GetRoomInfo()					{	return &m_Room;														}
	UINT32					GetReminTime()					{	return m_dt32NextStartTime.DiffTime( S2Time::StandTime() );			}

	void					Copy( NET_ROOM_INFO* pData )
	{
		m_Room.Copy( pData );
		pData->m_ui32RoomIdx	= m_ui32Idx;
	}	
};

#endif
