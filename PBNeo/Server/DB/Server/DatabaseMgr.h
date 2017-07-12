#ifndef __DATABASE_MGR_H__
#define __DATABASE_MGR_H__

#include "Database.h"

class CDatabaseMgr
{
	INT32					m_i32DBCount;
	CDatabase*				m_pDB;

public:
	CDatabaseMgr();
	~CDatabaseMgr();

	BOOL					Create( INT32 i32DBCount, S2DB_CONNECT_INFO* pConnectInfo, S2RING_INFO* pPush, S2RING_INFO* pPop );
	void					Destroy();

	T_RESULT				Push( RING_DB_PUSH* pPush );

	INT32					GetPopBufferCount()							{	return m_i32DBCount;						}
	S2RingBuffer*			GetPopBuffer( INT32 i32Idx )				{	return m_pDB[ i32Idx ].GetPopBuffer();		}
};

#endif
