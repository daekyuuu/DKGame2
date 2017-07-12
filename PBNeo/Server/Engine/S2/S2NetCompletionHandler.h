#ifndef __S2_NET_COMPLETION_HANDLER_H__
#define __S2_NET_COMPLETION_HANDLER_H__

#include "S2CriticalSection.h"

class S2NetIocpWorker; 
class S2NetSession; 
class S2NetSessionManager; 

class S2_EXPORT_BASE S2NetCompletionHandler 
{
private:
	 
	S2NetIocpWorker**		m_ppWorker;
	S2NetSession**			m_ppWorkSession; 
	S2CriticalSection		m_CS;
	S2NetSessionManager *	m_pSessionManager;

	HANDLE					m_hIOCP;
	INT32					m_i32WorkerCount;

	UINT64*					m_pui64PacketCount;
	UINT64*					m_pui64PacketSize;	

public:
	S2NetCompletionHandler(); 
	virtual ~S2NetCompletionHandler(); 

	BOOL					OnCreate( INT32 i32WorkCount, S2NetSessionManager * pSessionManager );
	BOOL					Destroy(void);    
	
	void					OnAddIOCP(HANDLE handle, ULONG_PTR keyValue);

	//Add만 필요함 
	BOOL					AddWorkingSession(S2NetSession *	pSession, INT32 Idx);
	void					RemoveWorkingSession(INT32 Idx)									{	m_ppWorkSession[Idx] = NULL; 		}


	void AddPacketInfo( INT32 Idx, INT32 i32PacketSize )
	{
		m_pui64PacketCount[Idx]++;
		m_pui64PacketSize[Idx]	+= i32PacketSize;
	}
	
	HANDLE					GetWorkerIOCPHandle(void)										{	return m_hIOCP;						}

	void					GetPacketInfo( UINT64* pi64Count, UINT64* pi32Size );
	void					GetPacketInfoThread( INT32 i32ThreadIdx, UINT64* pi64Count, UINT64* pi64Size );

	//inline	S2NetworkIocpWorker*	GetIocpWorker( UINT32 idx ) { return m_ppWorker[idx]; }
};

#endif