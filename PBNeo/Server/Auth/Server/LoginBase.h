#ifndef __LOGIN_BASE_H__
#define __LOGIN_BASE_H__

class CLoginBase
{
protected:
	S2Thread				m_Thread;

	S2RingBuffer*			m_pRingPush;
	S2RingBuffer*			m_pRingPop;

public:
	CLoginBase();
	virtual ~CLoginBase();

	virtual BOOL			Create( S2DB_CONNECT_INFO* pConnectInfo, S2RING_INFO* pPush, S2RING_INFO* pPop );
	void					Destroy();

	virtual void			OnRun();

	virtual BOOL			OnUpdate();

	BOOL					InsertUser( RING_LOGIN_PUSH* pPush );

	S2RingBuffer*			GetPopBuffer()					{	return m_pRingPop;	}
};

#endif
