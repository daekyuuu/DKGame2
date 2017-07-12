// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

#include "..\S2.h"
#include "S2Header.h"


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
typedef		signed char					INT8;
typedef		unsigned char				UINT8;
typedef		short int					INT16;
typedef		unsigned short int			UINT16;
typedef		signed int					INT32;
typedef		unsigned int				UINT32;
typedef		signed __int64				INT64;
typedef		unsigned __int64			UINT64;
typedef		char						CHAR8;
typedef		char16_t					CHAR16;