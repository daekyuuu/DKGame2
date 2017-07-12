#include "stdafx.h"
#include "LoginBase.h"

static UINT32 _DefThreadProc( void * pPointer )
{
	((CLoginBase*)pPointer)->OnRun(); 
	return 0;
}

CLoginBase::CLoginBase()
{
	m_pRingPush		= NULL;
	m_pRingPop		= NULL;
}

CLoginBase::~CLoginBase()
{
	Destroy();
}

BOOL CLoginBase::Create( S2DB_CONNECT_INFO* pConnectInfo, S2RING_INFO* pPush, S2RING_INFO* pPop )
{
	m_pRingPush = new S2RingBuffer;
	if( NULL == m_pRingPush )														return FALSE;
	if( FALSE == m_pRingPush->Create( pPush ) )										return FALSE;

	m_pRingPop = new S2RingBuffer;
	if( NULL == m_pRingPop )														return FALSE;
	if( FALSE == m_pRingPop->Create( pPop ) )										return FALSE;

	if( FALSE == m_Thread.Create( (LPTHREAD_START_ROUTINE)_DefThreadProc ,this ) )	return FALSE;
	return TRUE;
}

void CLoginBase::Destroy()
{
	m_Thread.Destroy();

	SAFE_DELETE( m_pRingPush );
	SAFE_DELETE( m_pRingPop );
}

void CLoginBase::OnRun()
{
	m_Thread.Init();

	while( m_Thread.IsRunning() )
	{
		if( FALSE == OnUpdate() )
		{
			m_Thread.Waiting( 1 );
		}
	}
}

BOOL CLoginBase::InsertUser( RING_LOGIN_PUSH* pPush )
{
	if( RING_BUFFER_SUCCESS != m_pRingPush->Push( pPush ) )	return FALSE;

	return TRUE;
}

BOOL CLoginBase::OnUpdate()
{
	return FALSE;
}