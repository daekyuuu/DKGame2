#ifndef __S2_NET_SESSION_MANAGER_H__
#define __S2_NET_SESSION_MANAGER_H__

#include "S2ContainerList.h"
#include "S2CriticalSection.h"
#include "S2List.h"

class S2NetSession; 

class S2_EXPORT_BASE S2NetSessionManager
{ 
private:
	S2ContainerList			m_ContainerList;

	INT32					m_i32ActiveSessionCount; 
	S2CriticalSection		m_CS;

protected: 
	INT32					m_i32NewSessionIdx;
	INT32					m_i32MaxSessionIdx; 
	S2NetSession	**		m_pSessionList;
	
public: 	
	S2NetSessionManager(); 	
	virtual ~S2NetSessionManager();

	virtual BOOL			Create( INT32 i32SessionCount );
	virtual void			Update(void); 
	virtual void			Destroy(void);

	BOOL					AddSession( INT32 i32Idx, S2NetSession* pSession );

	ULONG_PTR				ConnectSession(SOCKET Socket, struct sockaddr_in * pAddr); 
	void					DisConnectSession(INT32 i32ThreadIdx, S2NetSession * pSession, bool bForceMain); 

	INT32					GetSessionCount(void)																	{ return m_i32ActiveSessionCount; }	
	void					DisConnectAllSession(INT32 i32ThreadIdx);

	//Get & Set
	S2NetSession*			GetSessionList( INT32 ItemIdx )															{ return m_pSessionList[ItemIdx]; }
};

#endif