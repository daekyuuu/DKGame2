#include "stdafx.h"
#include "S2Error.h"

#define S2_TRACE_STRING_MAX			1024

void S2Error::Trace( const char* strFormat, ...)
{
	char strTemp[ S2_TRACE_STRING_MAX ];
	va_list marker;
	va_start( marker, strFormat);
	_vsnprintf_s( strTemp, S2_TRACE_STRING_MAX, _TRUNCATE, strFormat, marker);
	va_end( marker);
	OutputDebugStringA( strTemp );
}

void S2Error::Trace( const wchar_t* strFormat, ...)
{
	wchar_t strTemp[ S2_TRACE_STRING_MAX ];
	va_list marker;
	va_start( marker, strFormat);
	_vsnwprintf_s( strTemp, S2_TRACE_STRING_MAX, _TRUNCATE, strFormat, marker);
	va_end( marker);
	OutputDebugStringW( strTemp );
}