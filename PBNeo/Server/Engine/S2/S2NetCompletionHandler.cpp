#include "stdafx.h"
#include "S2NetCompletionHandler.h"
#include "S2NetIocpWorker.h"
#include "S2NetSessionManager.h"
#include "S2Memory.h"

S2NetCompletionHandler::S2NetCompletionHandler()
{
	m_hIOCP				= 0;
	m_ppWorker			= NULL;
	m_i32WorkerCount	= 0;
	m_pSessionManager	= NULL;
	m_ppWorkSession		= NULL; 
}

S2NetCompletionHandler::~S2NetCompletionHandler()
{
	Destroy(); 
}

BOOL S2NetCompletionHandler::OnCreate( INT32 i32WorkCount, S2NetSessionManager * pSessionManager )
{
	m_pSessionManager	= pSessionManager; 

	//Create IOCP
	m_hIOCP				= ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, 0, 0, i32WorkCount ); 
	if( NULL == m_hIOCP )
	{
		S2TRACE( _T("[S2NetCompletionHandler::OnCreate] CreateIoCompletionPort \n") );
		return FALSE; 
	}

	//Create WorkIOCP
	m_i32WorkerCount	= i32WorkCount; 
	m_ppWorker			= new S2NetIocpWorker*[ m_i32WorkerCount ];
	if( NULL == m_ppWorker )														return FALSE;

	for( INT32 i = 0; i < m_i32WorkerCount; i++ )
	{
		m_ppWorker[i] = new S2NetIocpWorker;
		if( NULL == m_ppWorker[i] )													return FALSE;

		m_ppWorker[i]->SetIocpHandle( m_hIOCP, m_pSessionManager, i, this ); 

		if( 0 == m_ppWorker[i]->Create() )											return FALSE;
	}

	m_ppWorkSession		= new S2NetSession*[ m_i32WorkerCount ]; 
	if( NULL == m_ppWorkSession )													return FALSE;
	S2Memory::FillZero( m_ppWorkSession, sizeof(S2NetSession*) * m_i32WorkerCount ); 

	m_pui64PacketCount	= new UINT64[ m_i32WorkerCount ];
	if( NULL == m_pui64PacketCount )												return FALSE;
	m_pui64PacketSize	= new UINT64[ m_i32WorkerCount ];
	if( NULL == m_pui64PacketSize )													return FALSE;

	return TRUE; 
}

BOOL S2NetCompletionHandler::Destroy(void)
{
	SAFE_DELETE_ARRAY( m_pui64PacketSize );
	SAFE_DELETE_ARRAY( m_pui64PacketCount );

	if( m_ppWorker )
	{
		for( INT32 i = 0; i < m_i32WorkerCount; i++ )
		{
			SAFE_DELETE( m_ppWorker[i] );
		}

		SAFE_DELETE_ARRAY( m_ppWorker ); 	
	}
	SAFE_DELETE_ARRAY( m_ppWorkSession );

	return TRUE; 
}

void S2NetCompletionHandler::OnAddIOCP(HANDLE handle, ULONG_PTR keyValue)
{	
	::CreateIoCompletionPort( handle, m_hIOCP, keyValue, 0 );
	
	return; 
}

BOOL S2NetCompletionHandler::AddWorkingSession( S2NetSession* pSession, INT32 i32Idx )
{
	BOOL Rv = TRUE; 
	m_CS.Enter();
	{
		for(INT32 i= 0; i < m_i32WorkerCount; i++)
		{
			if( m_ppWorkSession[i] == pSession)
			{
				Rv = FALSE; 
				break; 
			}
		}
		if(Rv) m_ppWorkSession[ i32Idx ] = pSession; 
	}
	m_CS.Leave(); 

	return Rv; 
}

void S2NetCompletionHandler::GetPacketInfo( UINT64* pi64Count, UINT64* pi64Size )
{
	*pi64Count	= 0;
	*pi64Size	= 0;		
	for( INT32 i = 0 ; i < m_i32WorkerCount ; i++ )
	{
		*pi64Count	+= m_pui64PacketCount[i];
		*pi64Size	+= m_pui64PacketSize[i];
	}
}

void S2NetCompletionHandler::GetPacketInfoThread( INT32 i32ThreadIdx, UINT64* pi64Count, UINT64* pi64Size )
{
	if( 0 > i32ThreadIdx )				return;
	if( m_i32WorkerCount <= i32ThreadIdx )	return;

	*pi64Count	+= m_pui64PacketCount[ i32ThreadIdx ];
	*pi64Size	+= m_pui64PacketSize[ i32ThreadIdx ];
}	