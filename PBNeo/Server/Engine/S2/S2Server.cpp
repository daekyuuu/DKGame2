#include "stdafx.h"
#include "S2Server.h"
#include "S2MiniDump.h"

static UINT32 _DefThreadProc( void * pPointer )
{
	((S2Server*)pPointer)->OnRun(); 
	return 0; 
}

S2LogFile* g_pLog = NULL;

S2Server::S2Server()
{
	m_eServerState		= S2_SERVER_STATE_NONE;
}

S2Server::~S2Server()
{
	Destroy();
}

S2_RESULT_CODE S2Server::StartServer( TCHAR* strDumpFile, INT32 i32InitDelayTimeSec )
{
	S2MiniDump::SetExceptionHandler( strDumpFile );

	if( FALSE == S2Time::Init() )																	return S2_RESULT_ERROR_ST_TIMER;

	g_pLog = new S2LogFile;
	if( NULL == g_pLog )																			return S2_RESULT_ERROR_ST_LOG;
	if( FALSE == g_pLog->Create( 
		m_pConfig->m_strLogFilePath, FILE_SAVE_INTERVAL_HOUR, m_pConfig->m_i32LogBufferCount ) )	return S2_RESULT_ERROR_ST_LOG;

	//Network Init
	if( FALSE == S2Net::Create() )																	return S2_RESULT_ERROR_ST_NETWORK;
	
	m_eServerState		= S2_SERVER_STATE_LOADING;
	m_dt32DelayTime		= S2Time::StandTime();
	m_dt32DelayTime.AddTime( S2_DATE_ADD_TYPE_SECOND, i32InitDelayTimeSec );

	if( FALSE == m_Thread.Create( (LPTHREAD_START_ROUTINE)_DefThreadProc, this ) )					return S2_RESULT_ERROR_ST_THREAD;	

	return S2_RESULT_SUCCESS;
}

void S2Server::SetInit()
{
	m_eServerState		= S2_SERVER_STATE_INIT;
}

S2_RESULT_CODE S2Server::OnInit()
{
	m_eServerState		= S2_SERVER_STATE_START;

	return S2_RESULT_SUCCESS;
}

void S2Server::OnRun()
{
	m_Thread.Init();

	while( m_Thread.IsRunning() )
	{
		switch( m_eServerState )
		{
		case S2_SERVER_STATE_LOADING:
			{
				// 10초 이상이면 문제
				if( S2Time::StandTime() > m_dt32DelayTime )
				{
					StopServer();
				}
			}
			break;
		case S2_SERVER_STATE_INIT:
			{
				if( S2_RESULT_SUCCESS != OnInit() )
				{
					StopServer();
				}
			}
			break;
		case S2_SERVER_STATE_START:
			{
				OnUpdate();
			}
			break;
		case S2_SERVER_STATE_END:
			{
				m_Thread.Terminate();
			}
			break;
		}
		m_Thread.Waiting( 1 );
	}
	//Destroy();
}

void S2Server::OnUpdate()
{
}

void S2Server::StopServer()
{
	m_eServerState = S2_SERVER_STATE_END;
}

void S2Server::Destroy()
{
	SAFE_DELETE( g_pLog );
	S2Time::Destroy();

	//exit(0);
}