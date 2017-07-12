#ifndef __SESSION_H__
#define __SESSION_H__

class CSession : public S2NetSession
{
	S2RingBuffer*			m_pMainPop;

	S2RingBuffer*			m_pCenterPush;

public:
	T_UID					m_TUID;

protected:
	BOOL					_PushRing( S2RingBuffer* pRing, INT32 i32Size, char* pPacket );

public:
	CSession();
	~CSession();

	BOOL					Create( UINT32 ui32SendDelayTimeMilli = 0 );
	void					Destroy();

	virtual BOOL			OnConnectInit();
	virtual BOOL			OnDisconnect( BOOL bForceMainThread = FALSE );


	INT32					SendPacket( S2MOPacketBase* pPacket );
	virtual INT32			PacketParsing( char * pPacket, INT32 i32Size );

	void					SetPacketBuffer( S2RingBuffer* pMainPop, S2RingBuffer* pCenterPush );
};

#endif
