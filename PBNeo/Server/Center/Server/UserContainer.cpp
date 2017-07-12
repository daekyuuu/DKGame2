#include "stdafx.h"
#include "UserContainer.h"

CUserContainer::CUserContainer()
{
	m_pContainer	= NULL;
	m_ppUserList	= NULL;
}

CUserContainer::~CUserContainer()
{
	Destroy();
}

BOOL CUserContainer::Create( UINT32 ui32Count )
{
	m_ui32Count = ui32Count;

	m_pContainer = new S2ContainerList;
	if( (NULL == m_pContainer) ||
		(FALSE == m_pContainer->Create( ui32Count, S2_CONTAINER_TYPE_FREE ) ) )
	{
		return FALSE;
	}

	m_ppUserList = new USER_LIST*[ ui32Count ];
	if( NULL == m_ppUserList )					return FALSE;
	S2Memory::Fill( m_ppUserList, 0, sizeof(USER_LIST*)*ui32Count );
	for( INT32 i = 0 ; i < ui32Count ; i++ )
	{
		m_ppUserList[ i ] = new USER_LIST;
		if( NULL == m_ppUserList[ i ] )			return FALSE;
		m_ppUserList[ i ]->m_ui32Idx = i;
	}

	return TRUE;
}

void CUserContainer::Destroy()
{
	SAFE_DELETE( m_pContainer );

	if( m_ppUserList )
	{
		for( INT32 i = 0 ; i < m_ui32Count ; i++ )
		{
			SAFE_DELETE( m_ppUserList[ i ] );
		}
		SAFE_DELETE_ARRAY( m_ppUserList );
	}
}

USER_LIST* CUserContainer::PopEmptyUser()
{
	UINT32 ui32Idx;
	if( FALSE == m_pContainer->PopContainerList( &ui32Idx ) )	return NULL;

	return m_ppUserList[ ui32Idx ];
}

void CUserContainer::PushEmptyUser( USER_LIST* pUser )
{
	m_pContainer->PushContainerList( pUser->m_ui32Idx );
}