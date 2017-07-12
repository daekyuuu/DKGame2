#include "stdafx.h"
#include "SessionMgr.h"
#include "Session.h"

CSessionMgr::CSessionMgr()
{
	m_pPopRing				= NULL;
}

CSessionMgr::~CSessionMgr()
{
	Destroy();
}

BOOL CSessionMgr::Create( INT32 i32SessionCount, INT32 i32ThreadCount, INT32 i32SessionPacketSize, INT32 i3SessionBufferCount )
{
	if( FALSE == S2NetSessionManager::Create( i32SessionCount ) )
	{
		return FALSE;
	}
	
	m_i32ThreadCount	= i32ThreadCount;

	m_pPopRing = new S2RingBuffer[ m_i32ThreadCount ];
	if( NULL == m_pPopRing )															return FALSE;

	for( INT32 i = 0 ; i < m_i32ThreadCount ; i++ )
	{
		if( FALSE == m_pPopRing[i].Create( i32SessionPacketSize, i3SessionBufferCount ) )
		{
			return FALSE;
		}
	}

	CSession* pSession;
	for( INT32 i = 0 ; i < m_i32MaxSessionIdx ; i++ )
	{
		pSession = new CSession;
		if( NULL == pSession )						return FALSE;
		if( FALSE == AddSession( i, pSession ) )	return FALSE;
	}

	return TRUE;
}

void CSessionMgr::Destroy()
{
	SAFE_DELETE_ARRAY( m_pPopRing );
}

void CSessionMgr::SetPacketBuffer( S2RingBuffer* pCenter )
{
	for( INT32 i = 0 ; i < m_i32MaxSessionIdx ; i++ )
	{
		((CSession*)m_pSessionList[ i ])->SetPacketBuffer( m_pPopRing, pCenter );
	}
}