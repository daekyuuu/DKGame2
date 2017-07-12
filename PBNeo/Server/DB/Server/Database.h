#ifndef __DATABASE_H__
#define __DATABASE_H__

struct RING_DATA
{
};

class CDatabase
{
	S2DBAdo*				m_pDB;
	S2Thread				m_Thread;

	S2RingBuffer*			m_pRingPush;
	S2RingBuffer*			m_pRingPop;

protected:

	void					_GetUserInfo( RING_DB_PUSH* pPush, RING_DB_POP* pPop );
	void					_SetUserInfo( RING_DB_PUSH* pPush, RING_DB_POP* pPop );

public:
	CDatabase();
	~CDatabase();

	BOOL					Create( S2DB_CONNECT_INFO* pConnectInfo, S2RING_INFO* pPush, S2RING_INFO* pPop );
	void					Destroy();

	void					OnUpdate();

	T_RESULT				PushData( RING_DB_PUSH* pPush );
	S2RingBuffer*			GetPopBuffer()				{	return m_pRingPop;	}
};

#endif
