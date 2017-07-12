#ifndef __MAIN_FRM_H__
#define __MAIN_FRM_H__

class CConfig;
class CSessionMgr;
class CConnectorCenter;
class CBattleMgr;

class CMainFrm
{
	CSessionMgr*			m_pSessionMgr;
	S2NetServerContext*		m_pServerContext;

	CConnectorCenter*		m_pServerCenter;

	CBattleMgr*				m_pBattleMgr;

	INT32					m_i32IOCPThreadCount;

protected:

	void					_CheckPacketBuffer();
	void					_PacketParsing( char* pPacket );

public:
	CMainFrm();
	~CMainFrm();

	BOOL					Create( CConfig* pConfig );
	void					Destroy();

	void					OnUpdate();

	BOOL					StartServer( TCHAR* pMapName );
};

#endif
