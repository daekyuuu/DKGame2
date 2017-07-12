#include "stdafx.h"
#include "S2LogFileBase.h"
#include "S2String.h"

static UINT32 _DefThreadProc( void * pPointer )
{
	((S2LogFileBase*)pPointer)->OnUpdate(); 
	return 0;
}

S2LogFileBase::S2LogFileBase()
{
	m_hFile				= NULL;
	m_ui32WorkCounter	= 0; 

	m_ui16Ver01			= 0;
	m_ui16Ver02			= 0;
	m_ui32Ver03			= 0;
	m_ui32Ver04			= 0;
}

S2LogFileBase::~S2LogFileBase(void)
{
	Destroy(); 
}

BOOL S2LogFileBase::_Create( INT32 iSaveType, INT32 iLogBufferCount )
{
	m_iSaveType			= iSaveType;
	m_iBufferCount		= iLogBufferCount;

	if( FALSE == m_Thread.Create( (LPTHREAD_START_ROUTINE)_DefThreadProc ,this ) )
	{
		return FALSE;
	}

	return TRUE; 
}

void S2LogFileBase::Destroy(void)
{
	m_Thread.Destroy();

	for(UINT32 i = 0; i < m_ui32WorkCounter; i++)
	{
		SAFE_DELETE( m_ppLogBuffer[i] ); 
	}

	::CloseHandle( m_hFile );
}

void S2LogFileBase::SetVersion( UINT16 ui16Ver01, UINT16 ui16Ver02, UINT32 ui32Ver03, UINT32 ui32Ver04 )
{
	m_ui16Ver01	= ui16Ver01;
	m_ui16Ver02	= ui16Ver02;
	m_ui32Ver03	= ui32Ver03;
	m_ui32Ver04	= ui32Ver04;
}

void S2LogFileBase::OnUpdate(void)
{
	BOOL bWorking; 

	m_Thread.Init();

	while( m_Thread.IsRunning() )
	{
		bWorking = FALSE; 

		//Create File 
		_UpdateTime();
		switch( m_iSaveType )
		{
		case FILE_SAVE_INTERVAL_DAY		: 
			if(m_ui32FileDay != m_iDay)		_CreateFile(); 
			break; 
		case FILE_SAVE_INTERVAL_HOUR	:
		default							: 
			if(m_ui32FileHour != m_iHour)	_CreateFile(); 
			break; 
		}
		
		bWorking = _Working();

		if( FALSE == bWorking )
		{
			m_Thread.Waiting( 1 );
		}
	}

	return; 
}

INT32 S2LogFileBase::_FindWorkIdx()
{
	DWORD	dwThreadId = GetCurrentThreadId();

	for( UINT32 i = 0 ; i < m_ui32WorkCounter ; ++i )
	{
		if( dwThreadId == m_adwThreadID[i] )
		{
			return i;
		}
	}

	return RV_ADD_FAIL;
}

INT32 S2LogFileBase::_FindWorkIdx( DWORD dwThreadID )
{	

	for( UINT32 i = 0 ; i < m_ui32WorkCounter ; ++i )
	{
		if( dwThreadID == m_adwThreadID[i] )
		{
			return i;
		}
	}

	return RV_ADD_FAIL;
}

BOOL S2LogFileBase::_CreateFile(void)
{
	return TRUE;
}

BOOL S2LogFileBase::_Working()
{
	return TRUE;
}