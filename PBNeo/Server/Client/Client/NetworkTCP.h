#pragma once

#define PACKETBUFFERSIZE			8192
#define CLIENT_SOCKET_BUFFER_SIZE	(PACKETBUFFERSIZE * 3)

#define TCP_DISCONNECT_TYPE_OTHER			0X80000001
#define TCP_DISCONNECT_TYPE_READ_ERROR		0X80000002

class CClientApp;

class CNetworkTCP
{
	SOCKET					m_hSocket;
	WSAEVENT				m_hRecvEvent;
	
	INT32					m_i32ReceivedPacketSize;
	char					m_pReceiveBuffer[ CLIENT_SOCKET_BUFFER_SIZE ];

	CClientApp*				m_pApp;

protected:
	char *					_GetRecvBuffer();
	INT32					_OnReceive();
	INT32					_SendPacketMessage( const char* pBuffer, INT32 i32Size );

public:
	CNetworkTCP( CClientApp* pApp );
	~CNetworkTCP();

	BOOL					Create( UINT32 ui32IP, UINT16 ui16Port );
	void					Destroy();

	void					DisConnect();

	INT32					SelectEvent(void);

	INT32					PacketParsing( char* pBuffer, INT32 i32Size );

	INT32					SendPacket( S2MOPacketBase* pPacket );
	
};
