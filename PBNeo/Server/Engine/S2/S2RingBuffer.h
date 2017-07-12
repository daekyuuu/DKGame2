#ifndef __S2_RING_BUFFER_H__
#define __S2_RING_BUFFER_H__

enum RING_BUFFER_RESULT
{
	RING_BUFFER_SUCCESS					= 0x00000000,
	RING_BUFFER_ERROR_BUFFER_FULLIDX	= 0x80000001,
	RING_BUFFER_ERROR_BUFFER_FULL		= 0x80000002,
};

struct S2_EXPORT_BASE S2RING_INFO
{
	UINT32					m_ui32BufferSize; 
	UINT32					m_ui32BufferCount; 

	S2RING_INFO::S2RING_INFO( INT32 i32Size, INT32 i32Count )
	{
		m_ui32BufferSize	= i32Size;
		m_ui32BufferCount	= i32Count;
	}
};

class S2_EXPORT_BASE S2RingBuffer
{
protected: 
	UINT32					m_ui32BufferSize; 
	UINT32					m_ui32BufferCount; 
	
	UINT32					m_ui32WriteIdx; 
	UINT32					m_ui32ReadIdx; 

	char*					m_pBuffer; 

public:
	S2RingBuffer();
	virtual ~S2RingBuffer();

	BOOL					Create( S2RING_INFO* pInfo );
	BOOL					Create( INT32 i32BufferSize, INT32 i32BufferCount ); 
	void					Destroy(void);

	inline INT32			GetEmptyBufferCount(void);							// 남은 버퍼의 카운트를 전달 한다. 
	inline INT32			GetBufferCount(void);								// 사용하는 버퍼의 카운트를 전달한다. 

	//Data Save 
	INT32					Push( void * pBuffer );								//데이터 Save 
	void*					PushPointer(void);									//데이터 Save Pointer		-	PushPointerIdx	<- 이것과 한쌍입니다.
    void					PushPointerIdx(void);								//데이터 Save (Add Idx)		-	PushPointer		<- 이것과 한쌍입니다.

	//Data Load 
	void*					Pop(void);											//데이터 Read
	void					PopIdx(void);										//데이터 ReadIdx 
};

#endif