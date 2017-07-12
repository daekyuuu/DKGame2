#ifndef __S2_MINI_DUMP_H__
#define __S2_MINI_DUMP_H__

#include <DbgHelp.h>

class S2ExceptionHandler
{
	MINIDUMP_TYPE			m_eDumpType;
	TCHAR					m_strDmpFileName[ S2_STRING_COUNT ];

public:
	S2ExceptionHandler();
	virtual ~S2ExceptionHandler();

	void					SetPath( TCHAR* strDmpFileName );
	void					SetExceptionHandlerOption( MINIDUMP_TYPE eOption );
	void					SetCreateMiniDump( PEXCEPTION_POINTERS pExceptionInfo );
};

namespace S2MiniDump
{
	S2_EXPORT_BASE void		SetExceptionHandler( TCHAR* strDmpFileName );
	S2_EXPORT_BASE void		SetExceptionHandlerOption( MINIDUMP_TYPE eOption );
}

#endif