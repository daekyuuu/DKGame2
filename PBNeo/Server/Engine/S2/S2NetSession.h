#ifndef __S2_NET_SESSION_H__
#define __S2_NET_SESSION_H__

class S2Packet;

struct S2_EXPORT_BASE OVERLAPPED_S2 : OVERLAPPED
{
	UINT32		m_ui32Flags;
};

#define SESSION_ASYNCFLAG_SEND		0x01
#define SESSION_ASYNCFLAG_RECEIVE	0x02
#define	IPADDRESS_STRING_SIZE		32

class S2_EXPORT_BASE S2NetSession 
{
	OVERLAPPED_S2			m_OverlappedSend;
	OVERLAPPED_S2			m_OverlappedRecv;

	bool					m_IsActive;

	UINT32					m_ui32ConIP;		// StrIP 를 얻고 싶다면 GetConnectIPString 함수를 이용하세요.
	UINT16					m_ui16ConPort;

	INT32					m_i32ReceiveSize;
	char					m_pReceiveBuffer[ S2_PACKET_BUFFER_SIZE ];

	UINT32					m_ui32SendDelayTimeMilli;
	UINT32					m_ui32SendDelayInsertTime;
	INT32					m_i32SendSize;
	char					m_pSendBuffer[ S2_PACKET_BUFFER_SIZE ];

	S2CriticalSection		m_CS;
protected:
	SOCKET					m_hSocket;
	INT32					m_i32SessionIdx;

	//For Work Thread 
	INT32					m_i32WorkThreadIdx;

	TCHAR					m_strIPString[ IPADDRESS_STRING_SIZE ];

protected: 
	void					_DispatchReceive( DWORD Transferbytes ); 
	BOOL					_SendPacketMessage( char * pPacket, INT32 i32Size, DWORD& dwSendByte );

public:
	S2NetSession();
	virtual ~S2NetSession();

	virtual BOOL			Create( UINT32 ui32SendDelayTimeMilli = 0 );
	virtual void			Destroy();
	virtual void			OnUpdate();

	virtual INT32			PacketParsing( char * pPacket, INT32 i32Size )		= NULL;	

	virtual BOOL			OnConnect(SOCKET Socket, struct sockaddr_in * pAddr);
	virtual BOOL			OnConnectInit();
	virtual BOOL			OnDisconnect(BOOL bForceMainThread = FALSE);

	void					Dispatch( DWORD dwTransferSize ); 
	bool					WaitPacketReceive(); 

	void					SetWorkThreadIdx( INT32 i32WorkThreadIdx )					{	m_i32WorkThreadIdx	= i32WorkThreadIdx;	}
	void					SetSessionIdx( INT32 i32SessionIdx )						{	m_i32SessionIdx		= i32SessionIdx;	}
		
	virtual BOOL			SendPacketMessage( char * pPacket, INT32 iSize ); 
	virtual BOOL			SendPacketMessage( S2Packet* pPacket ); 

	bool					GetIsActive()												{	return m_IsActive;						}
	INT32					GetWorkThreadIdx()											{	return m_i32WorkThreadIdx;				}
	INT32					GetSessionIdx()												{	return m_i32SessionIdx;					}
	
	UINT32					GetConnectIP()												{	return m_ui32ConIP;						}
	UINT16					GetConnectPort()											{	return m_ui16ConPort;					}
	//void					GetConnectIPString( TCHAR* strIP, INT32 i32Size );
};

#endif
