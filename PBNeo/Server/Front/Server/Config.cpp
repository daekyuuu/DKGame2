#include "stdafx.h"
#include "Config.h"

CConfig::CConfig()
{
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

	return TRUE;
}