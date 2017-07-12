#include "stdafx.h"
#include "LoginMgr.h"

CLoginMgr::CLoginMgr()
{
}

CLoginMgr::~CLoginMgr()
{
	Destroy();
}

BOOL CLoginMgr::Create( INT32 i32LoginCount, S2DB_CONNECT_INFO* pConnectInfo, S2RING_INFO* pPush, S2RING_INFO* pPop )
{
	m_i32LoginCount = i32LoginCount;

	m_pLogin = new CLoginFree[ m_i32LoginCount ];
	if( NULL == m_pLogin )												return FALSE;
	for( INT32 i = 0 ; i < m_i32LoginCount ; i++ )
	{
		if( FALSE == m_pLogin[i].Create( pConnectInfo, pPush, pPop ) )		return FALSE;
	}

	return TRUE;
}

void CLoginMgr::Destroy()
{
	SAFE_DELETE_ARRAY( m_pLogin );
}

BOOL CLoginMgr::InsertUser( RING_LOGIN_PUSH* pPush )
{
	for( INT32 i = 0 ; i < m_i32LoginCount ; i++ )
	{
		if( FALSE == m_pLogin[ i ].InsertUser( pPush ) )
		{
			return FALSE;
		}
	}
	return TRUE;
}