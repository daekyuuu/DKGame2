#include "stdafx.h"
#include "NetworkTCP.h"
#include "Client.h"

CNetworkTCP::CNetworkTCP( CClientApp* pApp )
{
	m_hSocket				=  INVALID_SOCKET;
	m_hRecvEvent			= INVALID_HANDLE_VALUE;
	m_i32ReceivedPacketSize	= 0;
	memset( m_pReceiveBuffer, 0, sizeof(m_pReceiveBuffer) );
	m_pApp					= pApp;
}

CNetworkTCP::~CNetworkTCP()
{
	Destroy();
}

BOOL CNetworkTCP::Create( UINT32 ui32IP, UINT16 ui16Port )
{
	WSADATA wsaData;		
	if( ::WSAStartup( MAKEWORD(2,2), &wsaData ) != 0 )
	{
		TRACE( _T("WSAStartup Failed. \n") );
		return FALSE; 
	}

	if( wsaData.wVersion != MAKEWORD(2,2) )
	{
		::WSACleanup();
		TRACE( _T("WSAStart Version Failed. \n") );
		return FALSE; 
	}

	// 리시브 이벤트 생성
	m_hRecvEvent = ::WSACreateEvent();

	// 소켓 생성
	m_hSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == m_hSocket)
	{
		return FALSE;
	}

	// 서버 접속
	struct sockaddr_in serverAddr;
	memset( &serverAddr, 0, sizeof(serverAddr) );
	serverAddr.sin_family		= AF_INET;
	serverAddr.sin_addr.s_addr	= ui32IP;
	serverAddr.sin_port			= ::htons(ui16Port);

	INT32 result = ::connect(m_hSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

	if (SOCKET_ERROR == result)
	{
		//i3Net::GetError("[S2ClientSocket::OnConnectTo]");
		return FALSE;
	}

	// 리시브 관련 멤버 초기화
	m_i32ReceivedPacketSize = 0;

	return TRUE;
}

void CNetworkTCP::Destroy()
{
	DisConnect();
}

void CNetworkTCP::DisConnect()
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

INT32 CNetworkTCP::SelectEvent()
{
	INT32 i32Rv = 0; 
	WSANETWORKEVENTS event;
	memset( &event, 0, sizeof(event) );

	// 처리할 이벤트를 확인한다.
	if( SOCKET_ERROR != ::WSAEventSelect( m_hSocket, m_hRecvEvent, FD_WRITE | FD_READ | FD_CLOSE ) )
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

	return i32Rv;
}

INT32 CNetworkTCP::_OnReceive(void)
{
	// 패킷을 받는다.
	INT32 receivedByte = ::recv(m_hSocket, _GetRecvBuffer(), PACKETBUFFERSIZE, 0);

	if (SOCKET_ERROR == receivedByte)
	{
		TCHAR strMessage[ MAX_PATH ];
		INT32 i32Error = ::WSAGetLastError();

		_stprintf_s( strMessage, MAX_PATH, _T("[S2ClientSocket::_OnReceive] lastError = %d"), i32Error );
		TRACE( strMessage );

		DisConnect();
		return TCP_DISCONNECT_TYPE_READ_ERROR;		
	}
	
	if (0 == receivedByte)
	{
		return receivedByte;	// ClosedSocket
	}

	
	// 받은 데이터 크기 누적
	m_i32ReceivedPacketSize += receivedByte;

	if (CLIENT_SOCKET_BUFFER_SIZE < m_i32ReceivedPacketSize)
	{
		TRACE( _T("[WARNING] S2ClientSocket::_OnReceive() - Buffer overflow(%d bytes)!\n"), m_i32ReceivedPacketSize);
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
		i32PacketSize = PacketParsing( m_pReceiveBuffer+i32StartIdx, m_i32ReceivedPacketSize-i32StartIdx );

		if (0 > i32PacketSize || PACKETBUFFERSIZE < i32PacketSize)
		{
			TRACE( _T("[WARNING] S2ClientSocket::_OnReceive() - Break packet size(%d bytes)!\n"), i32PacketSize);
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

	return receivedByte;
}

char *	CNetworkTCP::_GetRecvBuffer()
{
	return m_pReceiveBuffer + m_i32ReceivedPacketSize;
}

INT32 CNetworkTCP::SendPacket( S2MOPacketBase* pPacket )
{
	char pBuffer[ S2MO_PACKET_CONTENT_SIZE ];
	pPacket->Packing_T( pBuffer );
	return _SendPacketMessage( pBuffer, pPacket->GetPacketSize() );
}

INT32 CNetworkTCP::_SendPacketMessage( const char* pBuffer, INT32 i32Size )
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
			INT32 i32Error = ::WSAGetLastError();
			
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

	return i32SendIdx;
}

INT32 CNetworkTCP::PacketParsing( char* pBuffer, INT32 i32Size )
{	
	S2MOPacketBase PacketBase( PROTOCOL_VALUE_NONE );
	PacketBase.UnPacking_Head( pBuffer );

	T_PACKET_SIZE TSize = PacketBase.GetPacketSize();
	if( TSize > i32Size )		return 0;
		
	char* pContents = pBuffer + S2MO_PACKET_HEAD_SIZE;

	switch( PacketBase.GetProtocol() )
	{
	case PROTOCOL_BASE_CONNECT_ACK:
		{
			m_pApp->ConnectServerAck( pContents );
		}
		break;
	case PROTOCOL_BASE_NOTICE_ACK:
		{
		int kk = 0;
		}
		break;
	case PROTOCOL_LOGIN_INPUT_ACK:
		{
			m_pApp->LoginAck( pContents );
		}
		break;
	case PROTOCOL_USER_INFO_LOAD_ACK:
		{
			m_pApp->UserInfoLoadAck( pContents );
		}
		break;
	case PROTOCOL_USER_CHANGE_NICK_ACK:
		{
			m_pApp->UserChangeNickAck( pContents );
		}
		break;
	case PROTOCOL_ROOM_CREATE_ACK:
		{
			m_pApp->RoomCreateAck( pContents );
		}
		break;
	case PROTOCOL_ROOM_ENTER_ACK:
		{
			m_pApp->RoomEnterAck( pContents );
		}
		break;
	case PROTOCOL_ROOM_SLOT_INFO_ACK:
		{
			m_pApp->RoomSlotInfoAck( pContents );
		}
		break;
	case PROTOCOL_ROOM_LEAVE_ACK:
		{
			m_pApp->RoomLeaveAck( pContents );
		}
		break;
	case PROTOCOL_ROOM_ENTER_TRANS_ACK:
		{
			m_pApp->RoomEnterTransAck( pContents );
		}
		break;
	case PROTOCOL_ROOM_LEAVE_TRANS_ACK:
		{
			m_pApp->RoomLeaveTransAck( pContents );
		}
		break;
	case PROTOCOL_ROOM_SLOT_READY_ACK:
		break;
	case PROTOCOL_ROOM_STATE_CHANGE_ACK:
		{
			m_pApp->RoomStateChangeAck( pContents );
		}
		break;
	case PROTOCOL_ROOM_MAP_VOTE_TRANS_ACK:
		{
			m_pApp->RoomMapVoteAck( pContents );
		}
		break;
	case PROTOCOL_ROOM_SLOT_STATE_CHANGE_ACK:
		{
			m_pApp->RoomSlotStateChangeAck( pContents );
		}
		break;
	case PROTOCOL_ROOM_BATTLE_START_ACK:
		{
			m_pApp->RoomBattleStartAck( pContents );
		}
		break;
	case PROTOCOL_ROOM_BATTLE_END_ACK:
		{
			m_pApp->RoomBattleEndAck( pContents );
		}
		break;
	case PROTOCOL_ROOM_LIST_ACK:
		{
			m_pApp->RoomList( pContents );
		}
		break;
	}

	return TSize;
}