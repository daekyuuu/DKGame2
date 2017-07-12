#ifndef __S2_LOG_FILE_W_H__
#define __S2_LOG_FILE_W_H__

#include "S2LogFileBase.h"

class S2_EXPORT_BASE S2LogFileW : public S2LogFileBase
{
	wchar_t					m_wstrFileName[ LOG_STRING_MAX ]; 
	wchar_t					m_astrLogInit[ LOG_BUFFER_MAX_COUNTER ][ LOG_INIT_STRING ]; 
	wchar_t					m_wstrStartText[ LOG_STRING_MAX ];

protected:
	BOOL					_CreateFile(void);
	void					_WriteLogFile( wchar_t * strLog );

	virtual BOOL			_Working();

	void					_WriteLog( wchar_t* strLog );

public: 	
	
	S2LogFileW();
	virtual ~S2LogFileW(void);

	BOOL					Create( wchar_t* wstrFileName, INT32 iSaveType = FILE_SAVE_INTERVAL_HOUR, INT32 iLogBufferCount = 1000 );
	void					Destroy(void);
	INT32					AddLogThread( wchar_t* strPreString, INT32 iType, DWORD dwThreadID );
	void					WriteLog( S2_LOG_TYPE eLogType, wchar_t * strLogString, ... );
	void					WriteLog( INT32 i32WarningType, INT32 i32WarningLevel, wchar_t * strLogString, ... );
};

#endif
