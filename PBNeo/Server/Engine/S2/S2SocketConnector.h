#ifndef __S2_SOCKET_CONNECTOR_H__
#define __S2_SOCKET_CONNECTOR_H__

#define TCP_DISCONNECT_TYPE_OTHER			0X80000001
#define TCP_DISCONNECT_TYPE_READ_ERROR		0X80000002

class i3NetworkPacket;

typedef	INT32 ( *CallbackRoutine )( void* pAgent, char* pPacket, INT32 i32Size );

class S2_EXPORT_BASE S2SocketConnector
{
protected:
	SOCKET					m_hSocket;
	WSAEVENT				m_hRecvEvent;

	UINT32					m_ui32IP;
	UINT16					m_ui16Port;
	
	INT32					m_i32ReceivedPacketSize;
	char					m_pReceiveBuffer[ S2_PACKET_BUFFER_SIZE ];

	// for callback
	void*					m_pAgent;
	CallbackRoutine			m_pCallbackFunc;

	DATE32					m_dt32LastSendTime;
	DATE32					m_dt32LastRecvTime;

protected:
	BOOL					_Create( UINT32 ui32IP, UINT16 ui16Port, CallbackRoutine CallBack, void* pAgent );
	BOOL					_Connect();
	SOCKET					_GetSocket(void);
	char *					_GetRecvBuffer();
	virtual INT32			_OnReceive(void);

public:
	S2SocketConnector();
	virtual ~S2SocketConnector();

	virtual BOOL			Create( TCHAR* strIP,UINT16 ui16Port, CallbackRoutine CallBack = NULL, void* pAgent = NULL );
	virtual BOOL			Create( UINT32 ui32IP,UINT16 ui16Port, CallbackRoutine CallBack = NULL, void* pAgent = NULL );
	BOOL					SetSocket(SOCKET Socket); 

	virtual void			OnDestroy(void);
	virtual INT32			PacketParsing( char* pBuffer, INT32 i32Size );
	virtual INT32			SendPacketMessage( S2Packet* pPacket );
	virtual INT32			SendPacketMessage( const char* pBuffer, INT32 i32Size );
	
	INT32					SelectEvent(void);	
	BOOL					IsConnected(void) const;
	virtual void			DisConnect();
};

#endif
