#include "stdafx.h"
#include "S2NetConnector.h"

static UINT32 _DefThreadProc( void * pPointer )
{
	((S2NetConnector*)pPointer)->OnUpdate(); 
	return 0;
}

S2NetConnector::S2NetConnector()
{
	m_pPushRing		= NULL;
	m_pPopRing		= NULL;
}

S2NetConnector::~S2NetConnector()
{
	Destroy();
}

BOOL S2NetConnector::Create( UINT32 ui32IP, UINT16 ui16Port, CallbackRoutine CallBack, void* pAgent )
{
	if( FALSE == S2SocketConnector::Create( ui32IP, ui16Port, CallBack, pAgent ) )
	{
		return FALSE;
	}

	if( FALSE == m_Thread.Create( (LPTHREAD_START_ROUTINE)_DefThreadProc ,this ) )
	{
		return FALSE;
	}

	m_dt32ReconnectTime = S2Time::StandTime();
	m_dt32ReconnectTime.AddTime( S2_DATE_ADD_TYPE_SECOND, 10 );

	return TRUE;
}

void S2NetConnector::Destroy()
{
	SAFE_DELETE_ARRAY( m_pPopRing );
	SAFE_DELETE_ARRAY( m_pPushRing );
}

void S2NetConnector::OnUpdate()
{
	m_Thread.Init();

	DATE32 dt32Heartbit;

	BOOL bWorking;
	while( m_Thread.IsRunning() )
	{
		bWorking = FALSE;
		
		if( INVALID_SOCKET == m_hSocket )
		{
			DisConnect();
			if( m_dt32ReconnectTime < S2Time::StandTime() )
			{
				_Connect();
				m_dt32ReconnectTime = S2Time::StandTime();
				m_dt32ReconnectTime.AddTime( S2_DATE_ADD_TYPE_SECOND, 10 );
				bWorking = TRUE;
			}
		}
		else
		{
			SelectEvent();
			bWorking = OnRunning();
		}

		if( FALSE == bWorking )
		{
			m_Thread.Waiting( 1 );
		}
	}
}

BOOL S2NetConnector::OnRunning()
{
	return TRUE;
}

BOOL S2NetConnector::CreateRingBuffer( INT32 i32PushCount, INT32 i32PushBufferSize, INT32 i32PushBufferCount, INT32 i32PopCount, INT32 i32PopBufferSize, INT32 i32PopBufferCount )
{
	m_pPushRing = new S2RingBuffer[ i32PushCount ];
	if( NULL == m_pPushRing )
	{
		return FALSE;
	}
	for( INT32 i = 0 ; i < i32PushCount ; i++ )
	{
		if( FALSE == m_pPushRing[i].Create( i32PushBufferSize, i32PushBufferCount ) )
		{
			return FALSE;
		}
	}
	m_pPopRing	= new S2RingBuffer[ i32PushCount ];
	if( NULL == m_pPopRing )
	{
		return FALSE;
	}
	for( INT32 i = 0 ; i < i32PopCount ; i++ )
	{
		if( FALSE == m_pPopRing[i].Create( i32PopBufferSize, i32PopBufferCount ) )
		{
			return FALSE;
		}
	}

	m_i32RingPushCount = i32PushCount;

	return TRUE;
}