#include "stdafx.h"
#include "S2Config.h"

S2Config::S2Config()
{
	m_pJsonFile = NULL;
	m_pJson		= NULL;
}

S2Config::~S2Config()
{
	OnDestroy();
}

BOOL S2Config::Create( TCHAR* strConfigPath )
{
	m_pJsonFile = new S2JsonFile;
	if( NULL == m_pJsonFile )
	{
		return FALSE;
	}
	m_pJson = m_pJsonFile->Create( strConfigPath );
	if( NULL == m_pJson )
	{
		return FALSE;
	}

	if( (*m_pJson)[ _T("LogPath") ].IsNull() )
	{
		return FALSE;
	}
	S2String::Copy( m_strLogFilePath, (wchar_t*)(*m_pJson)[ _T("LogPath") ].GetString(), S2_STRING_COUNT );

	if( (*m_pJson)[ _T("LogBufferCount") ].IsNull() )
	{
		return FALSE;
	}
	m_i32LogBufferCount = (*m_pJson)[ _T("LogBufferCount") ].GetInt();

	return TRUE;
}

void S2Config::OnDestroy()
{
	SAFE_DELETE( m_pJsonFile );
}