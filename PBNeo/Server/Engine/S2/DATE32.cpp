#include "stdafx.h"
#include "DATE32.h"

DATE32::DATE32()
{
	m_ui32Date = 0;
}

DATE32::DATE32( UINT32 ui32Date )
{
	m_ui32Date = ui32Date;
}

DATE32::~DATE32()
{
}

DATE32&	DATE32::operator=( const SYSTEMTIME& pTime )
{
	INT32 i32Year = pTime.wYear;
	if( DATE32_YEAR_MAX < i32Year )	i32Year = DATE32_YEAR_MAX;

	m_ui32Date =	((i32Year - DATE32_YEAR_INIT)	<< 26) |
					(pTime.wMonth					<< 22) |
					(pTime.wDay						<< 17) |
					(pTime.wHour					<< 12) |
					(pTime.wMinute					<< 6) |
					pTime.wSecond;
	return *this;
}

DATE32&	DATE32::operator=( const tm& pTime )
{
	INT32 i32Year = pTime.tm_year;
	if( DATE32_YEAR_MAX < i32Year )	i32Year = DATE32_YEAR_MAX;

	m_ui32Date =	(((i32Year + 1900) - DATE32_YEAR_INIT)	<< 26) |
					((pTime.tm_mon + 1)						<< 22) |
					(pTime.tm_mday							<< 17) |
					(pTime.tm_hour							<< 12) |
					(pTime.tm_min							<< 6) |
					pTime.tm_sec;
	return *this;
}

DATE32&	DATE32::operator=( const DATE& pTime )
{
	COleDateTime oleDate( pTime );

	INT32 i32Year = oleDate.GetYear();
	if( DATE32_YEAR_MAX < i32Year )	i32Year = DATE32_YEAR_MAX;

	m_ui32Date =	((i32Year - DATE32_YEAR_INIT)	<< 26) |
					(oleDate.GetMonth()				<< 22) |
					(oleDate.GetDay()				<< 17) |
					(oleDate.GetHour()				<< 12) |
					(oleDate.GetMinute()			<< 6) |
					oleDate.GetSecond();

	return *this;
}

DATE32&	DATE32::operator=( const UINT32 pTime )
{
	m_ui32Date = pTime;
	return *this;
}

DATE32& DATE32::operator=( const DATE32& pTime )
{
	m_ui32Date = pTime.m_ui32Date;
	return *this;
}

UINT32 DATE32::GetDateTimeBin( INT32 i32DateType )
{
	return (m_ui32Date & i32DateType);
}

UINT32 DATE32::GetDateTimeYYYYMMDD()
{
	return GetYear()*10000 + GetMonth() * 100 + GetDay();
}

UINT32 DATE32::GetDateTimeYYMMDDHHMI()
{
	return (GetYear()%100)*100000000 + GetMonth()*1000000 + GetDay()*10000 + GetHour()*100 + GetMinute();
}

UINT32 DATE32::GetDateTimeMMDDHHMISS()
{
	return GetMonth()*100000000 + GetDay()*1000000 + GetHour()*10000 + GetMinute()*100 + GetSecond();
}

void DATE32::GetSmallDate( char* strDate, INT32 i32Length )
{
	S2String::Format( strDate, i32Length, "%04d-%02d-%02d %02d:%02d:%02d", 
		GetYear(), GetMonth(), GetDay(), GetHour(), GetMinute(), GetSecond() );
}

void DATE32::GetSmallDate( wchar_t* wstrDate, INT32 i32Length )
{
	S2String::Format( wstrDate, i32Length, L"%04d-%02d-%02d %02d:%02d:%02d", 
		GetYear(), GetMonth(), GetDay(), GetHour(), GetMinute(), GetSecond() );
}

void DATE32::AddTime( S2_DATE_ADD_TYPE eAddType, INT32 i32Time )
{
	struct tm oldTime;
	oldTime.tm_year		= (GetYear() - 1900)	+ (S2_DATE_ADD_TYPE_YEAR	== eAddType?i32Time:0);
	oldTime.tm_mon		= GetMonth() - 1		+ (S2_DATE_ADD_TYPE_MONTH	== eAddType?i32Time:0);
	oldTime.tm_mday		= GetDay()				+ (S2_DATE_ADD_TYPE_DAY		== eAddType?i32Time:0);
	oldTime.tm_hour		= GetHour()				+ (S2_DATE_ADD_TYPE_HOUR	== eAddType?i32Time:0);
	oldTime.tm_min		= GetMinute()			+ (S2_DATE_ADD_TYPE_MINUTE	== eAddType?i32Time:0);
	oldTime.tm_sec		= GetSecond()			+ (S2_DATE_ADD_TYPE_SECOND	== eAddType?i32Time:0);
	oldTime.tm_isdst	= SUMMER_TIME_FOLLOW_SYSTEM;
	mktime( &oldTime );
	this->operator=( oldTime );
}

INT32 DATE32::DiffTime( DATE32 dt32Date )
{
	struct tm tmTime1;
	
	tmTime1.tm_year		= dt32Date.GetYear() - 1900;
	tmTime1.tm_mon		= dt32Date.GetMonth() - 1;
	tmTime1.tm_mday		= dt32Date.GetDay();
	tmTime1.tm_hour		= dt32Date.GetHour();
	tmTime1.tm_min		= dt32Date.GetMinute();
	tmTime1.tm_sec		= dt32Date.GetSecond();
	tmTime1.tm_isdst	= SUMMER_TIME_FOLLOW_SYSTEM;

	time_t tm_tTime1	= mktime( &tmTime1 );

	struct tm tmTime2;
	
	tmTime2.tm_year		= GetYear() - 1900;
	tmTime2.tm_mon		= GetMonth() - 1;
	tmTime2.tm_mday		= GetDay();
	tmTime2.tm_hour		= GetHour();
	tmTime2.tm_min		= GetMinute();
	tmTime2.tm_sec		= GetSecond();
	tmTime2.tm_isdst	= SUMMER_TIME_FOLLOW_SYSTEM;
	
	time_t tm_tTime2	= mktime( &tmTime2 );

	return (INT32)(difftime( tm_tTime2, tm_tTime1 ));
}