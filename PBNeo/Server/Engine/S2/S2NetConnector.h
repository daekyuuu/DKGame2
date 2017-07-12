#ifndef __S2_NET_CONNECTOR_H__
#define __S2_NET_CONNECTOR_H__

class S2_EXPORT_BASE S2NetConnector : public S2SocketConnector
{		
protected:
	S2Thread				m_Thread;
	DATE32					m_dt32ReconnectTime;	

	INT32					m_i32RingPushCount;
	S2RingBuffer*			m_pPushRing;
	S2RingBuffer*			m_pPopRing;

public:
	S2NetConnector();
	virtual ~S2NetConnector();

	virtual BOOL			Create( UINT32 ui32IP, UINT16 ui16Port, CallbackRoutine CallBack = NULL, void* pAgent = NULL );
	void					Destroy();

	virtual void			OnUpdate();
	virtual BOOL			OnRunning();

	BOOL					CreateRingBuffer( INT32 i32PushCount, INT32 i32PushBufferSize, INT32 i32PushBufferCount, INT32 i32PopCount, INT32 i32PopBufferSize, INT32 i32PopBufferCount );

	S2RingBuffer*			GetPushRing()								{	return m_pPushRing;	}
	S2RingBuffer*			GetPopRing()								{	return m_pPopRing;	}
};

#endif
