#include "stdafx.h"
#include "S2NetIocpWorker.h"
#include "S2NetSessionManager.h"
#include "S2NetCompletionHandler.h"
#include "S2NetSession.h"

static UINT32 _DefThreadProc( void * pPointer )
{
	((S2NetIocpWorker*)pPointer)->Update();
	return 0;
}

S2NetIocpWorker::S2NetIocpWorker()
{
	m_bThreadRun			= FALSE;
	m_hThread				= NULL;

	m_i32Idx				= -1; 
	m_pSessionManager		= NULL;
}

S2NetIocpWorker::~S2NetIocpWorker()
{
	OnDestory();
}

BOOL S2NetIocpWorker::Create()
{
	// Thread Create
	UINT32	ui32ThreadID = 0;
	m_hThread  = CreateThread( NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc, this, 0, (DWORD *)&ui32ThreadID );
	if( NULL == m_hThread )													return FALSE;
	if( FALSE == SetThreadPriority( m_hThread, THREAD_PRIORITY_NORMAL) )	return FALSE;

	return TRUE;
}

void S2NetIocpWorker::OnDestory()
{
	m_bThreadRun = FALSE;
	::WaitForSingleObject( m_hThread, INFINITE );
}

void S2NetIocpWorker::Update(void)	
{	
	ULONG_PTR		ptrIoKey;
	DWORD			dwTransferSize;
	LPOVERLAPPED	pOverLapped;
	BOOL			bRv; 
	S2NetSession*	pSession; 
	bool			bWorking;
	OVERLAPPED_S2*	pS2Overlapped;

	m_bThreadRun = TRUE;
	while( m_bThreadRun )
	{
		bWorking = false;

		//Check IOPort 
		ptrIoKey		= 0;
		pOverLapped		= 0;
		bRv = ::GetQueuedCompletionStatus( m_hIOCP, &dwTransferSize, &ptrIoKey, &pOverLapped, 1 ); 
		
		if( bRv && ( 0 != ptrIoKey ) && ( 0 != dwTransferSize ) && ( 0 != pOverLapped ) )
		{
			bWorking = true;

			pS2Overlapped = ( OVERLAPPED_S2* )pOverLapped;
			if( SESSION_ASYNCFLAG_RECEIVE == pS2Overlapped->m_ui32Flags )
			{
				//처리 
				pSession = ( S2NetSession* )ptrIoKey;
				if( pSession->GetIsActive() )
				{
					if( m_pCompletionHandler->AddWorkingSession( pSession, m_i32Idx ) )
					{
						pSession->SetWorkThreadIdx( m_i32Idx );
						pSession->Dispatch( dwTransferSize );
						pSession->SetWorkThreadIdx( -1 );

						m_pCompletionHandler->AddPacketInfo( m_i32Idx, dwTransferSize );

						m_pCompletionHandler->RemoveWorkingSession( m_i32Idx );
						
						// Read Event 를 걸어줍니다.
						pSession->WaitPacketReceive();
					}
				}
			}
			else
			{
				// Send 일때는 아무런 일도 하지 않습니다.
			}
		}
		else
		{
			if( ( 0 != ptrIoKey ) && ( ERROR_OPERATION_ABORTED != GetLastError() ) )
			{//이녀석은 삭제처리 합니다. 
				bWorking = true;

				pSession = ( S2NetSession* )ptrIoKey;
				if( m_pCompletionHandler->AddWorkingSession(pSession, m_i32Idx ) )
				{	
					pSession->SetWorkThreadIdx( m_i32Idx );
					m_pSessionManager->DisConnectSession( m_i32Idx, pSession, false );
					pSession->SetWorkThreadIdx( -1 );
					m_pCompletionHandler->RemoveWorkingSession( m_i32Idx ); 
				}
				else
				{
					S2TRACE(  _T("[S2NetIocpWorker::OnUpdate] AddWorkingSession DisConnect Error \n") );
				}
			}
		}
		::WaitForSingleObject( m_hThread, 1 );
	}

	return; 
}
