#include "stdafx.h"
#include "RoomContainer.h"

CRoomContainer::CRoomContainer()
{
	m_pContainer	= NULL;
	m_ppRoomList	= NULL;
}

CRoomContainer::~CRoomContainer()
{
	Destroy();
}

BOOL CRoomContainer::Create( UINT32 ui32Count )
{
	m_ui32Count = ui32Count;

	m_pContainer = new S2ContainerList;
	if( (NULL == m_pContainer) ||
		(FALSE == m_pContainer->Create( ui32Count, S2_CONTAINER_TYPE_FREE ) ) )
	{
		return FALSE;
	}

	m_ppRoomList = new ROOM_LIST*[ ui32Count ];
	if( NULL == m_ppRoomList )					return FALSE;
	S2Memory::Fill( m_ppRoomList, 0, sizeof(ROOM_LIST*)*ui32Count );
	for( INT32 i = 0 ; i < ui32Count ; i++ )
	{
		m_ppRoomList[ i ] = new ROOM_LIST;
		if( NULL == m_ppRoomList[ i ] )			return FALSE;
		m_ppRoomList[ i ]->m_ui32Idx = i;
	}

	return TRUE;
}

void CRoomContainer::Destroy()
{
	SAFE_DELETE( m_pContainer );

	if( m_ppRoomList )
	{
		for( INT32 i = 0 ; i < m_ui32Count ; i++ )
		{
			SAFE_DELETE( m_ppRoomList[ i ] );
		}
		SAFE_DELETE_ARRAY( m_ppRoomList );
	}
}

ROOM_LIST* CRoomContainer::PopEmptyUser()
{
	UINT32 ui32Idx;
	if( FALSE == m_pContainer->PopContainerList( &ui32Idx ) )	return NULL;

	return m_ppRoomList[ ui32Idx ];
}

void CRoomContainer::PushEmptyUser( ROOM_LIST* pUser )
{
	m_pContainer->PushContainerList( pUser->m_ui32Idx );
}