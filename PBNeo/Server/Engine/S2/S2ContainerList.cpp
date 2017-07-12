#include "stdafx.h"
#include "S2ContainerList.h"
#include "S2Memory.h"

S2ContainerList::S2ContainerList()
{
	m_eState		= S2_CONTAINER_STATE_NONE;
	m_ui8Buffer		= NULL;
	m_ui16Buffer	= NULL;
	m_ui32Buffer	= NULL;
}

S2ContainerList::~S2ContainerList()
{
	Destroy();
}

BOOL S2ContainerList::_EmptyBuffer()
{
	// 각 버퍼는 자리수보다 1 작은 값까지 커버 할 수 있습니다.
	if( 0xFF > m_ui32Count )
	{
		m_ui32StartIdx		= 0xFF;
	}
	else if( 0xFFFF > m_ui32Count )
	{
		m_ui32StartIdx		= 0xFFFF;
	}
	else if( 0xFFFFFFFF > m_ui32Count )
	{
		m_ui32StartIdx		= 0xFFFFFFFF;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL S2ContainerList::_FillBuffer()
{
	m_ui32StartIdx			= 0;
	
	// 각 버퍼는 자리수보다 1 작은 값까지 커버 할 수 있습니다.
	if( 0xFF > m_ui32Count )
	{
		for( UINT32 i = 0 ; i < m_ui32Count-1 ; i++ )
		{
			m_ui8Buffer[i] = (UINT8)(i+1);
		}
		m_ui8Buffer[ m_ui32Count-1 ] = 0xFF;
	}
	else if( 0xFFFF > m_ui32Count )
	{
		for( UINT32 i = 0 ; i < m_ui32Count-1 ; i++ )
		{
			m_ui16Buffer[i] = (UINT16)(i+1);
		}
		m_ui16Buffer[ m_ui32Count-1 ] = 0xFFFF;
	}
	else if( 0xFFFFFFFF > m_ui32Count )
	{
		for( UINT32 i = 0 ; i < m_ui32Count-1 ; i++ )
		{
			m_ui32Buffer[i] = i+1;
		}
		m_ui32Buffer[ m_ui32Count-1 ] = 0xFFFFFFFF;
	}
	else
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL S2ContainerList::Create( UINT32 ui32Count, S2_CONTAINER_TYPE eType )
{
	m_ui32Count				= ui32Count;
	m_eType					= eType;

	if( 0xFF > m_ui32Count )
	{
		m_eState	= S2_CONTAINER_STATE_8;
		
		m_ui8Buffer = new UINT8[m_ui32Count];
		if( NULL == m_ui8Buffer )						return FALSE;
	}
	else if( 0xFFFF > m_ui32Count )
	{
		m_eState	= S2_CONTAINER_STATE_16;

		m_ui16Buffer = new UINT16[m_ui32Count];
		if( NULL == m_ui16Buffer )						return FALSE;
	}
	else if( 0xFFFFFFFF > m_ui32Count )
	{
		m_eState	= S2_CONTAINER_STATE_32;

		m_ui32Buffer = new UINT32[m_ui32Count];
		if( NULL == m_ui32Buffer )						return FALSE;
	}
	else
	{
		return FALSE;
	}

	return ResetBuffer();
}

void S2ContainerList::Destroy()
{
	SAFE_DELETE_ARRAY( m_ui8Buffer );
	SAFE_DELETE_ARRAY( m_ui16Buffer );
	SAFE_DELETE_ARRAY( m_ui32Buffer );
}

BOOL S2ContainerList::ResetBuffer()
{
	switch( m_eType )
	{
	case S2_CONTAINER_TYPE_USE:
		{
			return _EmptyBuffer();
		}
		break;
	case S2_CONTAINER_TYPE_FREE:
		{
			return _FillBuffer();
		}
		break;
	}
	return FALSE;
}

BOOL S2ContainerList::PopContainerList( UINT32* pui32Idx )
{
	switch( m_eState )
	{
	case S2_CONTAINER_STATE_8:
		{
			if( 0xFF == m_ui32StartIdx )		return FALSE;

			*pui32Idx		= m_ui32StartIdx;
			m_ui32StartIdx	= m_ui8Buffer[ m_ui32StartIdx ];
		}
		break;
	case S2_CONTAINER_STATE_16:
		{
			if( 0xFFFF == m_ui32StartIdx )		return FALSE;

			*pui32Idx		= m_ui32StartIdx;
			m_ui32StartIdx	= m_ui16Buffer[ m_ui32StartIdx ];
		}
		break;
	case S2_CONTAINER_STATE_32:
		{
			if( 0xFFFFFFFF == m_ui32StartIdx )	return FALSE;

			*pui32Idx		= m_ui32StartIdx;
			m_ui32StartIdx	= m_ui32Buffer[ m_ui32StartIdx ];
		}
		break;
	default:
		{
			return FALSE;
		}
		break;
	}
	
	return TRUE;
}

void S2ContainerList::PushContainerList( UINT32 ui32Idx )
{
	switch( m_eState )
	{
	case S2_CONTAINER_STATE_8:
		{
			m_ui8Buffer[ ui32Idx ]	= (UINT8)m_ui32StartIdx;
			m_ui32StartIdx			= ui32Idx;
		}
		break;
	case S2_CONTAINER_STATE_16:
		{
			m_ui16Buffer[ ui32Idx ]	= (UINT16)m_ui32StartIdx;
			m_ui32StartIdx			= ui32Idx;
		}
		break;
	case S2_CONTAINER_STATE_32:
		{
			m_ui32Buffer[ ui32Idx ]	= m_ui32StartIdx;
			m_ui32StartIdx			= ui32Idx;
		}
		break;
	}
}