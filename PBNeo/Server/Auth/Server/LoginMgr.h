#ifndef __LOGIN_MGR_H__
#define __LOGIN_MGR_H__

#include "LoginFree.h"

class CLoginMgr
{
	INT32					m_i32LoginCount;
	CLoginBase*				m_pLogin;

	INT32					m_i32LoginIdx;

public:
	CLoginMgr();
	~CLoginMgr();

	BOOL					Create( INT32 i32LoginCount, S2DB_CONNECT_INFO* pConnectInfo, S2RING_INFO* pPush, S2RING_INFO* pPop );
	void					Destroy();

	BOOL					InsertUser( RING_LOGIN_PUSH* pPush );

	INT32					GetPopBufferCount()							{	return m_i32LoginCount;							}
	S2RingBuffer*			GetPopBuffer( INT32 i32Idx )				{	return m_pLogin[ i32Idx ].GetPopBuffer();		}
};

#endif
