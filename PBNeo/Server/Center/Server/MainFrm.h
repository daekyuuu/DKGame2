#ifndef __MAIN_FRM_H__
#define __MAIN_FRM_H__

class CConfig;
class CUserMgr;
class CRoomMgr;

#include "SessionMgr.h"

class CMainFrm
{
	CUserMgr*				m_pUserMgr;

	CRoomMgr*				m_pRoomMgr;

	CSessionMgr*			m_pSessionMgr;
	S2NetServerContext*		m_pServerContext;	

	INT32					m_i32IOCPThreadCount;

protected:

	void					_CheckPacketBuffer();
	void					_PacketParsing( RING_IOCP_POP* pRing );

public:
	CMainFrm();
	~CMainFrm();

	BOOL					Create( CConfig* pConfig );
	void					Destroy();

	void					OnUpdate();

	void					SendNotice( S2MOPacketBase* pPacket );

protected:

	T_RESULT				_LoginReq( RING_IOCP_POP* pRing );
	T_RESULT				_LoginInputAck( RING_IOCP_POP* pRing );
	T_RESULT				_LogoutReq( RING_IOCP_POP* pRing );
	T_RESULT				_UserInfoLoadReq( RING_IOCP_POP* pRing );
	T_RESULT				_UserInfoLoadAck( RING_IOCP_POP* pRing );
	T_RESULT				_UserChangeNick( RING_IOCP_POP* pRing );
	T_RESULT				_RoomCreateReq(  RING_IOCP_POP* pRing );
	T_RESULT				_RoomEnterReq(  RING_IOCP_POP* pRing );
	T_RESULT				_RoomLeaveReq(  RING_IOCP_POP* pRing );
	T_RESULT				_RoomQuickMatch( RING_IOCP_POP* pRing );
	T_RESULT				_RoomSlotReady( RING_IOCP_POP* pRing );
	T_RESULT				_RoomMapVote( RING_IOCP_POP* pRing );
	T_RESULT				_RoomBattleReady( RING_IOCP_POP* pRing );
	T_RESULT				_RoomBattleEnd( RING_IOCP_POP* pRing );
	T_RESULT				_RoomList( RING_IOCP_POP* pRing );
};

#endif
