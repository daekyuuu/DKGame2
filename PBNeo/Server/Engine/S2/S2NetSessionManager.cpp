#include "stdafx.h"
#include "S2NetSessionManager.h"
#include "S2NetSession.h"

S2NetSessionManager::S2NetSessionManager()
{
	m_i32NewSessionIdx		= 0;
	m_i32ActiveSessionCount	= 0;
	m_pSessionList			= NULL;
}

S2NetSessionManager::~S2NetSessionManager()
{
	Destroy();
}

BOOL S2NetSessionManager::Create( INT32 i32SessionCount )
{
	m_i32MaxSessionIdx	= i32SessionCount;

	if( FALSE == m_ContainerList.Create( m_i32MaxSessionIdx, S2_CONTAINER_TYPE_FREE ) )	return FALSE;

	m_pSessionList		= new S2NetSession*[ m_i32MaxSessionIdx ];
	if( NULL == m_pSessionList )														return FALSE;
	S2Memory::FillZero( m_pSessionList, sizeof(S2NetSession*)*m_i32MaxSessionIdx );

	return TRUE;
}

BOOL S2NetSessionManager::AddSession( INT32 i32Idx, S2NetSession* pSession )
{
	if( i32Idx >= m_i32MaxSessionIdx )	return FALSE;

	m_pSessionList[ i32Idx ] = pSession;
	m_pSessionList[ i32Idx ]->SetSessionIdx( i32Idx );

	return TRUE;
}

void S2NetSessionManager::Destroy(void)
{
	if( m_pSessionList )
	{
		for( INT32 i = 0 ; i < m_i32MaxSessionIdx ; i++ )
		{
			SAFE_DELETE( m_pSessionList[i] );
		}
		SAFE_DELETE_ARRAY( m_pSessionList );
	}
}

void S2NetSessionManager::Update(void)
{
	for( INT32 i = 0 ; i < m_i32MaxSessionIdx ; i++ )
	{
		m_pSessionList[i]->OnUpdate();
	}
	return; 
}

ULONG_PTR S2NetSessionManager::ConnectSession(SOCKET Socket, struct sockaddr_in * pAddr)
{
	S2NetSession * pNewSession = NULL; 
	m_CS.Enter(); 
	{
		UINT32 ui32Idx;
		if( m_ContainerList.PopContainerList( &ui32Idx ) )
		{
			pNewSession = m_pSessionList[ ui32Idx ]; 
			if( pNewSession->OnConnect(Socket, pAddr) )
			{
				m_i32ActiveSessionCount++; 
			}
		}
	}
	m_CS.Leave();
	return (ULONG_PTR)pNewSession; 
}

void S2NetSessionManager::DisConnectSession(INT32 i32ThreadIdx, S2NetSession * pSession, bool bForceMain)
{
	m_CS.Enter(); 
	{
		if(pSession->GetIsActive()) 
		{
			if( pSession->OnDisconnect(bForceMain) )
			{
				m_ContainerList.PushContainerList( pSession->GetSessionIdx() );

				m_i32ActiveSessionCount--;
			}
		}
	} 
	m_CS.Leave(); 
	return; 
}

void S2NetSessionManager::DisConnectAllSession(INT32 i32ThreadIdx)
{
	for(INT32 i = 0; i < m_i32MaxSessionIdx; i++)
	{
		DisConnectSession( i32ThreadIdx, m_pSessionList[i], true ); 
	}
	return; 
}
