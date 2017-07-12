#ifndef __S2_NET_IOCP_WORKER_H__
#define __S2_NET_IOCP_WORKER_H__

class S2NetSessionManager; 
class S2NetCompletionHandler; 

class S2_EXPORT_BASE S2NetIocpWorker 
{			
private:
	BOOL					m_bThreadRun;
	HANDLE					m_hThread;

	HANDLE					m_hIOCP; 

protected: 
	INT32					m_i32Idx; 
	S2NetSessionManager	*	m_pSessionManager; 
	S2NetCompletionHandler*	m_pCompletionHandler; 
	
public:
	S2NetIocpWorker(); 
	virtual ~S2NetIocpWorker(); 

	BOOL					Create();
	void					OnDestory();
	void					Update( void );

	void					SetIocpHandle(HANDLE IocpHandle, S2NetSessionManager * pSessionManager, INT32 ThreadIdx, S2NetCompletionHandler *	pCompletionHandler)	
	{ 
		m_hIOCP				 = IocpHandle; 
		m_pSessionManager	 = pSessionManager; 
		m_i32Idx			 = ThreadIdx; 
		m_pCompletionHandler = pCompletionHandler; 
	}
};

#endif