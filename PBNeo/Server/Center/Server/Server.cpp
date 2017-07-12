#include "stdafx.h"
#include "Server.h"

#include "Config.h"
#include "MainFrm.h"

CServer::CServer()
{
	m_pConfig	= NULL;
	m_pMainFrm	= NULL;
}

CServer::~CServer()
{
	Destroy();
}

S2_RESULT_CODE CServer::StartServer( TCHAR* strDumpFile )
{
	//_CrtSetBreakAlloc( 364 );

	m_pConfig = new CConfig;
	if( NULL == m_pConfig || 
		FALSE == m_pConfig->Create( _T(".\\Config.txt" ) ) )
	{
		return S2_RESULT_ERROR_ST_CONFIG;
	}

	S2_RESULT_CODE eErrorCode = S2Server::StartServer( strDumpFile );
	if( S2_RESULT_SUCCESS != eErrorCode )
	{
		return eErrorCode;
	}

	return S2_RESULT_SUCCESS;
}

S2_RESULT_CODE CServer::OnInit()
{
	if( FALSE == ((CConfig*)m_pConfig)->LoadConfig() )
	{
		return S2_RESULT_ERROR_ST_CONFIG;
	}

	m_pMainFrm = new CMainFrm;
	if( NULL == m_pMainFrm )
	{
		return S2_RESULT_ERROR_ST_MAINFRM;
	}
	if( FALSE == m_pMainFrm->Create( (CConfig*)m_pConfig ) )
	{
		return S2_RESULT_ERROR_ST_MAINFRM;
	}
	
	return S2Server::OnInit();
}

void CServer::OnUpdate()
{
	m_pMainFrm->OnUpdate();
}

void CServer::Destroy()
{
	SAFE_DELETE( m_pMainFrm );
	SAFE_DELETE( m_pConfig );

	S2Server::Destroy();
}
