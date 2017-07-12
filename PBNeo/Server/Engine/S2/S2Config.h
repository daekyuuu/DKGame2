#ifndef __S2_CONFIG_H__
#define __S2_CONFIG_H__

#include "S2JsonFile.h"

class S2_EXPORT_BASE S2Config
{
	S2JsonFile*				m_pJsonFile;

public:
	S2Json*					m_pJson;

public:

	TCHAR					m_strLogFilePath[ S2_STRING_COUNT ];
	INT32					m_i32LogBufferCount;

public:
	S2Config();
	virtual ~S2Config();

	BOOL					Create(TCHAR* strConfigPath);
	void					OnDestroy();

	
};

#endif