#include "stdafx.h"
#include "S2Thread.h"

S2Thread::S2Thread()
{
	m_hThread	= NULL;
	m_bRunning	= FALSE;
}

S2Thread::~S2Thread()
{
}

BOOL S2Thread::Create( LPTHREAD_START_ROUTINE Proc, void* pPoint, INT32 i32ThreadPriority ) //     THREAD_PRIORITY_NORMAL)
{
	m_hThread = CreateThread( NULL, 4096, Proc, pPoint, 0, &m_dwThreadID );
	if( NULL == m_hThread )													return FALSE;
	if( FALSE == SetThreadPriority( m_hThread, i32ThreadPriority) )			return FALSE;

	return TRUE;
}

void S2Thread::Init()
{
	m_bRunning = TRUE;
}

void S2Thread::Terminate()
{
	m_bRunning = FALSE;
}

UINT32 S2Thread::Waiting( UINT32 ui32Millseconds )
{
	return WaitForSingleObject( m_hThread, ui32Millseconds );
}

void S2Thread::Destroy()
{
	Terminate();
	::WaitForSingleObject( m_hThread, INFINITE );
}