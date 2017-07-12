#ifndef __ROOM_MGR_H__
#define __ROOM_MGR_H__

#include "ConnectorCenter.h"
#include "RoomContainer.h"

struct BATTLE_INFO
{
	UINT32					m_ui32RoomIdx;

	UINT16					m_ui16Port;
};

struct BATTLE_GROUP
{
	T_MapID					m_TMapID;
	S2ListBin*				m_pList;
};

class CBattleMgr
{
	S2ListBin*				m_pMapList;
	CRoomContainer*			m_pRoomContainer;

	CConnectorCenter*		m_pCenter;

protected:
	BOOL					_StartServer( T_MapID TMapID );

public:
	CBattleMgr();
	~CBattleMgr();

	BOOL					Create( UINT32 ui32RoomCount, LOAD_MAP_INFO* pMapInfo, CConnectorCenter* pServer );
	void					Destroy();

	BOOL					StartBattle( PacketDediStartReq* pPacket );
	BOOL					EndBattle();
	BOOL					LoadingComplete();
};

#endif