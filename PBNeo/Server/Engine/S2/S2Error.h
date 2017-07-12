#ifndef __S2_ERROR_H__
#define __S2_ERROR_H__

namespace S2Error
{
	void					Trace( const char* strFormat, ... );
	void					Trace( const wchar_t* strFormat, ... );
};

#ifdef _DEBUG
	#define S2ASSERT(com)		if( false == (com) ) __debugbreak();
#else
	#define S2ASSERT(com)		((void)0)
#endif

#define S2ASSERT_BUILD(com)		{ char Temp[ (com) ? 1 : 0 ]; UNREFERENCED_PARAMETER(Temp); } 
#define S2TRACE					S2Error::Trace

#endif