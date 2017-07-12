#include "stdafx.h"
#include "Config.h"

CConfig*	g_pConfig	= NULL;

CConfig::CConfig()
{
	g_pConfig					= this;
	m_i32IOCPThreadCount		= 60;
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

	if( (*m_pJson)[ _T("ServerKey") ].IsNull() )
	{
		return FALSE;
	}
	m_ServerKey = (*m_pJson)[ _T("ServerKey") ].GetInt();

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

	if( (*m_pJson)[ _T("CenterIP") ].IsNull() )
	{
		return FALSE;
	}
	m_ui32CenterIP = S2Net::GetIPToLong( (wchar_t*)(*m_pJson)[ _T("CenterIP") ].GetString() );

	if( (*m_pJson)[ _T("CenterPort") ].IsNull() )
	{
		return FALSE;
	}
	m_ui16CenterPort = (*m_pJson)[ _T("CenterPort") ].GetInt();

	m_i32DBCount = (*m_pJson)[ _T("DBCount") ].GetInt();
	S2String::Copy( m_DBConnectInfo.m_strDataSource,	(wchar_t*)(*m_pJson)[ _T("DBConnectSource") ].GetString(),	S2_STRING_COUNT );
	S2String::Copy( m_DBConnectInfo.m_strCatalog,		(wchar_t*)(*m_pJson)[ _T("DBConnectCatalog") ].GetString(), S2_STRING_COUNT );
	S2String::Copy( m_DBConnectInfo.m_strID,			(wchar_t*)(*m_pJson)[ _T("DBConnectID") ].GetString(),		S2_STRING_COUNT );
	S2String::Copy( m_DBConnectInfo.m_strPW,			(wchar_t*)(*m_pJson)[ _T("DBConnectPW") ].GetString(),		S2_STRING_COUNT );	

	return TRUE;
}

// 웹툴이 완성 되기 전까지 임시로 로드하는 데이터 입니다.
BOOL CConfig::_TempLoad()
{
	m_ui32Season = 1;
	
	return TRUE;
}