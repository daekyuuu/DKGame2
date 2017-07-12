#include "stdafx.h"
#include "S2SocketConnector.h"
#include "S2Packet.h"
#include "S2Net.h"


S2SocketConnector::S2SocketConnector()
{
	m_hSocket				=  INVALID_SOCKET;
	m_hRecvEvent			= INVALID_HANDLE_VALUE;
	m_i32ReceivedPacketSize	= 0;
	S2Memory::FillZero( m_pReceiveBuffer, sizeof(m_pReceiveBuffer) );
}

S2SocketConnector::~S2SocketConnector()
{
	OnDestroy();
}

BOOL S2SocketConnector::Create( TCHAR* strIP,UINT16 ui16Port, CallbackRoutine CallBack, void* pAgent )
{	
	UINT32 ui32IP = S2Net::GetIPToLong( strIP );

	return _Create( ui32IP, ui16Port, CallBack, pAgent );
}

BOOL S2SocketConnector::Create( UINT32 ui32IP,UINT16 ui16Port, CallbackRoutine CallBack, void* pAgent )
{
	return _Create( ui32IP, ui16Port, CallBack, pAgent );
}

INT32 S2SocketConnector::_OnReceive(void)
{
	m_dt32LastRecvTime = S2Time::StandTime();

	// 패킷을 받는다.
	INT32 i32ReceivedByte = ::recv( m_hSocket, _GetRecvBuffer(), S2_PACKET_BUFFER_SIZE, 0 );

	if( SOCKET_ERROR == i32ReceivedByte )
	{
		TCHAR strMessage[ S2_STRING_COUNT ];
		INT32 i32Error = ::WSAGetLastError();

		S2String::Format( strMessage, S2_STRING_COUNT, _T("[S2SocketConnector::_OnReceive] lastError = %d"), i32Error );
		//i3Net::GetError(message);

		DisConnect();
		return TCP_DISCONNECT_TYPE_READ_ERROR;		
	}
	
	if( 0 == i32ReceivedByte )
	{
		return i32ReceivedByte;	// ClosedSocket
	}

	
	// 받은 데이터 크기 누적
	m_i32ReceivedPacketSize += i32ReceivedByte;

	if( S2_PACKET_BUFFER_SIZE < m_i32ReceivedPacketSize )
	{
		S2TRACE( _T("[WARNING] S2SocketConnector::_OnReceive() - Buffer overflow(%d bytes)!\n") , m_i32ReceivedPacketSize);
		DisConnect();
		return TCP_DISCONNECT_TYPE_READ_ERROR;
	}


	// 버퍼상의 파싱 시작 위치 
	INT32 i32StartIdx = 0;

	// 수신한 모든 패킷을 파싱한다.
	for(;;)
	{
		// 패킷 사이즈의 버퍼 오버플로우 검사
		INT32 i32PacketSize;
		if( m_pCallbackFunc )
		{
			i32PacketSize = (*m_pCallbackFunc)( m_pAgent, m_pReceiveBuffer+i32StartIdx, m_i32ReceivedPacketSize-i32StartIdx );
		}
		else
		{
			i32PacketSize = PacketParsing( m_pReceiveBuffer+i32StartIdx, m_i32ReceivedPacketSize-i32StartIdx );
		}

		if (0 > i32PacketSize || S2_PACKET_BUFFER_SIZE < i32PacketSize )
		{
			S2TRACE( _T("[WARNING] S2SocketConnector::_OnReceive() - Break packet size(%d bytes)!\n"), i32PacketSize );
			DisConnect();
			return TCP_DISCONNECT_TYPE_READ_ERROR;
		}
		if( 0 == i32PacketSize ) break;

		i32StartIdx += i32PacketSize;
		if( i32StartIdx >= m_i32ReceivedPacketSize )
		{
			break;
		}
	}

	// 파싱 후 남은 데이터를 재정렬한다.
	m_i32ReceivedPacketSize -= i32StartIdx;
	if (0 < i32StartIdx && 0 < m_i32ReceivedPacketSize)
	{
		memmove(m_pReceiveBuffer, m_pReceiveBuffer + i32StartIdx, m_i32ReceivedPacketSize);
	}

	return i32ReceivedByte;
}

void S2SocketConnector::DisConnect()
{
	if (INVALID_HANDLE_VALUE != m_hRecvEvent)
	{
		::WSACloseEvent(m_hRecvEvent);
		m_hRecvEvent = INVALID_HANDLE_VALUE;
	}

	if (INVALID_SOCKET != m_hSocket)
	{
		::shutdown(m_hSocket, SD_BOTH);
		::closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}

	m_i32ReceivedPacketSize = 0;
}

void S2SocketConnector::OnDestroy(void)
{
	

	DisConnect();
}

INT32 S2SocketConnector::PacketParsing( char* pBuffer, INT32 i32Size )
{
	// Do nothing
	return 0;
}

INT32 S2SocketConnector::SendPacketMessage( S2Packet* pPacket )
{
	return SendPacketMessage( pPacket->GetPacketBuffer(), pPacket->GetPacketSize() );
}

INT32 S2SocketConnector::SendPacketMessage( const char* pBuffer, INT32 i32Size )
{
	// 소켓이 닫혔으면 보네지 않는다.
	if (INVALID_SOCKET == m_hSocket)	return 0;

	// 패킷을 다 보낼때까지 반복한다.
	INT32 i32SendIdx = 0;
	while( i32Size > i32SendIdx)
	{
		INT32 sendedByte = ::send(m_hSocket, pBuffer + i32SendIdx, i32Size - i32SendIdx, 0);

		// 전송 실패 
		if (SOCKET_ERROR == sendedByte)
		{
			TCHAR strMessage[ S2_STRING_COUNT ];
			INT32 i32Error = ::WSAGetLastError();

			S2String::Format( strMessage, S2_STRING_COUNT, _T("[S2SocketConnector::SendPacketMessage] lastError = %d"), i32Error );
			//i3Net::GetError(message);
			
			return -1;
		}

		// 연결이 끊어졌다.
		if (0 == sendedByte)
		{
			return 0;
		}

		// 전송 바이트만큼 인덱스 증가
		i32SendIdx += sendedByte;
	}

	m_dt32LastSendTime = S2Time::StandTime();

	return i32SendIdx;
}

INT32 S2SocketConnector::SelectEvent(void)
{
	INT32 i32Rv = 0; 
	WSANETWORKEVENTS event;
	S2Memory::FillZero(&event, sizeof(event));

	// 처리할 이벤트를 확인한다.
	if( SOCKET_ERROR != ::WSAEventSelect(m_hSocket, m_hRecvEvent, FD_WRITE | FD_READ | FD_CLOSE) )
	{
		if( SOCKET_ERROR != ::WSAEnumNetworkEvents(m_hSocket, m_hRecvEvent, &event ) )
		{
			if( event.lNetworkEvents & FD_READ )
			{
				i32Rv = _OnReceive();
			}
			if( event.lNetworkEvents & FD_CLOSE )
			{
				DisConnect();
				i32Rv = TCP_DISCONNECT_TYPE_OTHER;
			}

			return i32Rv;
		}
	}

	// 오류 발생시 처리한다.
	// 오류 발생 원인이 동일하기때문에 출력을 구분하지 않습니다.
	TCHAR strMessage[ S2_STRING_COUNT ];	
	INT32 i32Error = ::WSAGetLastError();
	S2String::Format( strMessage, S2_STRING_COUNT, _T("[S2SocketConnector::SelectEvent] lastError = %d"), i32Error );
	//i3Net::GetError(message);

	return i32Rv;
}

BOOL S2SocketConnector::IsConnected(void) const
{
	if (INVALID_SOCKET == m_hSocket)
	{
		return FALSE;
	}
	
	return TRUE;
}

SOCKET S2SocketConnector::_GetSocket(void)
{
	return m_hSocket;
}

char *	S2SocketConnector::_GetRecvBuffer()
{
	return m_pReceiveBuffer + m_i32ReceivedPacketSize;
}

BOOL S2SocketConnector::SetSocket(SOCKET Socket)
{
	// 리시브 이벤트 생성
	m_hRecvEvent = ::WSACreateEvent();

	// 소켓 생성
	m_hSocket = Socket;

	// 리시브 관련 멤버 초기화
	m_i32ReceivedPacketSize = 0;

	return TRUE;
}

BOOL S2SocketConnector::_Create( UINT32 ui32IP, UINT16 ui16Port, CallbackRoutine CallBack, void* pAgent )
{
	m_ui32IP		= ui32IP;
	m_ui16Port		= ::htons(ui16Port);
	
	if( FALSE == _Connect() )
	{
		return FALSE;
	}

	m_pAgent				= pAgent;
	m_pCallbackFunc			= CallBack;	

	return TRUE;
}

BOOL S2SocketConnector::_Connect()
{
	// 리시브 이벤트 생성
	m_hRecvEvent	= ::WSACreateEvent();

	// 서버 접속
	struct sockaddr_in serverAddr;
	S2Memory::FillZero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family		= AF_INET;
	serverAddr.sin_addr.s_addr	= m_ui32IP;
	serverAddr.sin_port			= m_ui16Port;

	// 소켓 생성
	m_hSocket		= ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == m_hSocket)
	{
		S2TRACE( _T("[S2SocketConnector::OnConnectTo] Error Socket Is Invalid \n") );
		DisConnect();
		return FALSE;
	}

	INT32 i32Result = ::connect(m_hSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

	if( SOCKET_ERROR == i32Result )
	{
		DisConnect();
		return FALSE;
	}

	// 리시브 관련 멤버 초기화
	m_i32ReceivedPacketSize = 0;

	return TRUE;
}