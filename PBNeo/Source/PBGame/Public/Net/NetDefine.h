#ifndef __NET_DEFINE_H__
#define __NET_DEFINE_H__

#define USE_UNREAL4

#include "S2MODataType.h"

typedef		int							T_ItemID;
typedef		int							T_MapID;


#define NET_IP_LENGTH					16
#define NET_NOTICE_LENGTH				100

// for User
#define NET_ID_LENGTH					64
#define NET_PW_LENGTH					16
#define NET_NICK_LENGTH					16

// for Room
#define NET_ROOM_TITLE_LENGTH			16
#define NET_ROOM_PASSWORLD_LENGTH		4	
#define NET_ROOM_PLAYER_USER_COUNT		16
#define NET_ROOM_OBSERVER_USER_COUNT	4
#define NET_ROOM_USER_COUNT				NET_ROOM_PLAYER_USER_COUNT+NET_ROOM_OBSERVER_USER_COUNT
#define NET_ROOM_LIST_PAGE_COUNT		10
#define NET_ROOM_MAP_LIST_COUNT			3

enum NET_NOTICE_TYPE
{
	NET_NOTICE_TYPE_NORMAL,
};

enum NET_ROOM_MODE
{
	NET_ROOM_MODE_NONE			= -1,

	NET_ROOM_MODE_DEATHMATCH,
	NET_ROOM_MODE_BOMBMISSION,

	NET_ROOM_MODE_MAX,
};

enum NET_ROOM_STATE
{
	NET_ROOM_STATE_CLOSE,
	NET_ROOM_STATE_OPEN,					// 방만 생성
	NET_ROOM_STATE_READY,					// 60 ~ 10초전
	NET_ROOM_STATE_COUNTDOWN,				// 10 ~ 0초전
	NET_ROOM_STATE_PLAYING,					// Play
};

enum NET_ROOM_SLOT_STATE
{
	NET_ROOM_SLOT_STATE_CLOSE			= 0,				// Slot 닫힘
	NET_ROOM_SLOT_STATE_OPEN			= 0x01,				// Slot Open
	NET_ROOM_SLOT_STATE_WAITING			= 0x02,				// User 입장
	NET_ROOM_SLOT_STATE_READY			= 0x04,				// User Ready
	NET_ROOM_SLOT_STATE_INTRUSION		= 0x08,				// User 난입 ( RoomState == NET_ROOM_STATE_COUNTDOWN 시에 입장한 유저 )
	NET_ROOM_SLOT_STATE_PLAYING			= 0x10,				// Playing

	NET_ROOM_SLOT_STATE_ADD_READY		= NET_ROOM_SLOT_STATE_WAITING|NET_ROOM_SLOT_STATE_READY,
	NET_ROOM_SLOT_STATE_ADD_ALLUSER		= NET_ROOM_SLOT_STATE_WAITING|NET_ROOM_SLOT_STATE_READY|NET_ROOM_SLOT_STATE_INTRUSION|NET_ROOM_SLOT_STATE_PLAYING,
};

enum NET_ROOM_SLOT_TYPE
{
	NET_ROOM_SLOT_TYPE_PLAYER,
	NET_ROOM_SLOT_TYPE_OBSERVER,
};

// 기본 정보로 방에서 교환을 위해 사용됨
struct NET_USER_BASE : public S2MOStructBase
{
	S2MOStringW<NET_NICK_LENGTH>					m_strNickname;
	S2MOINT16										m_ui16Level;

	NET_USER_BASE()
	{
		_SetValue( m_strNickname );
		_SetValue( m_ui16Level );
	}
};

// 기본 정보
struct NET_USER_INFO : public S2MOStructBase
{
	S2MOINT32										m_ui32Exp;

	NET_USER_INFO()
	{
		_SetValue( m_ui32Exp );
	}
};

struct NET_USER_MONEY : public S2MOStructBase
{
	S2MOINT32										m_ui32Point;
	S2MOINT32										m_ui32RealCash;
	S2MOINT32										m_ui32BonusCash;

	NET_USER_MONEY()
	{
		_SetValue( m_ui32Point );
		_SetValue( m_ui32RealCash );
		_SetValue( m_ui32BonusCash );
	}
};

struct NET_USER_RECORD : public S2MOStructBase
{
	S2MOINT32										m_ui32Match;
	S2MOINT32										m_ui32Win;
	S2MOINT32										m_ui32Lose;

	S2MOINT32										m_ui32Kill;
	S2MOINT32										m_ui32Death;
	S2MOINT32										m_ui32Assist;

	S2MOINT32										m_ui32HeadShot;
	S2MOINT32										m_ui32PlayTime;
	S2MOINT32										m_ui32MVPCount;
	S2MOINT32										m_ui32HelmetProtect;
	S2MOINT32										m_ui32BestScore;
	S2MOINT32										m_ui32BestDamage;

	S2MOINT32										m_ui32RankPoint;


	NET_USER_RECORD()
	{
		_SetValue( m_ui32Match );
		_SetValue( m_ui32Win );
		_SetValue( m_ui32Lose );
		_SetValue( m_ui32Kill );
		_SetValue( m_ui32Death );
		_SetValue( m_ui32Assist );
		_SetValue( m_ui32HeadShot );
		_SetValue( m_ui32PlayTime );
		_SetValue( m_ui32MVPCount );
		_SetValue( m_ui32HelmetProtect );
		_SetValue( m_ui32BestScore );
		_SetValue( m_ui32BestDamage );
		_SetValue( m_ui32RankPoint );
	}
};

struct NET_USER_EQUIPMENT : public S2MOStructBase
{
	S2MOINT32										m_TWPPrimary;
	S2MOINT32										m_TWPSecondary;
	S2MOINT32										m_TWPThrowing1;
	S2MOINT32										m_TWPThrowing2;

	S2MOINT32										m_TCHAlpha;
	S2MOINT32										m_TCHBravo;

	NET_USER_EQUIPMENT()
	{
		_SetValue( m_TWPPrimary );
		_SetValue( m_TWPSecondary );
		_SetValue( m_TWPThrowing1 );
		_SetValue( m_TWPThrowing2 );
		_SetValue( m_TCHAlpha );
		_SetValue( m_TCHBravo );
	}
};

struct NET_ROOM_CREATE_INFO : public S2MOStructBase
{
	S2MOStringW<NET_ROOM_TITLE_LENGTH>				m_strTitle;
	S2MOINT8										m_i8RoomMode;
	S2MOUINT16										m_ui16TimeToRoundSec;
	S2MOUINT8										m_ui8MemberCountOfTeam;

	NET_ROOM_CREATE_INFO()
	{
		_SetValue( m_strTitle );
		_SetValue( m_i8RoomMode );
		_SetValue( m_ui16TimeToRoundSec );
		_SetValue( m_ui8MemberCountOfTeam );
	}
};

struct NET_ROOM_INFO : public S2MOStructBase
{
	S2MOUINT32										m_ui32RoomIdx;
	S2MOStringW<NET_ROOM_TITLE_LENGTH>				m_strTitle;
	S2MOINT8										m_i8RoomMode;
	S2MOUINT8										m_ui8UserCountCur;
	S2MOUINT8										m_ui8UserCountMax;
	S2MOUINT8										m_ui8State;									// NET_ROOM_STATE

	NET_ROOM_INFO()
	{
		_SetValue( m_ui32RoomIdx );
		_SetValue( m_strTitle );
		_SetValue( m_i8RoomMode );
		_SetValue( m_ui8UserCountCur );
		_SetValue( m_ui8UserCountMax );
		_SetValue( m_ui8State );
	};
};

struct NET_ROOM_ENTER_INFO : public S2MOStructBase
{
	S2MOINT8										m_i8RoomMode;
	S2MOUINT16										m_ui16TimeToRoundSec;
	S2MOUINT8										m_ui8MemberCountOfTeam;

	NET_ROOM_ENTER_INFO()
	{
		_SetValue( m_i8RoomMode );
		_SetValue( m_ui16TimeToRoundSec );
		_SetValue( m_ui8MemberCountOfTeam );
	};
};

struct NET_ROOM_MAP_INFO : public S2MOStructBase
{
	S2MOUINT32										m_ui32MapIdx;	
	S2MOUINT8										m_ui8VoteCount;

	NET_ROOM_MAP_INFO()
	{
		_SetValue( m_ui32MapIdx );
		_SetValue( m_ui8VoteCount );
	};
};

struct NET_ROOM_SLOT_BASE : public S2MOStructBase
{
	S2MOINT8										m_i8TeamIdx;
	S2MOINT8										m_i8SlotIdx;
	S2MOINT8										m_i8SlotState;						// NET_ROOM_SLOT_STATE

	NET_ROOM_SLOT_BASE()
	{
		_SetValue( m_i8TeamIdx );
		_SetValue( m_i8SlotIdx );
		_SetValue( m_i8SlotState );
	};
};

struct NET_ROOM_SLOT_INFO : public S2MOStructBase
{
	S2MOINT8										m_i8TeamIdx;
	S2MOINT8										m_i8SlotIdx;
	S2MOINT8										m_i8SlotState;						// NET_ROOM_SLOT_STATE
	S2MOStringW<NET_NICK_LENGTH>					m_strNickname;
	S2MOINT16										m_ui16Level;

	NET_ROOM_SLOT_INFO()
	{
		_SetValue( m_i8TeamIdx );
		_SetValue( m_i8SlotIdx );
		_SetValue( m_i8SlotState );
		_SetValue( m_strNickname );
		_SetValue( m_ui16Level );
	};
};
#endif
