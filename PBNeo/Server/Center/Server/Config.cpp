#include "stdafx.h"
#include "Config.h"

CConfig*	g_pConfig	= NULL;

CConfig::CConfig()
{
	g_pConfig				= this;

	m_i32IOCPThreadCount	= 32;
}

CConfig::~CConfig()
{
	Destroy();
}

BOOL CConfig::Create( TCHAR* strConfigPath )
{	
	return S2Config::Create( strConfigPath );
}

void CConfig::Destroy()
{
}

BOOL CConfig::LoadConfig()
{
	_TempLoad();

	if( (*m_pJson)[ _T("IP") ].IsNull() )
	{
		return FALSE;
	}
	m_ui32IP = S2Net::GetIPToLong( (wchar_t*)(*m_pJson)[ _T("IP") ].GetString() );

	if( (*m_pJson)[ _T("Port") ].IsNull() )
	{
		return FALSE;
	}
	m_ui16Port = (*m_pJson)[ _T("Port") ].GetInt();

	if( (*m_pJson)[ _T("UserCount") ].IsNull() )
	{
		return FALSE;
	}
	m_ui32UserCount = (*m_pJson)[ _T("UserCount") ].GetInt();

	if( (*m_pJson)[ _T("RoomCount") ].IsNull() )
	{
		return FALSE;
	}
	m_ui32RoomCount = (*m_pJson)[ _T("RoomCount") ].GetInt();

	return TRUE;
}

// 웹툴이 완성 되기 전까지 임시로 로드하는 데이터 입니다.
BOOL CConfig::_TempLoad()
{
	m_pMapInfo = new LOAD_MAP_INFO[ NET_ROOM_MODE_MAX ];

	m_pMapInfo[ NET_ROOM_MODE_DEATHMATCH ].m_i32MapCount	= 3;
	m_pMapInfo[ NET_ROOM_MODE_DEATHMATCH ].m_pMapID			= new T_MapID[ 3 ];
	m_pMapInfo[ NET_ROOM_MODE_DEATHMATCH ].m_pMapID[ 0 ]	= 110020;
	m_pMapInfo[ NET_ROOM_MODE_DEATHMATCH ].m_pMapID[ 1 ]	= 110040;
	m_pMapInfo[ NET_ROOM_MODE_DEATHMATCH ].m_pMapID[ 2 ]	= 110050;

	m_pMapInfo[ NET_ROOM_MODE_BOMBMISSION ].m_i32MapCount	= 3;
	m_pMapInfo[ NET_ROOM_MODE_BOMBMISSION ].m_pMapID		= new T_MapID[ 3 ];
	m_pMapInfo[ NET_ROOM_MODE_BOMBMISSION ].m_pMapID[ 0 ]	= 110010;
	m_pMapInfo[ NET_ROOM_MODE_BOMBMISSION ].m_pMapID[ 1 ]	= 110030;
	m_pMapInfo[ NET_ROOM_MODE_BOMBMISSION ].m_pMapID[ 2 ]	= 110060;
	
	return TRUE;
}