#ifndef __S2_THREAD_H__
#define __S2_THREAD_H__

class S2_EXPORT_BASE S2Thread
{
	HANDLE					m_hThread;
	DWORD					m_dwThreadID;
	BOOL					m_bRunning;

public:
	S2Thread();
	~S2Thread();

	BOOL					Create( LPTHREAD_START_ROUTINE Proc, void* pPoint, INT32 i32ThreadPriority = THREAD_PRIORITY_BELOW_NORMAL );
	void					Destroy();
	void					Init();
	void					Terminate();
	UINT32					Waiting( UINT32 ui32Millseconds );

	//HANDLE					GetHandle()					{	return m_hThread;			}
	DWORD					GetThreadID()				{	return m_dwThreadID;		}
	BOOL					IsRunning()					{	return m_bRunning;			} 

};

#endif
