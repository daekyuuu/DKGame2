#include "stdafx.h"
#include "S2NetSession.h"
#include "S2Packet.h"

S2NetSession::S2NetSession()
{
	m_IsActive					= false;
	m_hSocket					= INVALID_SOCKET;
	m_strIPString[0]			= '\0';;
	m_i32WorkThreadIdx			= -1; 
	m_i32ReceiveSize			= 0;

	m_ui32ConIP					= 0;
	m_ui16ConPort				= 0;

	m_ui32SendDelayTimeMilli	= 0;
}

S2NetSession::~S2NetSession()
{
	Destroy();
}

BOOL S2NetSession::Create( UINT32 ui32SendDelayTimeMilli )
{
	m_ui32SendDelayTimeMilli	= ui32SendDelayTimeMilli;
	m_ui32SendDelayInsertTime	= 0;
	m_i32SendSize				= 0;

	return TRUE;
}

void S2NetSession::Destroy()
{
	OnDisconnect();
}

void S2NetSession::OnUpdate()
{
m_CS.Enter();
	if( 0 == m_i32SendSize )
	{
		goto e_UpdateExit;
	}

	if( 0 < m_ui32SendDelayTimeMilli )
	{
		DWORD dwOldTime = GetTickCount();
		if( m_ui32SendDelayInsertTime + m_ui32SendDelayTimeMilli > dwOldTime )
		{
			goto e_UpdateExit;
		}
	}

	DWORD dwSendByte;
	_SendPacketMessage( m_pSendBuffer, m_i32SendSize, dwSendByte );
	m_i32SendSize				-= dwSendByte;
	if( 0 == m_i32SendSize )
	{
		m_ui32SendDelayInsertTime	= 0;
	}

e_UpdateExit:
m_CS.Leave();
}

BOOL S2NetSession::OnConnect(SOCKET hSocket, struct sockaddr_in * pAddr)
{	
	m_hSocket	= hSocket;

	////예외 처리 나올 가능성 없을듯 .
	//i3Net::GetIPToSockA( pAddr, m_strIPString );
	//i3Net::GetIPToSockW( pAddr, m_wstrIPString );

	//Set Real Adress
	m_ui32ConIP			= pAddr->sin_addr.s_addr; 
	m_ui16ConPort		= pAddr->sin_port;

	m_IsActive			= true; 
	m_i32ReceiveSize	= 0; 

	return TRUE; 
}

BOOL S2NetSession::OnConnectInit()
{
	WaitPacketReceive();
	return TRUE;
}

BOOL S2NetSession::OnDisconnect( BOOL bForceMainThread )
{	
	m_strIPString[0]	= '\0';

	if( INVALID_SOCKET != m_hSocket )
	{
		::shutdown( m_hSocket, SD_BOTH );
		::closesocket( m_hSocket );
		m_hSocket = INVALID_SOCKET;
	}

	m_IsActive	= false; 

	return TRUE;
}

void S2NetSession::Dispatch( DWORD dwTransferSize )
{
	if( 0 == dwTransferSize )	return;
	_DispatchReceive( dwTransferSize );
}

void S2NetSession::_DispatchReceive(DWORD dwTransferbytes)
{	
	m_i32ReceiveSize	= m_i32ReceiveSize + dwTransferbytes; 

	//1. 패킷의 사이즈가 패킷 버퍼 보다 클때 //절대로 나오면 않됨
	if( S2_PACKET_BUFFER_SIZE < m_i32ReceiveSize )
	{//Error ...에러 입니다. //로그를 남기는 것이 좋습니다.  
		S2TRACE(  _T("[S2NetSession::_DispatchReceive] Packet Buffer Over\n") );
		S2ASSERT(0);
		return;
	}
    
	INT32 iReadSize = 0;
	INT32 i32PacketRv;
	while( ( (m_i32ReceiveSize - iReadSize) >= sizeof(PACKET_SIZE) ) )
	{		 
		i32PacketRv = PacketParsing( &m_pReceiveBuffer[iReadSize], (m_i32ReceiveSize - iReadSize) ); 
		if( 0 == i32PacketRv ) break;
		iReadSize = iReadSize + i32PacketRv; 
	}

	//만약 받은 패킷보다 더 읽으면 에러 // 이부분은 PacketParsing이 함수에서 처리를 해줘야 합니다. 
	if( iReadSize > m_i32ReceiveSize ) 
	{
		m_i32ReceiveSize = 0;
		return;
		//I3ASSERT_0; 
	}
	 
	m_i32ReceiveSize = m_i32ReceiveSize - iReadSize;	
	//받은만큼 처리 못했으면. 버퍼 이동
	if( 0 < m_i32ReceiveSize )
	{		
		memmove( m_pReceiveBuffer, m_pReceiveBuffer + iReadSize, m_i32ReceiveSize );
	}    
	return; 
}
	
bool S2NetSession::WaitPacketReceive(void)
{
	if( INVALID_SOCKET == m_hSocket )	return false;
	DWORD readBytes = 0;

	S2Memory::FillZero( &m_OverlappedRecv, sizeof( OVERLAPPED_S2 ) );
	m_OverlappedRecv.m_ui32Flags = SESSION_ASYNCFLAG_RECEIVE;
	
	//이부분도 수정되어야 합니다. 
	if( !::ReadFile( ( HANDLE )m_hSocket, &m_pReceiveBuffer[m_i32ReceiveSize], ( S2_PACKET_BUFFER_SIZE - (m_i32ReceiveSize) ), &readBytes, ( LPOVERLAPPED )&m_OverlappedRecv ) )
	{
		DWORD lastError = ::GetLastError();
		switch( lastError )
		{
		case ERROR_IO_PENDING:
		case ERROR_SUCCESS:
			break;
		default:	
			S2TRACE(  _T("[S2NetSession::WaitPacketReceive] Packet Receive Error : %d\n"), lastError);
			return false;
		}
	}	

	return true; 
}

BOOL S2NetSession::SendPacketMessage( char * pPacket, INT32 i32Size )
{
m_CS.Enter();
	BOOL bResult = TRUE;
	if( S2_PACKET_BUFFER_SIZE < m_i32SendSize + i32Size )
	{
		bResult = FALSE;
		goto e_SendExit;
	}

	S2Memory::Copy( &m_pSendBuffer[ m_i32SendSize ], pPacket, i32Size );
	m_i32SendSize += i32Size;

	if( 0 < m_ui32SendDelayTimeMilli )
	{
		DWORD dwOldTime = GetTickCount();
		if( 0 == m_ui32SendDelayInsertTime )
		{
			m_ui32SendDelayInsertTime = dwOldTime;
			goto e_SendExit;
		}
		if( m_ui32SendDelayInsertTime + m_ui32SendDelayTimeMilli > dwOldTime )
		{
			goto e_SendExit;
		}
	}
				
	DWORD dwSendByte;
	_SendPacketMessage( m_pSendBuffer, m_i32SendSize, dwSendByte );
	m_i32SendSize				-= dwSendByte;
	if( 0 == m_i32SendSize )
	{
		m_ui32SendDelayInsertTime	= 0;
	}

e_SendExit:
m_CS.Leave();
	return bResult;
}

BOOL S2NetSession::_SendPacketMessage( char * pPacket, INT32 i32Size, DWORD& dwSendByte )
{
	S2Memory::FillZero( &m_OverlappedSend, sizeof( OVERLAPPED_S2 ) );
	m_OverlappedSend.m_ui32Flags = SESSION_ASYNCFLAG_SEND;
	
	if( !::WriteFile( ( HANDLE )m_hSocket, pPacket, i32Size, &dwSendByte, ( LPOVERLAPPED )&m_OverlappedSend ) )
	{
		DWORD lastError = ::GetLastError();
		switch( lastError )
		{
		case ERROR_IO_PENDING:
		case ERROR_SUCCESS:
			break;
		default:	
			S2TRACE(  _T("[S2NetSession::SendPacketMessage] Packet Send Error : %d\n"), lastError);	
			return FALSE;
		}
	}
	return TRUE;
}

BOOL S2NetSession::SendPacketMessage( S2Packet* pPacket )
{
	return SendPacketMessage( pPacket->GetPacketBuffer(), pPacket->GetPacketTotalSize() );
}
