#ifndef __SESSION_H__
#define __SESSION_H__

class CSession : public S2NetSession
{
	SERVER_TYPE				m_eServerType;
	INT32					m_i32ServerIdx;
	UINT32					m_ui32ServerKey;


	S2RingBuffer*			m_pMainPop;

public:
	CSession();
	~CSession();

	BOOL					Create( UINT32 ui32SendDelayTimeMilli = 0 );
	void					Destroy();

	virtual BOOL			OnConnectInit();
	virtual BOOL			OnDisconnect(BOOL bForceMainThread = FALSE);

	BOOL					SendPacket( char* pPacket );
	BOOL					SendPacket( S2MOPacketBase* pPacket );

	virtual INT32			PacketParsing( char * pPacket, INT32 i32Size );

	void					SetPacketBuffer( S2RingBuffer* pMainPop );

	void					SetServerInfo( SERVER_TYPE eType, INT32 i32Idx );
};

struct RING_IOCP_POP
{
	SERVER_TYPE				m_eServerType;
	INT32					m_i32ServerIdx;
	CSession*				m_pSession;
	char					m_pBuffer[ S2_PACKET_MAX_SIZE ];
};

#endif
