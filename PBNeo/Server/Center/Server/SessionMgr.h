#ifndef __SESSION_MGR_H__
#define __SESSION_MGR_H__

#include "Session.h"

#define SERVER_COUNT_MAX		100

class CSessionMgr : public S2NetSessionManager
{
	INT32					m_i32ThreadCount;

	S2RingBuffer*			m_pPopRing;

	INT32					m_i32ServerListCount[ SERVER_TYPE_COUNT ];
	CSession*				m_pServerList[ SERVER_TYPE_COUNT ][ SERVER_COUNT_MAX ];

public:
	CSessionMgr();
	virtual ~CSessionMgr();

	BOOL					Create( INT32 i32SessionCount, INT32 i32ThreadCount, INT32 i32SessionPacketSize, INT32 i3SessionBufferCount );
	void					Destroy();

	void					SetPacketBuffer();

	BOOL					SetServerType( SERVER_TYPE eType, UINT32 ui32Idx, CSession* pSession );
	CSession*				GetServerType( SERVER_TYPE eType, UINT32 ui32Idx = 0 );

	RING_IOCP_POP*			GetMainPop( INT32 i32Idx );
	void					GetMainPopIdx( INT32 i32Idx );

	BOOL					SendPacketServer( SERVER_TYPE eType, S2MOPacketBase* pPacket );
};
#endif
