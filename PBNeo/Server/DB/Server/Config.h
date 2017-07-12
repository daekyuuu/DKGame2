#ifndef __CONFIG_H__
#define __CONFIG_H__

class CConfig : public S2Config
{
public:
	UINT32					m_ServerKey;
	UINT32					m_ui32IP;
	UINT16					m_ui16Port;

	INT32					m_i32IOCPThreadCount;

	UINT32					m_ui32CenterIP;
	UINT16					m_ui16CenterPort;

	INT32					m_i32DBCount;
	S2DB_CONNECT_INFO		m_DBConnectInfo;

	// Server 에서 받는 데이터
	UINT32					m_ui32Season;

protected:
	BOOL					_TempLoad();

public:
	CConfig();
	~CConfig();

	BOOL					Create( TCHAR* strConfigPath );
	void					Destroy();

	BOOL					LoadConfig();
};

extern CConfig*	g_pConfig;

#endif
