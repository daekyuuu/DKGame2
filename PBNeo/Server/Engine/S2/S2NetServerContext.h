#ifndef __S2_NET_SERVER_CONTEXT_H__
#define __S2_NET_SERVER_CONTEXT_H__

class S2NetAcceptor; 
class S2NetCompletionHandler; 
class S2NetSessionManager; 

class S2_EXPORT_BASE S2NetServerContext 
{
	S2Thread					m_Thread;

	S2NetAcceptor*				m_pAcceptor;				// 郴何俊辑 积己 
	S2NetCompletionHandler*		m_pCompletionHandler;		// 郴何俊辑 积己 
	S2NetSessionManager*		m_pSessionManager;		// 寇何俊辑 积己 

protected: 
	INT32						UpdateAcceptor(void); 

public :
	S2NetServerContext(void);
	virtual ~S2NetServerContext(void); 
	
	virtual BOOL				Create( INT32 i32SocketCount, UINT32* pui32IP, UINT16* pui16Port, INT32 i32WorkCount, S2NetSessionManager* pSessionManager);
	virtual BOOL				Create( INT32 i32SocketCount, UINT32* pui32IP, UINT16* pui16Port, INT32 i32WorkCount );
	virtual void				OnUpdate(void); 	
	virtual void				Destroy(void); 

	//Check Server 
	INT32						GetSessionCount(void);
	inline void					GetPacketInfo( UINT64* pi64Count, UINT64* pi64Size );
	inline void					GetPacketInfoThread( INT32 i32ThreadIdx, UINT64* pi64Count, UINT64* pi64Size );
};

#endif
