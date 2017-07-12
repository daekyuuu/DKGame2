#ifndef __CONFIG_H__
#define __CONFIG_H__

class CConfig : public S2Config
{
public:
	UINT32					m_ServerKey;
	UINT32					m_ui32IP;
	UINT16					m_ui16Port;

	UINT32					m_ui32CenterIP;
	UINT16					m_ui16CenterPort;

	INT32					m_i32DBCount;
	S2DB_CONNECT_INFO		m_DBConnectInfo;

public:
	CConfig();
	~CConfig();

	BOOL					Create( TCHAR* strConfigPath );
	void					Destroy();

	BOOL					LoadConfig();
};

#endif
