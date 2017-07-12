#ifndef __S2_LOG_FILE_A_H__
#define __S2_LOG_FILE_A_H__

#include "S2LogFileBase.h"

class S2_EXPORT_BASE S2LogFileA : public S2LogFileBase
{
	char					m_strFileName[ LOG_STRING_MAX ]; 
	char					m_astrLogInit[ LOG_BUFFER_MAX_COUNTER ][ LOG_INIT_STRING ]; 
	char					m_strStartText[ LOG_STRING_MAX ];

protected:
	BOOL					_CreateFile(void);
	void					_WriteLogFile( char * strLog );

	virtual BOOL			_Working();

	void					_WriteLog( char* strLog );

public: 	
	
	S2LogFileA();
	virtual ~S2LogFileA(void);

	BOOL					Create( char* wstrFileName, INT32 iSaveType = FILE_SAVE_INTERVAL_HOUR, INT32 iLogBufferCount = 1000 );
	void					Destroy(void);
	INT32					AddLogThread( char* strPreString, INT32 iType, DWORD dwThreadID );
	void					WriteLog( S2_LOG_TYPE eLogType, char * strLogString, ... );
	void					WriteLog( INT32 i32WarningType, INT32 i32WarningLevel, char * strLogString, ... );
};

#endif
