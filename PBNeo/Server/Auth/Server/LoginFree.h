#ifndef __LOGIN_SELF_H__
#define __LOGIN_SELF_H__

#include "LoginBase.h"

class CLoginFree : public CLoginBase
{
	S2DBAdo*				m_pDB;

public:
	CLoginFree();
	~CLoginFree();

	BOOL					Create( S2DB_CONNECT_INFO* pConnectInfo, S2RING_INFO* pPush, S2RING_INFO* pPop );
	void					Destroy();

	BOOL					OnUpdate();
};

#endif
