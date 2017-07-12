#include "stdafx.h"
#include "RoomMgr.h"

static INT32 _CompareProc( void* p1, void* p2 )
{
	UINT32	ui32RoomIdx1	= ((BATTLE_INFO*)p1)->m_ui32RoomIdx;
	UINT32	ui32RoomIdx2	= ((BATTLE_INFO*)p2)->m_ui32RoomIdx;

	if( ui32RoomIdx1 > ui32RoomIdx2 )			return 1;
	else if( ui32RoomIdx1 < ui32RoomIdx2 )		return -1;

	return 0;
}

CBattleMgr::CBattleMgr()
{
	m_pMapList	= NULL;
}

CBattleMgr::~CBattleMgr()
{
	Destroy();
}

BOOL CBattleMgr::Create( UINT32 ui32RoomCount, LOAD_MAP_INFO* pMapInfo, CConnectorCenter* pServer )
{
	m_pCenter = pServer;

	m_pRoomContainer = new CRoomContainer;
	if( (NULL == m_pRoomContainer ) ||
		( FALSE == m_pRoomContainer->Create( ui32RoomCount ) ) )							return FALSE;

	m_pMapList = new S2ListBin;
	if( NULL == m_pMapList )	return FALSE;

	BATTLE_GROUP* pGroup;
	for( INT32 i = 0 ; i < pMapInfo->m_i32MapCount ; i++ )
	{
		pGroup = new BATTLE_GROUP;
		if( NULL == pGroup )						return FALSE;
		pGroup->m_TMapID		= pMapInfo->m_pMapID[i];
		pGroup->m_pList			= new S2ListBin;
		if( NULL == pGroup->m_pList )				return FALSE;
		pGroup->m_pList->SetCompareProc( _CompareProc );
		if( -1 == m_pMapList->Add( pGroup ) )		return FALSE;
	}
	return TRUE;
}

void CBattleMgr::Destroy()
{
	BATTLE_GROUP* pGroup;
	INT32 i32Count = m_pMapList->GetCount() - 1;
	for( INT32 i = i32Count ; i >= 0 ; i-- )
	{
		pGroup = (BATTLE_GROUP*)m_pMapList->GetItem( i );
		SAFE_DELETE( pGroup->m_pList );
		SAFE_DELETE( pGroup );
	}
	SAFE_DELETE_ARRAY( m_pMapList );
}

BOOL CBattleMgr::StartBattle( PacketDediStartReq* pPacket )
{
	BATTLE_GROUP* pGroup = NULL;
	for( INT32 i = 0 ; i < m_pMapList->GetCount() ; i++ )
	{
		pGroup = (BATTLE_GROUP*)m_pMapList->GetItem( i );
		if( pGroup->m_TMapID != pPacket->m_TMapID ) continue;

		break;
	}
	if( NULL == pGroup )
	{
		pGroup = new BATTLE_GROUP;
		if( NULL == pGroup ) return FALSE;

		pGroup->m_TMapID		= pPacket->m_TMapID;
		pGroup->m_pList			= new S2ListBin;
		pGroup->m_pList->SetCompareProc( _CompareProc );
		if( -1 == m_pMapList->Add( pGroup ) )		return FALSE;
	}

	BATTLE_INFO RoomInfo;
	RoomInfo.m_ui32RoomIdx = pPacket->m_ui32RoomIdx;

	INT32 i32Idx = pGroup->m_pList->FindItem( &RoomInfo );
	if( -1 != i32Idx )
	{
		g_pLog->WriteLog( S2_LOG_TYPE_ERROR, _T("StartBattle FindItem Error RoomIdx = %d"), i32Idx );
		return FALSE;
	}

	ROOM_LIST* pRoom = m_pRoomContainer->PopEmptyUser();
	if( NULL == pRoom )
	{
		g_pLog->WriteLog( S2_LOG_TYPE_ERROR, _T("StartBattle PopEmptyUser NULL") );
		return FALSE;
	}
	pRoom->m_ui32RoomIdx	= pPacket->m_ui32RoomIdx;
	pRoom->m_TMapID			= pPacket->m_TMapID;
	if( -1 == pGroup->m_pList->Add( pRoom ) )
	{
		g_pLog->WriteLog( S2_LOG_TYPE_ERROR, _T("StartBattle RoomAdd Fail") );
		return FALSE;
	}

	if( FALSE == _StartServer( pRoom->m_TMapID ) )
	{
		g_pLog->WriteLog( S2_LOG_TYPE_ERROR, _T("[ERROR]StartBattle Start Server Fail") );
		return FALSE;
	}

	return TRUE;
}

BOOL CBattleMgr::EndBattle()
{
	return TRUE;
}

BOOL CBattleMgr::LoadingComplete()
{
	return TRUE;
}

BOOL CBattleMgr::_StartServer( T_MapID TMapID )
{
	// TCHAR* pMapName
	// initialize window flags
	UINT32 dwFlags = 0;
	UINT16 ui16ShowWindowFlags = SW_HIDE;

	// initialize startup info
	STARTUPINFO StartupInfo = {
		sizeof(STARTUPINFO),
		NULL, NULL, NULL,
		(::DWORD)CW_USEDEFAULT,
		(::DWORD)CW_USEDEFAULT,
		(::DWORD)CW_USEDEFAULT,
		(::DWORD)CW_USEDEFAULT,
		(::DWORD)0, (::DWORD)0, (::DWORD)0,
		(::DWORD)dwFlags,
		ui16ShowWindowFlags,
		0, NULL,
		HANDLE(nullptr),
		HANDLE(nullptr),
		HANDLE(nullptr)
	};

	// ProcessInfo
	PROCESS_INFORMATION ProcessInfo;
	ZeroMemory( &ProcessInfo, sizeof(ProcessInfo) );

	TCHAR SpawnProcessCommand[ MAX_PATH ] = _T( "PBNeoServer.exe ?game=BOMB ?SpawnedByGameServerManager -log" );

	// Start the child process. 
	BOOL bResult = CreateProcess( NULL, SpawnProcessCommand, NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcessInfo );

	// We will not communicate with the spawned process via its process/thread handle
	CloseHandle( ProcessInfo.hProcess );
	CloseHandle( ProcessInfo.hThread );

	return bResult;
}