#include "stdafx.h"
#include "S2RingBuffer.h"

#define RING_BUFFER_MAX_INT_COUNT			4000000000

S2RingBuffer::S2RingBuffer()
{
	m_pBuffer			= NULL; 
	m_ui32WriteIdx		= 0; 
	m_ui32ReadIdx		= 0; 
	m_ui32BufferSize	= 0; 
	m_ui32BufferCount	= 0; 

}
S2RingBuffer::~S2RingBuffer()
{
	Destroy(); 
}

BOOL S2RingBuffer::Create( S2RING_INFO* pInfo )
{
	return Create( pInfo->m_ui32BufferSize, pInfo->m_ui32BufferCount );
}

BOOL S2RingBuffer::Create( INT32 i32BufferSize, INT32 i32BufferCount )
{
	m_ui32WriteIdx		= 0; 
	m_ui32ReadIdx		= 0; 
	m_ui32BufferSize	= i32BufferSize; 
	m_ui32BufferCount	= i32BufferCount; 

	m_pBuffer = new char[ i32BufferSize * i32BufferCount ]; 
	if( NULL == m_pBuffer )	return FALSE; 

	return TRUE; 
}

void S2RingBuffer::Destroy(void)
{
	SAFE_DELETE_ARRAY( m_pBuffer );
}

INT32 S2RingBuffer::GetEmptyBufferCount(void)
{	
	return m_ui32BufferCount - GetBufferCount();
}

INT32 S2RingBuffer::GetBufferCount(void)
{
	INT32 Rv = 0; 
	if(m_ui32WriteIdx > m_ui32ReadIdx)
	{
		Rv = m_ui32WriteIdx - m_ui32ReadIdx; 
	}	
	return Rv;
}

INT32 S2RingBuffer::Push(void * pBuffer)
{
	if( m_ui32WriteIdx > (RING_BUFFER_MAX_INT_COUNT - 100) )
	{
		if(m_ui32WriteIdx == m_ui32ReadIdx)
		{
			m_ui32WriteIdx	= 0;
			m_ui32ReadIdx	= 0;
		}
	}

	if( m_ui32ReadIdx > RING_BUFFER_MAX_INT_COUNT )return RING_BUFFER_ERROR_BUFFER_FULLIDX;
	if( (m_ui32WriteIdx - m_ui32ReadIdx) >= m_ui32BufferCount )return RING_BUFFER_ERROR_BUFFER_FULL;

	INT32 iWriteIdx = m_ui32WriteIdx % m_ui32BufferCount; 

	S2Memory::Copy( &m_pBuffer[ iWriteIdx * m_ui32BufferSize ], pBuffer, m_ui32BufferSize); 
	
	m_ui32WriteIdx++;

	return RING_BUFFER_SUCCESS; 
}


void * S2RingBuffer::PushPointer(void)
{
	if( m_ui32WriteIdx > (RING_BUFFER_MAX_INT_COUNT - 100) )
	{
		if(m_ui32WriteIdx == m_ui32ReadIdx)
		{
			m_ui32WriteIdx	= 0; 
			m_ui32ReadIdx	= 0; 
		}
	}
	if( m_ui32ReadIdx > RING_BUFFER_MAX_INT_COUNT )return NULL;
	if( (m_ui32WriteIdx - m_ui32ReadIdx) >= m_ui32BufferCount )return NULL;

	INT32 iWriteIdx = m_ui32WriteIdx % m_ui32BufferCount; 

	return &m_pBuffer[iWriteIdx * m_ui32BufferSize]; 
}

void S2RingBuffer::PushPointerIdx(void)
{
	m_ui32WriteIdx++;
}

void * S2RingBuffer::Pop(void)
{
	void * pBuffer = NULL; 

	if( 0 < GetBufferCount() )
	{
		INT32 iReadIdx = m_ui32ReadIdx % m_ui32BufferCount;
		pBuffer = &m_pBuffer[ iReadIdx * m_ui32BufferSize ];
	}

	return pBuffer; 
}

void S2RingBuffer::PopIdx(void)
{
	if( GetBufferCount() > 0 ) m_ui32ReadIdx++;
}

