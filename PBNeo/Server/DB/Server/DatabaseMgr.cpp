#include "stdafx.h"
#include "DatabaseMgr.h"

CDatabaseMgr::CDatabaseMgr()
{
}

CDatabaseMgr::~CDatabaseMgr()
{
	Destroy();
}

BOOL CDatabaseMgr::Create( INT32 i32DBCount, S2DB_CONNECT_INFO* pConnectInfo, S2RING_INFO* pPush, S2RING_INFO* pPop )
{
	m_i32DBCount = i32DBCount;

	m_pDB = new CDatabase[ m_i32DBCount ];
	if( NULL == m_pDB )													return FALSE;
	for( INT32 i = 0 ; i < m_i32DBCount ; i++ )
	{
		if( FALSE == m_pDB[i].Create( pConnectInfo, pPush, pPop ) )		return FALSE;
	}

	return TRUE;
}

void CDatabaseMgr::Destroy()
{
	SAFE_DELETE_ARRAY( m_pDB );
}

T_RESULT CDatabaseMgr::Push( RING_DB_PUSH* pPush )
{
	return m_pDB[0].PushData( pPush );
}