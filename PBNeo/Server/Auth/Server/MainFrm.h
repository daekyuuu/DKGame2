#ifndef __MAIN_FRM_H__
#define __MAIN_FRM_H__

class CConfig;
class CSessionMgr;
class CConnectorCenter;
class CLoginMgr;

class CMainFrm
{
	CConnectorCenter*		m_pServerCenter;

	CLoginMgr*				m_pLogin;

protected:

	void					_CheckPacketBuffer();
	void					_PacketParsing( char* pPacket );

	void					_CheckLoginBuffer();

public:
	CMainFrm();
	~CMainFrm();

	BOOL					Create( CConfig* pConfig );
	void					Destroy();

	void					OnUpdate();
};

#endif
