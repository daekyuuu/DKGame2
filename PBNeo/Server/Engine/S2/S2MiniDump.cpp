#include "stdafx.h"
#include "S2MiniDump.h"

static S2ExceptionHandler	s_pExpectionOuter;
LONG WINAPI CheckUnhandledException( PEXCEPTION_POINTERS pExceptionInfo );

void S2MiniDump::SetExceptionHandler( TCHAR* strDmpFileName )
{
	s_pExpectionOuter.SetPath( strDmpFileName );

	SetUnhandledExceptionFilter( CheckUnhandledException );
}

void S2MiniDump::SetExceptionHandlerOption( MINIDUMP_TYPE eOption )
{
	s_pExpectionOuter.SetExceptionHandlerOption( eOption );
}

LONG WINAPI CheckUnhandledException( PEXCEPTION_POINTERS pExceptionInfo )
{
	SetErrorMode( SEM_NOGPFAULTERRORBOX );
	s_pExpectionOuter.SetCreateMiniDump( pExceptionInfo );
	return 0;
}

S2ExceptionHandler::S2ExceptionHandler()
{
	m_eDumpType = MiniDumpNormal;
	S2String::Copy( m_strDmpFileName, _T("S2BugReport.i3b"), S2String::Length(_T("S2BugReport.i3b"))+1 );
}

S2ExceptionHandler::~S2ExceptionHandler()
{
}

void	S2ExceptionHandler::SetPath( TCHAR* strDmpFileName )
{
	if( NULL == strDmpFileName ) return;

	S2String::Copy( m_strDmpFileName, strDmpFileName, S2String::Length(strDmpFileName)+1 );
}

void	S2ExceptionHandler::SetExceptionHandlerOption( MINIDUMP_TYPE eOption )
{
	m_eDumpType = eOption;
}

void	S2ExceptionHandler::SetCreateMiniDump( PEXCEPTION_POINTERS pExceptionInfo )
{
	// miniDump ½ÇÇà
	MINIDUMP_EXCEPTION_INFORMATION sExceptionInfo;

	sExceptionInfo.ThreadId				= GetCurrentThreadId();
	sExceptionInfo.ExceptionPointers	= pExceptionInfo;
	sExceptionInfo.ClientPointers		= FALSE;

	TCHAR strFileName[ S2_STRING_COUNT ];

	SYSTEMTIME st; 
    GetLocalTime(&st); 

	S2String::Format( strFileName, S2_STRING_COUNT, _T("%s_%04d%02d%02d%02d%02d"), m_strDmpFileName, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute );

	HANDLE hFile = ::CreateFile( strFileName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, m_eDumpType, &sExceptionInfo, NULL, NULL);
	
	CloseHandle( hFile );
}