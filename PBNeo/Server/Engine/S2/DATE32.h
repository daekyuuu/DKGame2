#ifndef __S2_DATE32_H__
#define __S2_DATE32_H__

#include <time.h>
#include <ATLComTime.h>

#define SMALLDATE_STRING_SIZE		20
#define DATE32_YEAR_INIT			1970
#define DATE32_YEAR_MAX				(DATE32_YEAR_INIT + 63)
#define SUMMER_TIME_FOLLOW_SYSTEM	-1			// A value less than zero to have the C run-time library code compute whether standard time or daylight saving time is in effect.

enum S2_DATE_TYPE
{
	S2_DATE_TYPE_YEAR		= 0xFC000000,
	S2_DATE_TYPE_MONTH		= 0x3C00000,
	S2_DATE_TYPE_DAY		= 0x3E0000,
	S2_DATE_TYPE_HOUR		= 0x1F000,
	S2_DATE_TYPE_MINUTE		= 0xFC0,
	S2_DATE_TYPE_SECOND		= 0x3F,
};

enum S2_DATE_ADD_TYPE
{
	S2_DATE_ADD_TYPE_SECOND,
	S2_DATE_ADD_TYPE_MINUTE,
	S2_DATE_ADD_TYPE_HOUR,
	S2_DATE_ADD_TYPE_DAY,
	S2_DATE_ADD_TYPE_MONTH,
	S2_DATE_ADD_TYPE_YEAR,
};

class S2_EXPORT_BASE DATE32
{
	// 32 비트를 사용합니다.
	// 111111 1111 11111 11111 111111 111111
	// (년도) (월)  (일)  (시)   (분)   (초)
	UINT32					m_ui32Date;

public:
	DATE32();
	DATE32( UINT32 ui32Date );
	~DATE32();

	INT32					GetYear()			{		return ((m_ui32Date & S2_DATE_TYPE_YEAR)	>> 26) + DATE32_YEAR_INIT;	}
	INT32					GetMonth()			{		return (m_ui32Date & S2_DATE_TYPE_MONTH)	>> 22;						}
	INT32					GetDay()			{		return (m_ui32Date & S2_DATE_TYPE_DAY)		>> 17;						}
	INT32					GetHour()			{		return (m_ui32Date & S2_DATE_TYPE_HOUR)		>> 12;						}
	INT32					GetMinute()			{		return (m_ui32Date & S2_DATE_TYPE_MINUTE)	>> 6;						}
	INT32					GetSecond()			{		return (m_ui32Date & S2_DATE_TYPE_SECOND);							}
	UINT32					GetDate()			{		return m_ui32Date;													}
		
	inline void				GetSmallDate( char* strDate, INT32 i32Length );
	inline void				GetSmallDate( wchar_t* wstrDate, INT32 i32Length );
	inline UINT32			GetDateTimeBin( INT32 i32DateType = (S2_DATE_TYPE_YEAR | S2_DATE_TYPE_MONTH | S2_DATE_TYPE_DAY | S2_DATE_TYPE_HOUR | S2_DATE_TYPE_MINUTE | S2_DATE_TYPE_SECOND) );
	inline UINT32			GetDateTimeYYYYMMDD();
	inline UINT32			GetDateTimeYYMMDDHHMI();
	inline UINT32			GetDateTimeMMDDHHMISS();
	
	void					Reset()				{		m_ui32Date = 0;														} 
	void					AddTime( S2_DATE_ADD_TYPE eAddType, INT32 i32Time );
	INT32					DiffTime( DATE32 dt32Date );

	inline DATE32&			operator=( const SYSTEMTIME& pTime );
	inline DATE32&			operator=( const tm& pTime );
	inline DATE32&			operator=( const DATE& pTime );
	inline DATE32&			operator=( const UINT32 pTime );
	inline DATE32&			operator=( const DATE32& pTime );

	friend inline BOOL		operator==( const DATE32& dt32Date1, const DATE32& dt32Date2 )
	{
		return (dt32Date1.m_ui32Date == dt32Date2.m_ui32Date );
	}
	friend inline BOOL		operator!=( const DATE32& dt32Date1, const DATE32& dt32Date2 )
	{
		return (dt32Date1.m_ui32Date != dt32Date2.m_ui32Date );
	}
	friend inline BOOL		operator<( const DATE32& dt32Date1, const DATE32& dt32Date2 )
	{
		return (dt32Date1.m_ui32Date < dt32Date2.m_ui32Date );
	}
	friend inline BOOL		operator<=( const DATE32& dt32Date1, const DATE32& dt32Date2 )
	{
		return (dt32Date1.m_ui32Date <= dt32Date2.m_ui32Date );
	}
	friend inline BOOL		operator>( const DATE32& dt32Date1, const DATE32& dt32Date2 )
	{
		return (dt32Date1.m_ui32Date > dt32Date2.m_ui32Date );
	}
	friend inline BOOL		operator>=( const DATE32& dt32Date1, const DATE32& dt32Date2 )
	{
		return (dt32Date1.m_ui32Date >= dt32Date2.m_ui32Date );
	}

};

#endif