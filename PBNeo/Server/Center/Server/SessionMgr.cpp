#include "stdafx.h"
#include "SessionMgr.h"
#include "Session.h"

CSessionMgr::CSessionMgr()
{
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

	S2Memory::Fill( m_pServerList, 0, (sizeof(CSession*)*SERVER_TYPE_COUNT*SERVER_COUNT_MAX) );

	CSession* pSession;
	for( INT32 i = 0 ; i < m_i32MaxSessionIdx ; i++ )
	{
		pSession = new CSession;
		if( NULL == pSession )								return FALSE;
		if( FALSE == pSession->Create() )					return FALSE;
		if( FALSE == AddSession( i, pSession ) )			return FALSE;
	}

	return TRUE;
}

void CSessionMgr::Destroy()
{
	SAFE_DELETE_ARRAY( m_pPopRing );
}

void CSessionMgr::SetPacketBuffer()
{
	for( INT32 i = 0 ; i < m_i32MaxSessionIdx ; i++ )
	{
		((CSession*)m_pSessionList[ i ])->SetPacketBuffer( m_pPopRing );
	}
}

BOOL CSessionMgr::SetServerType( SERVER_TYPE eType, UINT32 ui32Idx, CSession* pSession )
{
	if( SERVER_TYPE_COUNT <= eType )			return FALSE;
	if( SERVER_COUNT_MAX <= ui32Idx )			return FALSE;

	m_pServerList[ eType ][ ui32Idx ] = pSession;
	pSession->SetServerInfo( eType, ui32Idx );

	SERVER_TYPE_INFO ServerTypeInfo;
	ServerTypeInfo.m_eType		= eType;
	ServerTypeInfo.m_ui32Idx	= ui32Idx;

	DLG_SENDMESSAGE( DLG_MESSAGE_SERVER_INSERT, (LPARAM)&ServerTypeInfo );

	return TRUE;
}

CSession* CSessionMgr::GetServerType( SERVER_TYPE eType, UINT32 ui32Idx )
{
	if( SERVER_TYPE_COUNT <= eType )			return NULL;
	if( SERVER_COUNT_MAX <= ui32Idx )			return NULL;

	return m_pServerList[ eType ][ ui32Idx ];
}

RING_IOCP_POP* CSessionMgr::GetMainPop( INT32 i32Idx )
{
	if( i32Idx < 0 )							return NULL;
	if( i32Idx >= m_i32ThreadCount )			return NULL;

	S2RingBuffer* pRing;
	pRing = &m_pPopRing[ i32Idx ];

	if( 0 == pRing->GetBufferCount() )			return NULL;

	return (RING_IOCP_POP*)pRing->Pop();;
}

void CSessionMgr::GetMainPopIdx( INT32 i32Idx )
{	
	if( i32Idx < 0 )							return;
	if( i32Idx >= m_i32ThreadCount )			return;

	m_pPopRing[ i32Idx ].PopIdx();
}

BOOL CSessionMgr::SendPacketServer( SERVER_TYPE eType, S2MOPacketBase* pPacket )
{
	if( SERVER_TYPE_COUNT <= eType )			return FALSE;

	CSession* pSession;
	for( INT32 i = 0 ; i < SERVER_COUNT_MAX ; i++ )
	{
		pSession = m_pServerList[ eType ][ i ];
		if( NULL == pSession )					continue;
		pSession->SendPacket( pPacket );
	}
	return TRUE;
}