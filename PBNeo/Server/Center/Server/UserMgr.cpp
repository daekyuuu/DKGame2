#include "stdafx.h"
#include "UserMgr.h"

static INT32 _CompareProc( void* p1, void* p2 )
{
	T_UID	TUID1	= ((USER_LIST*)p1)->m_TUID;
	T_UID	TUID2	= ((USER_LIST*)p2)->m_TUID;

	if( TUID1 > TUID2 )				return 1;
	else if( TUID1 < TUID2 )		return -1;

	return 0;
}

CUserMgr::CUserMgr()
{
	m_pUserContainer = NULL;
}

CUserMgr::~CUserMgr()
{
	Destroy();
}

BOOL CUserMgr::Create( UINT32 ui32UserCount )
{
	m_pUserContainer = new CUserContainer;
	if( (NULL == m_pUserContainer ) ||
		( FALSE == m_pUserContainer->Create( ui32UserCount ) ) )							return FALSE;

	for( INT32 i = 0 ; i < USER_NODE_COUNT ; i++ )
	{
		m_UserNode[ i ].SetAllocationUnit( ui32UserCount );
		m_UserNode[ i ].SetCompareProc( _CompareProc );
	}

	return TRUE;
}

void CUserMgr::Destroy()
{
	SAFE_DELETE( m_pUserContainer );
}

USER_LIST* CUserMgr::InsertUser( USER_LIST* pUser, T_RESULT& TResult )
{
	UINT64 ui64DivisionIdx = pUser->m_TUID % USER_NODE_COUNT;

	INT32 i32UserIdx = m_UserNode[ ui64DivisionIdx ].FindItem( pUser );
	if( -1 != i32UserIdx )
	{
		TResult = NET_RESULT_ERROR_LOGIN_DUPLICATION;
		return NULL;
	}

	USER_LIST* pInsertUser = m_pUserContainer->PopEmptyUser();
	if( NULL == pInsertUser )
	{
		TResult = NET_RESULT_ERROR_SYSTEM;
		return NULL;
	}
	pInsertUser->Reset();
	pInsertUser->InsertCopy( pUser );
	
	if( -1 == m_UserNode[ ui64DivisionIdx ].Add( pInsertUser ) )
	{
		m_pUserContainer->PushEmptyUser( pInsertUser );
		return NULL;
	}

	return pInsertUser;
}

BOOL CUserMgr::DeleteUser( USER_LIST* pUser )
{
	UINT64 ui64DivisionIdx = pUser->m_TUID % USER_NODE_COUNT;
	
	INT32 i32Idx = m_UserNode[ ui64DivisionIdx ].FindItem( pUser );
	if( -1 == i32Idx )
	{
		return FALSE;
	}
	
	USER_LIST* pDeleteUser = (USER_LIST*)m_UserNode[ ui64DivisionIdx ].GetItem( i32Idx );

	if( -1 == m_UserNode[ ui64DivisionIdx ].Delete( i32Idx ) )
	{
		return FALSE;
	}

	m_pUserContainer->PushEmptyUser( pDeleteUser );

	return TRUE;
}

USER_LIST* CUserMgr::FindUser( T_UID& TUID )
{
	UINT64 ui64DivisionIdx = TUID % USER_NODE_COUNT;

	USER_LIST UserList;
	UserList.m_TUID = TUID;

	INT32 i32Idx = m_UserNode[ ui64DivisionIdx ].FindItem( &UserList );
	if( -1 == i32Idx )
	{
		return FALSE;
	}

	return (USER_LIST*)m_UserNode[ ui64DivisionIdx ].GetItem( i32Idx );
}