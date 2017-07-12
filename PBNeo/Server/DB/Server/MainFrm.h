#ifndef __MAIN_FRM_H__
#define __MAIN_FRM_H__

class CConfig;
class CSessionMgr;
class CConnectorCenter;
class CDatabaseMgr;

class CMainFrm
{
	CConnectorCenter*		m_pServerCenter;

	CDatabaseMgr*			m_pDB;

protected:

	void					_CheckPacketBuffer();
	void					_CheckDBBuffer();

	void					_PacketParsing( char* pPacket );

	void					_GetUserInfo( RING_DB_POP* pPop );

public:
	CMainFrm();
	~CMainFrm();

	BOOL					Create( CConfig* pConfig );
	void					Destroy();

	void					OnUpdate();
};

#endif
