#ifndef __SESSION_MGR_H__
#define __SESSION_MGR_H__

class CSessionMgr : public S2NetSessionManager
{
	INT32					m_i32ThreadCount;

	S2RingBuffer*			m_pPopRing;

public:
	CSessionMgr();
	virtual ~CSessionMgr();

	BOOL					Create( INT32 i32SessionCount, INT32 i32ThreadCount, INT32 i32SessionPacketSize, INT32 i3SessionBufferCount );
	void					Destroy();

	void					SetPacketBuffer( S2RingBuffer* pCenter );

	S2RingBuffer*			GetPopRing( INT32 i32Idx )					{	return &m_pPopRing[ i32Idx ];	}
};
#endif
