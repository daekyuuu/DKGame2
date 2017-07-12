#ifndef __S2_LOG_FILE_BASE_H__
#define __S2_LOG_FILE_BASE_H__

#include "S2CriticalSection.h"

#ifdef UNICODE
#define S2LogFile S2LogFileW
#else
#define S2LogFile S2LogFileA
#endif

#define LOG_BUFFER_MAX_COUNTER		100
#define LOG_INIT_STRING				10
#define LOG_STRING_MAX				1024
#define RV_ADD_FAIL					0x80000001
#define LOG_START_DATE				32

enum FILE_SAVE_INTERVAL
{
	FILE_SAVE_INTERVAL_DAY =1,
	FILE_SAVE_INTERVAL_HOUR,
};

enum S2_LOG_TYPE
{
	S2_LOG_TYPE_SUCCESS,
	S2_LOG_TYPE_ERROR,
	S2_LOG_TYPE_WARNING,
};

class S2_EXPORT_BASE S2LogFileBase
{
protected:
	//For Thread 
	S2Thread				m_Thread;

	HANDLE					m_hFile;

	//For File
	INT32					m_iSaveType; 
	UINT32					m_ui32FileHour; 
	UINT32					m_ui32FileDay; 

	//For File Save Buffer 
	S2RingBuffer	*		m_ppLogBuffer[ LOG_BUFFER_MAX_COUNTER ];
	UINT32					m_ui32WorkCounter; 
	INT32					m_iBufferCount; 

	DWORD					m_adwThreadID[LOG_BUFFER_MAX_COUNTER];

	//For Add Log Buffer 
	S2CriticalSection		m_CS; 

	//For Save Time
	SYSTEMTIME				m_SystemTime;
	UINT32					m_iYear;
	UINT32					m_iMonth;
	UINT32					m_iDay;
	UINT32					m_iHour;
	UINT32					m_iMinute;
	UINT32					m_iSecond;

	// Server Version
	UINT16					m_ui16Ver01;
	UINT16					m_ui16Ver02;
	UINT32					m_ui32Ver03;
	UINT32					m_ui32Ver04;

protected:	

	BOOL					_Create( INT32 iSaveType, INT32 iLogBufferCount );	
	void					_UpdateTime(void)
	{
		::GetLocalTime( &m_SystemTime );
		m_iYear		= m_SystemTime.wYear; 
		m_iMonth	= m_SystemTime.wMonth; 
		m_iDay		= m_SystemTime.wDay; 	
		m_iHour		= m_SystemTime.wHour; 
		m_iMinute	= m_SystemTime.wMinute; 
		m_iSecond	= m_SystemTime.wSecond;
		return; 
	}

	INT32					_FindWorkIdx();
	INT32					_FindWorkIdx( DWORD dwThreadID );

	virtual BOOL			_CreateFile(void);
	virtual BOOL			_Working();

public: 	
	
	S2LogFileBase();
	virtual ~S2LogFileBase(void);

	virtual BOOL			Create( TCHAR* wstrFileName, INT32 iSaveType = FILE_SAVE_INTERVAL_HOUR, INT32 iLogBufferCount = 1000 ) = NULL;
	void					Destroy(void);
	void					SetVersion( UINT16 ui16Ver01, UINT16 ui16Ver02, UINT32 ui32Ver03, UINT32 ui32Ver04 );
	void					OnUpdate(void);
};

#endif
