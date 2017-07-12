#include "stdafx.h"
#include "S2NetServerContext.h"
#include "S2NetAcceptor.h" 
#include "S2NetCompletionHandler.h"
#include "S2NetSessionManager.h"
#include "S2NetSession.h"
#include "S2Memory.h"

static UINT32 _DefThreadProc( void * pPointer )
{
	((S2NetServerContext*)pPointer)->OnUpdate(); 
	return 0; 
}

S2NetServerContext::S2NetServerContext()
{	
	m_pAcceptor				= NULL; 
	m_pCompletionHandler	= NULL; 
	m_pSessionManager		= NULL; 
}

S2NetServerContext::~S2NetServerContext()
{
	Destroy(); 
}
BOOL S2NetServerContext::Create( INT32 i32SocketCount, UINT32* pui32IP, UINT16* pui16Port, INT32 i32WorkCount )
{
	return Create( i32SocketCount, pui32IP, pui16Port, i32WorkCount, m_pSessionManager);
}

BOOL S2NetServerContext::Create( INT32 i32SocketCount, UINT32* pui32IP, UINT16* pui16Port, INT32 i32WorkCount, S2NetSessionManager* pSessionManager)
{
	// Session Manager
	m_pSessionManager		= pSessionManager;

	// Create Iocp
	m_pCompletionHandler	= new S2NetCompletionHandler;
	if( NULL == m_pCompletionHandler )												return FALSE;
	if( FALSE == m_pCompletionHandler->OnCreate( i32WorkCount, m_pSessionManager) )	return FALSE; 

	// Create Acceptor
	m_pAcceptor = new S2NetAcceptor; 
	if( NULL == m_pAcceptor )														return FALSE;
	if( FALSE == m_pAcceptor->Create( i32SocketCount, pui32IP, pui16Port ))			return FALSE; 

	if( FALSE == m_Thread.Create( (LPTHREAD_START_ROUTINE)_DefThreadProc, this ) )	return FALSE;

	return TRUE; 
}

void S2NetServerContext::Destroy(void)
{
	m_Thread.Destroy();

	SAFE_DELETE( m_pAcceptor ); 				//Destroy Acceptor
	SAFE_DELETE( m_pCompletionHandler );		//Destroy Complettion 

	m_pSessionManager = NULL;					//Reset Sessin Manager
	return; 
}

void S2NetServerContext::OnUpdate(void)
{
	m_Thread.Init();

	while( m_Thread.IsRunning() )
	{
		UpdateAcceptor();
		
		m_Thread.Waiting( 1 );
	}
}

INT32 S2NetServerContext::GetSessionCount(void)
{
	return m_pSessionManager->GetSessionCount(); 
}

void S2NetServerContext::GetPacketInfo( UINT64* pi64Count, UINT64* pi64Size )
{
	m_pCompletionHandler->GetPacketInfo( pi64Count, pi64Size );	
}

void S2NetServerContext::GetPacketInfoThread( INT32 i32ThreadIdx, UINT64* pi64Count, UINT64* pi64Size )
{
	m_pCompletionHandler->GetPacketInfoThread( i32ThreadIdx, pi64Count, pi64Size );
}

INT32 S2NetServerContext::UpdateAcceptor(void)
{
	struct sockaddr_in FromAddr; 
	SOCKET hSocketAccept = m_pAcceptor->Update(&FromAddr);
	ULONG_PTR	SessionPointer;

	if( INVALID_SOCKET == hSocketAccept )	return 0;

	SessionPointer = m_pSessionManager->ConnectSession( hSocketAccept, &FromAddr );

	if( 0 != SessionPointer )
	{
		m_pCompletionHandler->OnAddIOCP((HANDLE)hSocketAccept, SessionPointer);
		((S2NetSession*)SessionPointer)->OnConnectInit();

		return 1; 
	}
	else
	{
		// 허용 세션 초과 시, 연결을 끊는다.
		::shutdown( hSocketAccept, SD_BOTH );
		::closesocket( hSocketAccept );

		return 0; 
	}
}