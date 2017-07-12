#include "stdafx.h"
#include "S2LogFileW.h"
#include "S2String.h"

struct LOG_FILE_BUFFER
{		
	wchar_t	_strMessage[ LOG_STRING_MAX ];
};

S2LogFileW::S2LogFileW()
{
	m_wstrFileName[0]	= '\0'; 
	m_wstrStartText[0]	= '\0';
}

S2LogFileW::~S2LogFileW(void)
{
	Destroy(); 
}

BOOL S2LogFileW::Create( wchar_t* wstrFileName, INT32 iSaveType, INT32 iLogBufferCount )
{
	//Set init Value
	S2Memory::FillZero( m_ppLogBuffer, sizeof(m_ppLogBuffer) );
		
	S2String::Copy( m_wstrFileName, wstrFileName, LOG_STRING_MAX );
	
	_UpdateTime();

	wchar_t wstrData[ MAX_PATH ];
	MultiByteToWideChar( CP_ACP, 0, __DATE__, -1, wstrData, MAX_PATH);

	wchar_t wstrTime[ MAX_PATH ];
	MultiByteToWideChar( CP_ACP, 0, __TIME__, -1, wstrTime, MAX_PATH);
			
	S2String::Format( m_wstrStartText, LOG_STRING_MAX, L"-- COMPILE DATE : Ver:%d.%d.%d.%d, BuildDate:%s %s, StartDate:%04d-%02d-%02d %02d:%02d --\r\n", 
		m_ui16Ver01, m_ui16Ver02, m_ui32Ver03, m_ui32Ver04, wstrData, wstrTime, m_iYear, m_iMonth, m_iDay, m_iHour, m_iMinute );

	//Create File 
	if( false == _CreateFile() )								return FALSE; 

	return S2LogFileBase::_Create( iSaveType, iLogBufferCount );
}

void S2LogFileW::Destroy(void)
{
}

void S2LogFileW::_WriteLogFile( wchar_t * wstrLog )
{
	INT32 i32Length = (INT32)(sizeof(wchar_t)*S2String::Length( wstrLog ));
	::WriteFile( m_hFile, wstrLog, i32Length, (LPDWORD)&i32Length, NULL ); 
	S2TRACE( wstrLog );
}

INT32 S2LogFileW::AddLogThread( wchar_t* strPreString, INT32 iType, DWORD dwThreadID )
{	iType;

	INT32 i32Rv = RV_ADD_FAIL; 

	m_CS.Enter(); 
	{
		i32Rv = _FindWorkIdx( dwThreadID );

		if( RV_ADD_FAIL != i32Rv )
		{
			m_CS.Leave();
			return i32Rv;
		}

		if( LOG_BUFFER_MAX_COUNTER <= m_ui32WorkCounter )
		{	// 더이상 로그 파일을 생성 할 수 없습니다.
			m_CS.Leave();
			return RV_ADD_FAIL;
		}

		m_ppLogBuffer[ m_ui32WorkCounter  ] = new S2RingBuffer;
		if( NULL == m_ppLogBuffer[ m_ui32WorkCounter ] )
		{	// 메모리 할당에 실패 했습니다.
			m_CS.Leave();
			return RV_ADD_FAIL;
		}

		if( false == m_ppLogBuffer[ m_ui32WorkCounter ]->Create( sizeof(LOG_FILE_BUFFER), m_iBufferCount ) )
		{	// Ring Buffer 메모리 할당 실패
			delete m_ppLogBuffer[ m_ui32WorkCounter  ];
			m_CS.Leave();
			return RV_ADD_FAIL;
		}
		
		S2String::Copy( m_astrLogInit[ m_ui32WorkCounter ], strPreString, LOG_INIT_STRING );
		i32Rv = m_ui32WorkCounter;

		m_adwThreadID[m_ui32WorkCounter] = dwThreadID;

		m_ui32WorkCounter++;
	}
	m_CS.Leave(); 

	return i32Rv; 
}

BOOL S2LogFileW::_Working()
{
	INT32 iLogCount; 
	LOG_FILE_BUFFER * pLogBuffer;	
	BOOL bWorking = FALSE; 

	//Write Log 
	for(UINT32 j = 0; j < m_ui32WorkCounter; j++)
	{
		iLogCount = m_ppLogBuffer[j]->GetBufferCount(); 
		for(INT32 i = 0; i < iLogCount; i++)
		{
			pLogBuffer = (LOG_FILE_BUFFER *) m_ppLogBuffer[j]->Pop();
			_WriteLogFile( pLogBuffer->_strMessage ); 
			m_ppLogBuffer[j]->PopIdx();
			bWorking = TRUE; 
		}
	}
	return bWorking;
}

void S2LogFileW::WriteLog( S2_LOG_TYPE eLogType, wchar_t * wstrLogString, ... )
{
	wchar_t wstrTemp[ LOG_STRING_MAX ];

	va_list marker;
	va_start( marker, wstrLogString );
	_vsnwprintf_s( wstrTemp, LOG_STRING_MAX, wstrLogString, marker );
	va_end( marker);

	_WriteLog( wstrTemp );
}

void S2LogFileW::WriteLog( INT32 i32WarningType, INT32 i32WarningLevel, wchar_t * wstrLogString, ... )
{
	wchar_t wstrTemp[ LOG_STRING_MAX ];

	va_list marker;
	va_start( marker, wstrLogString );
	_vsnwprintf_s( wstrTemp, LOG_STRING_MAX, wstrLogString, marker );
	va_end( marker);

	_WriteLog( wstrTemp );
}

void S2LogFileW::_WriteLog( wchar_t* wstrLog )
{
	INT32 i32WorkIdx = _FindWorkIdx();

	if( RV_ADD_FAIL == i32WorkIdx )
	{	
		#ifdef I3_DEBUG
		I3FATAL("Invalid Thread ID : %d\n", GetCurrentThreadId());
		#endif
		return;
	}

	//Make PreString
	S2RingBuffer*		pRingBuffer = m_ppLogBuffer[i32WorkIdx]; 
	LOG_FILE_BUFFER*	pLogBuffer	= (LOG_FILE_BUFFER*)pRingBuffer->PushPointer();

	if( pLogBuffer != NULL)
	{
		S2String::Format( pLogBuffer->_strMessage, LOG_STRING_MAX, L"[%02d:%02d:%02d][%s]%s\r\n", m_iHour, m_iMinute, m_iSecond, m_astrLogInit[ i32WorkIdx ], wstrLog );
		pRingBuffer->PushPointerIdx();
	}
}

BOOL S2LogFileW::_CreateFile(void)
{
	BOOL Rv = TRUE; 
	wchar_t wstrFileName[ MAX_PATH ]; 
	S2String::Format( wstrFileName, MAX_PATH, L"%s_%02d_%02d_%02d_%02d.txt", m_wstrFileName, m_iYear, m_iMonth, m_iDay, m_iHour);

	//Close Old File 
	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
	}

	// Create New File
	m_hFile = ::CreateFileW( wstrFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if(m_hFile == INVALID_HANDLE_VALUE)
	{			
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
		Rv = FALSE; 
	}
	else
	{
		SetFilePointer( m_hFile, 0, NULL, FILE_END);

		m_ui32FileHour	= m_iHour;
		m_ui32FileDay	= m_iDay;

		_WriteLogFile( m_wstrStartText ); 
	}

	return Rv;
}
