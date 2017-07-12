#ifndef __USER_MGR_H__
#define __USER_MGR_H__

#include "UserContainer.h"

class CUserMgr
{
	S2ListBin				m_UserNode[ USER_NODE_COUNT ];
	CUserContainer*			m_pUserContainer;

public:
	CUserMgr();
	~CUserMgr();

	BOOL					Create( UINT32 ui32UserCount );
	void					Destroy();

	USER_LIST*				InsertUser( USER_LIST* pUser, T_RESULT& TResult );
	BOOL					DeleteUser( USER_LIST* pUser );

	USER_LIST*				FindUser( T_UID& TUID );
};

#endif
