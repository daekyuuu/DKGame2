#ifndef __S2_H__
#define __S2_H__

#ifdef S2_EXPORTS
	#define S2_EXPORT_BASE		__declspec( dllexport)
#else
	#define S2_EXPORT_BASE		__declspec( dllimport)
#endif

// typedef
typedef unsigned char			UINT8;
typedef signed short int		INT16;
typedef unsigned short int		UINT16;
typedef __int64					INT64;
typedef unsigned __int64		UINT64;
typedef float 					REAL32;
typedef double 					REAL64;
typedef long					FIXED32;
typedef char *					PSTR;
typedef unsigned int			OBJREF;
typedef unsigned __int64		OBJREF64;
typedef unsigned short      	WORD;

#include ".\S2\S2Header.h"

#endif