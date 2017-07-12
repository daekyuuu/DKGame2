#ifndef __CONFIG_H__
#define __CONFIG_H__

class CConfig : public S2Config
{
public:
	// Config File ���� �д� ������
	UINT32					m_ui32IP;
	UINT16					m_ui16Port;

	INT32					m_i32IOCPThreadCount;

	UINT32					m_ui32UserCount;

	UINT32					m_ui32RoomCount;

	// Server ���� �޴� ������
	LOAD_MAP_INFO*			m_pMapInfo;

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
