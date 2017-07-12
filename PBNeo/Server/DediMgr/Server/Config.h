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

	UINT32					m_ui32RoomCount;
	LOAD_MAP_INFO			m_MapInfo;

public:
	CConfig();
	~CConfig();

	BOOL					Create( TCHAR* strConfigPath );
	void					Destroy();

	BOOL					LoadConfig();
};

#endif
