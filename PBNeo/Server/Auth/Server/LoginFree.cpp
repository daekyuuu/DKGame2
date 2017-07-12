#include "stdafx.h"
#include "LoginFree.h"

CLoginFree::CLoginFree()
{
	m_pDB			= NULL;
}

CLoginFree::~CLoginFree()
{
	Destroy();
}

BOOL CLoginFree::Create( S2DB_CONNECT_INFO* pConnectInfo, S2RING_INFO* pPush, S2RING_INFO* pPop )
{
	m_pDB = new S2DBAdo;
	if( NULL == m_pDB )														return FALSE;
	if( FALSE == m_pDB->Create() )											return FALSE;
	if( FALSE == m_pDB->Connect( pConnectInfo ) )							return FALSE;

	if( FALSE == CLoginBase::Create( pConnectInfo, pPush, pPop  ) )			return FALSE;

	return TRUE;
}

void CLoginFree::Destroy()
{
	m_Thread.Destroy();

	SAFE_DELETE( m_pDB );
}

BOOL CLoginFree::OnUpdate()
{
	if( 0 >= m_pRingPush->GetBufferCount() )	return FALSE;

	RING_LOGIN_PUSH* pPush	= (RING_LOGIN_PUSH*)m_pRingPush->Pop();
	RING_LOGIN_POP*	pPop	= (RING_LOGIN_POP*)m_pRingPop->PushPointer();
	if( ( NULL == pPush ) || ( NULL == pPop ) )	return FALSE;
		
	pPop->m_i32SessionIdx	= pPush->m_i32SessionIdx;
	pPop->m_TResult			= NET_RESULT_ERROR;

	TCHAR strSQL[ S2_STRING_COUNT ];
	S2DB::MakeQuery( strSQL, S2_STRING_COUNT, _T("usp_Account_Login"), 
		S2DB_DATA_TYPE_CHAR, pPush->m_strID, 
		S2DB_DATA_TYPE_CHAR, pPush->m_strPW,
		S2DB_DATA_TYPE_END );
	if( m_pDB->ExecuteSP( strSQL ) )
	{
		INT32 i32Result = m_pDB->GetINT32Data( 2 );
		if( 0 == i32Result )
		{
			pPop->m_TResult			= NET_RESULT_SUCCESS;
			pPop->m_TUID			= m_pDB->GetUINT64Data( 3 );
		}
	}

	m_pRingPush->PopIdx();
	m_pRingPop->PushPointerIdx();

	return TRUE;
}
