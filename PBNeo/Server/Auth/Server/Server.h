#ifndef __SERVER_H__
#define __SERVER_H__

class CMainFrm;

class CServer : public S2Server
{
	CMainFrm*				m_pMainFrm;

public:
	CServer();
	~CServer();

	virtual S2_RESULT_CODE	StartServer( TCHAR* strDumpFile );
	virtual S2_RESULT_CODE	OnInit();
	virtual void			OnUpdate();
	virtual void			Destroy();
};

#endif