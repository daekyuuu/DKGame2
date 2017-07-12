#ifndef __ROOM_MGR_H__
#define __ROOM_MGR_H__

#include "Room.h"

class CRoomMgr
{
	UINT32					m_ui32RoomCount;
	CRoom**					m_ppRoom;

public:
	CRoomMgr();
	~CRoomMgr();

	BOOL					Create( UINT32 i32RoomCount, CSessionMgr* pSessionMgr );
	void					Destroy();
	void					OnUpdate();

	T_RESULT				RoomCreate( PacketRoomCreateReq* pRecv, USER_LIST* pUser );
	T_RESULT				RoomUserEnter( PacketRoomEnterReq* pRecv, USER_LIST* pUser );
	T_RESULT				RoomUserLeave( USER_LIST* pUser );
	T_RESULT				RoomUserReady( USER_LIST* pUser );

	T_RESULT				QuickMatch( PacketRoomQuickMatchReq* pRecv, USER_LIST* pUser );
	T_RESULT				MapVote( PacketRoomMapVoteReq* pRecv, USER_LIST* pUser );

	T_RESULT				RoomBattleReady( USER_LIST* pUser );
	T_RESULT				RoomBattleEnd( USER_LIST* pUser );
	T_RESULT				RoomList( USER_LIST* pUser );
};

#endif
