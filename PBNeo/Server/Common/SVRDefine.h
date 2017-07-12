#ifndef __SVR_DEFINE_H__
#define __SVR_DEFINE_H__

#define SERVER_HEART_BIT_TIME_SEC					120

enum SERVER_TYPE
{
	SERVER_TYPE_NONE,

	SERVER_TYPE_FRONT,
	SERVER_TYPE_CENTER,
	SERVER_TYPE_AUTH,
	SERVER_TYPE_DB,
	SERVER_TYPE_BATTLE_MGR,

	SERVER_TYPE_COUNT,
};

enum DB_RESULT
{
	DB_RESULT_SUCCESS		= 0,
	DB_RESULT_EMPTYROW		= 8888,					// 8888 이란 숫자는 DBA 와 임의로 협의된 숫자입니다. 20161229 김현우&송현수
	
};

struct LOAD_MAP_INFO
{
	INT32					m_i32MapCount;
	T_MapID*				m_pMapID;
};

struct USER_BASE
{
	TCHAR					m_strNickname[ NET_NICK_LENGTH ];
	INT16					m_ui16Level;
	
	void Reset()
	{
		m_strNickname[0]	= '\0';
		m_ui16Level			= 0;
	}
	void CopyTo( NET_USER_BASE& Base )
	{
		Base.m_strNickname	= m_strNickname;
		Base.m_ui16Level	= m_ui16Level;
	}
	virtual void			operator=( NET_USER_BASE& Base )
	{
		Base.m_strNickname.GetValue( m_strNickname, NET_NICK_LENGTH );
		m_ui16Level = Base.m_ui16Level;
	}
};

// 기본 정보
struct USER_INFO
{
	INT32					m_ui32Exp;

	void Reset()
	{
		m_ui32Exp			= 0;
	}
	void CopyTo( NET_USER_INFO& Base )
	{
		Base.m_ui32Exp	= m_ui32Exp;
	}
	virtual void			operator=( NET_USER_INFO& Base )
	{
		m_ui32Exp = Base.m_ui32Exp;
	}
};

struct USER_MONEY
{
	INT32					m_ui32Point;
	INT32					m_ui32RealCash;
	INT32					m_ui32BonusCash;

	void Reset()
	{
		m_ui32Point			= 0;
		m_ui32RealCash		= 0;
		m_ui32BonusCash		= 0;
	}
	void CopyTo( NET_USER_MONEY& Base )
	{
		Base.m_ui32Point		= m_ui32Point;
		Base.m_ui32RealCash		= m_ui32RealCash;
		Base.m_ui32BonusCash	= m_ui32BonusCash;
	}
	virtual void			operator=( NET_USER_MONEY& Base )
	{
		m_ui32Point			= Base.m_ui32Point;
		m_ui32RealCash		= Base.m_ui32RealCash;
		m_ui32BonusCash		= Base.m_ui32BonusCash;
	}
};

struct USER_RECORD
{
	UINT32					m_ui32Match;
	UINT32					m_ui32Win;
	UINT32					m_ui32Lose;

	UINT32					m_ui32Kill;
	UINT32					m_ui32Death;
	UINT32					m_ui32Assist;

	UINT32					m_ui32HeadShot;

	UINT32					m_ui32PlayTime;
	UINT32					m_ui32MVPCount;
	UINT32					m_ui32HelmetProtect;
	UINT32					m_ui32BestScore;
	UINT32					m_ui32BestDamage;
	
	UINT32					m_ui32RankPoint;

	void Reset()
	{
		m_ui32Match			= 0;
		m_ui32Win			= 0;
		m_ui32Lose			= 0;

		m_ui32Kill			= 0;
		m_ui32Death			= 0;
		m_ui32Assist		= 0;

		m_ui32HeadShot		= 0;

		m_ui32PlayTime		= 0;
		m_ui32MVPCount		= 0;
		m_ui32HelmetProtect	= 0;
		m_ui32BestScore		= 0;
		m_ui32BestDamage	= 0;

		m_ui32RankPoint		= 0;
	}
	void CopyTo( NET_USER_RECORD& Base )
	{
		Base.m_ui32Match			= m_ui32Match;
		Base.m_ui32Win				= m_ui32Win;
		Base.m_ui32Lose				= m_ui32Lose;

		Base.m_ui32Kill				= m_ui32Kill;
		Base.m_ui32Death			= m_ui32Death;
		Base.m_ui32Assist			= m_ui32Assist;

		Base.m_ui32HeadShot			= m_ui32HeadShot;
		Base.m_ui32PlayTime			= m_ui32PlayTime;
		Base.m_ui32MVPCount			= m_ui32MVPCount;
		Base.m_ui32HelmetProtect	= m_ui32HelmetProtect;
		Base.m_ui32BestScore		= m_ui32BestScore;
		Base.m_ui32BestDamage		= m_ui32BestDamage;

		Base.m_ui32RankPoint		= m_ui32RankPoint;
	}
	virtual void			operator=( NET_USER_RECORD& Base )
	{
		m_ui32Match				= Base.m_ui32Match;
		m_ui32Win				= Base.m_ui32Win;
		m_ui32Lose				= Base.m_ui32Lose;

		m_ui32Kill				= Base.m_ui32Kill;
		m_ui32Death				= Base.m_ui32Death;
		m_ui32Assist			= Base.m_ui32Assist;

		m_ui32HeadShot			= Base.m_ui32HeadShot;
		m_ui32PlayTime			= Base.m_ui32PlayTime;
		m_ui32MVPCount			= Base.m_ui32MVPCount;
		m_ui32HelmetProtect		= Base.m_ui32HelmetProtect;
		m_ui32BestScore			= Base.m_ui32BestScore;
		m_ui32BestDamage		= Base.m_ui32BestDamage;

		m_ui32RankPoint			= Base.m_ui32RankPoint;
	}
};

struct USER_EQUIPMENT
{
	INT32					m_TWPPrimary;
	INT32					m_TWPSecondary;
	INT32					m_TWPThrowing1;
	INT32					m_TWPThrowing2;

	INT32					m_TCHAlpha;
	INT32					m_TCHBravo;

	void Reset()
	{
		m_TWPPrimary			= 0;
		m_TWPSecondary			= 0;
		m_TWPThrowing1			= 0;
		m_TWPThrowing2			= 0;

		m_TCHAlpha				= 0;
		m_TCHBravo				= 0;
	}
	void CopyTo( NET_USER_EQUIPMENT& Base )
	{
		Base.m_TWPPrimary		= m_TWPPrimary;
		Base.m_TWPSecondary		= m_TWPSecondary;
		Base.m_TWPThrowing1		= m_TWPThrowing1;
		Base.m_TWPThrowing2		= m_TWPThrowing2;
		Base.m_TCHAlpha			= m_TCHAlpha;
		Base.m_TCHBravo			= m_TCHBravo;
	}
	virtual void			operator=( NET_USER_EQUIPMENT& Base )
	{
		m_TWPPrimary			= Base.m_TWPPrimary;
		m_TWPSecondary			= Base.m_TWPSecondary;
		m_TWPThrowing1			= Base.m_TWPThrowing1;
		m_TWPThrowing2			= Base.m_TWPThrowing2;
		m_TCHAlpha				= Base.m_TCHAlpha;
		m_TCHBravo				= Base.m_TCHBravo;
	}
};

struct ROOM_INFO
{
	TCHAR					m_strTitle[ NET_ROOM_TITLE_LENGTH ];
	INT8					m_i8RoomMode;
	UINT16					m_ui16TimeToRoundSec;
	UINT8					m_ui8MemberCountOfTeam;
};

#endif
