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

	inline INT32			GetEmptyBufferCount(void);							// ���� ������ ī��Ʈ�� ���� �Ѵ�. 
	inline INT32			GetBufferCount(void);								// ����ϴ� ������ ī��Ʈ�� �����Ѵ�. 

	//Data Save 
	INT32					Push( void * pBuffer );								//������ Save 
	void*					PushPointer(void);									//������ Save Pointer		-	PushPointerIdx	<- �̰Ͱ� �ѽ��Դϴ�.
    void					PushPointerIdx(void);								//������ Save (Add Idx)		-	PushPointer		<- �̰Ͱ� �ѽ��Դϴ�.

	//Data Load 
	void*					Pop(void);											//������ Read
	void					PopIdx(void);										//������ ReadIdx 
};

#endif