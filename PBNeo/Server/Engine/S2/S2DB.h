#ifndef __S2_DB_H__
#define __S2_DB_H__

#include <ATLComTime.h>

enum S2DB_DATA_TYPE
{
	S2DB_DATA_TYPE_BIT,
	S2DB_DATA_TYPE_TINYINT,
	S2DB_DATA_TYPE_SMALLINT,
	S2DB_DATA_TYPE_INT,
	S2DB_DATA_TYPE_BIGINT,
	S2DB_DATA_TYPE_CHAR,
	S2DB_DATA_TYPE_BINARY,
	S2DB_DATA_TYPE_SMALLDATETIME,
	S2DB_DATA_TYPE_DATETIME,
	S2DB_DATA_TYPE_REAL,
	S2DB_DATA_TYPE_END,
};

#define S2_STRCPY( buffer, val ) \
{\
	*buffer = '\'';\
	buffer++;\
	while( *val )\
	{\
		if( '\'' == *val )\
		{\
			*buffer = '\'';\
			buffer++;\
		}\
		*buffer = *val;\
		buffer++;\
		val++;\
	}\
	*buffer = '\'';\
	buffer++;\
}

#define S2_ITOA( buffer, val ) \
{\
	pPtr = strNum;\
	INT32 i32Count = 0;\
	BOOL bSign = FALSE;\
	if( 0 > val )\
	{\
		val		= ~val + 1;\
		bSign	= TRUE;\
	}\
	do\
	{\
		*pPtr	= (val%10)+'0';\
		val		/= 10;\
		pPtr++;\
		i32Count++;\
	}\
	while( val );\
	if( bSign )\
	{\
		*pPtr	= '-';\
		pPtr++;\
		i32Count++;\
	}\
	for( INT32 i = 0 ; i < i32Count ; i++ )\
	{\
		pPtr--;\
		*buffer	= *pPtr;\
		buffer++;\
	}\
}

#define S2_FCVT( buffer, val ) \
{\
	pPtr = val;\
	while( *pPtr !='\0' )\
	{\
		*buffer = *pPtr;\
		buffer++;\
		pPtr++;\
	}\
}

namespace S2DB
{
	inline UINT32 ConvertSmallDateToUINT32( DATE* pdateData )
	{
		COleDateTime oleDate( *pdateData );
		INT32 i32Year	= oleDate.GetYear();
		INT32 i32Month	= oleDate.GetMonth();
		INT32 i32Day	= oleDate.GetDay();
		INT32 i32Hour	= oleDate.GetHour();
		INT32 i32Minute	= oleDate.GetMinute();
		if( i32Year < 2000 )			return 0;

		return ((i32Year - 2000) * 100000000) + (i32Month * 1000000) + (i32Day * 10000) + (i32Hour* 100) + i32Minute;
	};

	inline UINT64 ConvertSmallDateToUINT64( DATE* pdateData )
	{
		COleDateTime oleDate( *pdateData );
		INT64 iYear		= oleDate.GetYear();
		INT64 iMonth	= oleDate.GetMonth();
		INT64 iDay		= oleDate.GetDay();
		INT64 iHour		= oleDate.GetHour();
		INT64 iMinute	= oleDate.GetMinute();
		INT64 iSecond	= oleDate.GetSecond();	

		if( iYear < 2000 )
		{
			return 0;
		}
		return ((iYear - 2000) * 10000000000) + (iMonth * 100000000) + (iDay * 1000000) + (iHour* 10000) + (iMinute* 100) + iSecond;
	};

	inline UINT64 ConvertDateToUINT64( DATE* pdateData )
	{
		COleDateTime oleDate( *pdateData );
		INT64 iYear		= oleDate.GetYear();
		INT64 iMonth	= oleDate.GetMonth();
		INT64 iDay		= oleDate.GetDay();
		INT64 iHour		= oleDate.GetHour();
		INT64 iMinute	= oleDate.GetMinute();
		INT64 iSecond	= oleDate.GetSecond();	

		if( iYear < 2000 )
		{
			return 0;
		}
		return ((iYear - 2000) * 10000000000) + (iMonth * 100000000) + (iDay * 1000000) + (iHour* 10000) + (iMinute* 100) + iSecond;
	};

	inline void ConvertString( TCHAR* strDest, const TCHAR* strSrc, INT32 i32Size )
	{
		// DB 에서 ' 는 '' 로 입력해야 합니다.
		INT32 i32OutSize = 0;
		while( L'\0' != *strSrc )
		{
			if( L'\'' == *strSrc )	// "'"
			{
				*strDest = *strSrc;
				strDest++;
			}
			*strDest = *strSrc;

			strDest++;
			strSrc++;
			i32OutSize++;

			if( i32Size <= i32OutSize )	break;
		}
		*strDest = L'\0';
	};

	inline void ConvertUINT32ToSmallDate( TCHAR* strDate, const UINT32 ui32Date, const INT32 i32Size )
	{
		S2String::Format( strDate, i32Size, _T("%04d-%02d-%02d %02d:%02d:00"), 
			(ui32Date / 100000000) + 2000,
			(ui32Date % 100000000) / 1000000,
			(ui32Date % 1000000 )  / 10000,
			(ui32Date % 10000 )    / 100,
			(ui32Date % 100 ) );
	};
	
	inline void ConvertUINT64ToDate( TCHAR* strDate, const UINT64 ui64Date, const INT32 i32Size )
	{
		S2String::Format( strDate, i32Size, _T("%04d-%02d-%02d %02d:%02d:%02d"), 
			(ui64Date / 10000000000) + 2000,
			(ui64Date % 10000000000) / 100000000,
			(ui64Date % 100000000 )  / 1000000,
			(ui64Date % 1000000 )    / 10000,
			(ui64Date % 10000 )      / 100,
			(ui64Date % 100 ) );
	};

	inline void MakeQuery( char* strDest, INT32 i32Size, char* strSPName, ... )
	{
		char strNum[ 32 ];
		char* pPtr;

		va_list marker;
		va_start( marker, strSPName );

		// EXEC 삽입
		S2String::Copy( strDest, "EXEC ", i32Size );
		strDest += 5;
		// SP 이름 삽입
		while( '\0' != *strSPName )
		{
			*strDest = *strSPName;
			strDest++;
			strSPName++;
		}
		// SP 이후 스페이스 삽입
		*strDest = ' ';	strDest++;

		BOOL bContinue	= TRUE;
		while( bContinue )
		{
			// DataType 을 얻어온다.
			S2DB_DATA_TYPE eDataType = va_arg( marker, S2DB_DATA_TYPE );
			switch( eDataType )
			{
			case S2DB_DATA_TYPE_BIT:
			case S2DB_DATA_TYPE_TINYINT:
			case S2DB_DATA_TYPE_SMALLINT:
			case S2DB_DATA_TYPE_INT:
				{
					INT32 i32Value = va_arg( marker, INT32 );
					S2_ITOA( strDest, i32Value );
					*strDest = ',';
					strDest++;
				}
				break;
			case S2DB_DATA_TYPE_BIGINT:
				{
					INT64 i64Value = va_arg( marker, INT64 );
					S2_ITOA( strDest, i64Value );
					*strDest = ',';
					strDest++;
				}
				break;
			case S2DB_DATA_TYPE_CHAR:
				{
					*strDest = 'N';
					strDest++;
				}
				//break; 주석이 맞습니다.
			case S2DB_DATA_TYPE_SMALLDATETIME:
			case S2DB_DATA_TYPE_DATETIME:
				{
					char* strValue = va_arg( marker, char* );
					S2_STRCPY( strDest, strValue );
					*strDest = ',';
					strDest++;
				}
				break;
			case S2DB_DATA_TYPE_END:
				{
					strDest--;
					bContinue = FALSE;
				}
				break;
			case S2DB_DATA_TYPE_REAL:
				{
					double  dVal = va_arg(marker, double);
					char pDoubleBuf[32];
					S2String::MakePointFix(dVal, pDoubleBuf, 6, false);

					S2_FCVT( strDest, pDoubleBuf);
					*strDest = ',';
					strDest++;
				}
				break;
			case S2DB_DATA_TYPE_BINARY:
				{
					char * strValue = va_arg(marker, char*);
					S2_FCVT(strDest, strValue);
					*strDest = ',';
					strDest++;
				}
				break;
			default:
				{
					S2ASSERT(0);
				}
				break;
			}
		}
		
		*strDest = '\0';

		va_end( marker);
	}

	inline void MakeQuery( wchar_t* wstrDest, INT32 i32Size, wchar_t* wstrSPName, ... )
	{
		wchar_t  strNum[ 32 ];
		wchar_t* pPtr;

		va_list marker;
		va_start( marker, wstrSPName );

		// EXEC 삽입
		S2String::Copy( wstrDest, L"EXEC ", i32Size );
		wstrDest += 5;
		// SP 이름 삽입
		while( '\0' != *wstrSPName )
		{
			*wstrDest = *wstrSPName;
			wstrDest++;
			wstrSPName++;
		}
		// SP 이후 스페이스 삽입
		*wstrDest = ' ';	wstrDest++;

		BOOL bContinue	= TRUE;
		while( bContinue )
		{
			// DataType 을 얻어온다.
			S2DB_DATA_TYPE eDataType = va_arg( marker, S2DB_DATA_TYPE );
			switch( eDataType )
			{
			case S2DB_DATA_TYPE_BIT:
			case S2DB_DATA_TYPE_TINYINT:
			case S2DB_DATA_TYPE_SMALLINT:
			case S2DB_DATA_TYPE_INT:
				{
					INT32 i32Value = va_arg( marker, INT32 );
					S2_ITOA( wstrDest, i32Value );
					*wstrDest = ',';
					wstrDest++;
				}
				break;
			case S2DB_DATA_TYPE_BIGINT:
				{
					INT64 i64Value = va_arg( marker, INT64 );
					S2_ITOA( wstrDest, i64Value );
					*wstrDest = ',';
					wstrDest++;
				}
				break;
			case S2DB_DATA_TYPE_CHAR:
			case S2DB_DATA_TYPE_SMALLDATETIME:
			case S2DB_DATA_TYPE_DATETIME:
				{
					*wstrDest = 'N';
					wstrDest++;

					wchar_t* wstrValue = va_arg( marker, wchar_t* );
					S2_STRCPY( wstrDest, wstrValue );

					*wstrDest = ',';
					wstrDest++;
				}
				break;
			case S2DB_DATA_TYPE_END:
				{
					wstrDest--;
					bContinue = FALSE;
				}
				break;
			case S2DB_DATA_TYPE_REAL:
				{
					double  dVal = va_arg(marker, double);
					wchar_t pDoubleBuf[32];
					S2String::MakePointFix(dVal, pDoubleBuf, 6, false);

					S2_FCVT( wstrDest, pDoubleBuf);
					*wstrDest = ',';
					wstrDest++;
				}
				break;
			case S2DB_DATA_TYPE_BINARY:
				{
					wchar_t * strValue = va_arg(marker, wchar_t*);
					S2_FCVT( wstrDest, strValue );
					*wstrDest = ',';
					wstrDest++;
				}
				break;
			default:
				{
					S2ASSERT(0);
				}
				break;
			}
		}
		
		*wstrDest = '\0';

		va_end( marker);
	}
};

#endif