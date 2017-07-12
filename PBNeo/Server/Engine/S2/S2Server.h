#ifndef __S2_SERVER_H__
#define __S2_SERVER_H__

enum S2_RESULT_CODE
{
	S2_RESULT_SUCCESS,

	S2_RESULT_ERROR								= 0x80000001,
	S2_RESULT_ERROR_ST_TIMER,
	S2_RESULT_ERROR_ST_NETWORK,
	S2_RESULT_ERROR_ST_LOG,
	S2_RESULT_ERROR_ST_THREAD,
	S2_RESULT_ERROR_ST_CONFIG,
	S2_RESULT_ERROR_ST_MAINFRM,
};

enum S2_SERVER_STATE
{
	S2_SERVER_STATE_NONE		= 0,
	S2_SERVER_STATE_LOADING,
	S2_SERVER_STATE_INIT,
	S2_SERVER_STATE_START,
	S2_SERVER_STATE_END,
};

class S2_EXPORT_BASE S2Server
{
	S2_SERVER_STATE			m_eServerState;
	DATE32					m_dt32DelayTime;

	S2Thread				m_Thread;

protected:
	S2Config*				m_pConfig;

public:
	S2Server();
	virtual ~S2Server();

	virtual S2_RESULT_CODE	StartServer( TCHAR* strDumpFile, INT32 i32InitDelayTimeSec = 10 );
	void					SetInit();
	void					StopServer();
	virtual void			OnRun();

protected:	
	void					Destroy();

	virtual S2_RESULT_CODE	OnInit();
	virtual void			OnUpdate();
};

extern S2LogFile*			g_pLog;

#endif
