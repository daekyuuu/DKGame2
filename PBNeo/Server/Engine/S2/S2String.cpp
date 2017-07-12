#include "stdafx.h"
#include "S2String.h"
#include <stdio.h>

void S2String::Copy( char * strDest, const char * strSrc, INT32 i32DestLength )
{
	S2ASSERT( strDest && strSrc );

	if ( i32DestLength <= 0 )
		return;

	while( *strSrc )
	{
		*strDest = *strSrc;

		strDest++;
		strSrc++;
		--i32DestLength;
		if( 0 > i32DestLength ) break;
	}

	*strDest = '\0';
}

void S2String::Copy( wchar_t * strDest, const wchar_t * strSrc, INT32 i32DestLength )
{
	S2ASSERT( strDest && strSrc );

	if ( i32DestLength <= 0 )
		return;

	while( *strSrc )
	{
		*strDest = *strSrc;

		strDest++;
		strSrc++;
		--i32DestLength;
		if( 0 > i32DestLength ) break;
	}

	*strDest = '\0';
}

void S2String::Concat( char * strDest, const char * strSrc, INT32 i32DestLength )
{
	while( *strDest )
	{
		strDest++;
		i32DestLength--;
	}
	
	while( *strSrc )
	{
		*strDest = *strSrc;
		
		strDest++;
		strSrc++;

		--i32DestLength;
		if( 0 > i32DestLength ) break;
	}
	
	*strDest = '\0';
}

void S2String::Concat( wchar_t * strDest, const wchar_t * strSrc, INT32 i32DestLength )
{
	while( *strDest )
	{
		strDest++;
		i32DestLength--;
	}
	
	while( *strSrc )
	{
		*strDest = *strSrc;
		
		strDest++;
		strSrc++;

		--i32DestLength;
		if( 0 > i32DestLength ) break;
	}
	
	*strDest = 0;
}

INT32 S2String::Length( const char * strStr )
{
	INT32 i32Count = 0;

	while( *strStr)
	{
		i32Count++;
		strStr++;
	}

	return i32Count;
}

INT32 S2String::Length( const wchar_t * strStr )
{
	INT32 i32Count = 0;

	while( *strStr )
	{
		i32Count++;
		strStr++;
	}

	return i32Count;
}

INT32 S2String::Format( char * strDest, INT32 i32Length, const char * strFormat, ... )
{
	va_list marker;
	va_start( marker, strFormat);
	INT32 i32Rv = ::_vsnprintf_s( strDest, i32Length, _TRUNCATE, strFormat, marker);
	va_end( marker);
	if( 0 > i32Rv )		i32Rv = i32Length-1;
	return i32Rv;
}

INT32 S2String::Format( wchar_t * strDest, INT32 i32Length, const wchar_t * strFormat, ...)
{
	va_list marker;
	va_start( marker, strFormat);
	INT32 i32Rv = ::_vsnwprintf_s( strDest, i32Length, _TRUNCATE, strFormat, marker);
	va_end( marker);
	if( 0 > i32Rv )		i32Rv = i32Length-1;
	return i32Rv;
}


//////////////////////////////////////////////////////////////////////////////////////
// 실수를 문자열로 변환한다.
//
// double Value		: 변환하려는 실수값
// char *Result		: 문자열 결과를 받기 위한 버퍼의 포인터
// int UnderCount	: 문자열로 변환할 때, 남길 소숫점 아래의 개수
// bool FillZero	: 소숫점 아래자리가 UnderCount에 미치지 않을 경우, 나머지 자리를
//					  0으로 채울 것인지 말것인지 결정하는 인자
//					  ex.) 1.2를 소숫점 3자리까지 만들때
//						   FillZero = false -> 1.2
//						   FillZero = true  -> 1.200
//
// 작성자			: 이진균
// 작성일자			: 1999.6.1
void S2String::MakePointFix( REAL64 Value, char *Result, INT32 UnderCount, bool FillZero)
{ 
	char conv[128];
    int i, j, FractCount;
    bool UnderFlag, fFindNull;

    S2String::Format( (PSTR)conv, 128, (PSTR) "%f", Value);

    FractCount = 0;
    UnderFlag = false;		// 소숫점이 발견되었으며, 현재 소숫점 이하 자리를 처리하고 있음을 나타낸다.
    fFindNull = false;

    for( i = 0, j = 0 ; (UnderFlag != true) || (FractCount < UnderCount) ; ) 
	{
    	switch( conv[i]) 
		{
        	case '.' :
            	UnderFlag = true;
                Result[j] = '.';
                break;
            case 0 :
            	if( FillZero == false)
				{
                	UnderFlag = true;
                    FractCount = UnderCount;
                    fFindNull = true;
                }
                break;
            default :
            	if( UnderFlag == false)
				{
                	Result[j] = conv[i];
                }
                else
				{
                   	if( fFindNull == true)			Result[j] = '0';
                    else					    	Result[j] = conv[i];

                    FractCount ++;

                    if( FractCount >= UnderCount)
					{
                    	if( fFindNull != true)
						{
                        	if( conv[i+1] >= '5')
							{
                            	bool Borrow;
                            	int Saved = j;

                                do {
                                	Borrow = false;

                                    if( Result[j] == '.')
									{
                                    	Borrow = true;		// for cheat...
                                        j--;
                                    	continue;
                                    }

                                	if( Result[j] == '9')
									{
                                    	Result[j] = '0';

                                        j--;
                                        if( j < 0)
										{
                                        	// 반올림이 계속되어 정수자리를 넘어 단위자체가 올라가야 하는 경우...
                                            // 전체 문자열을 한칸 뒤로 밀고 첫자리에 '1'을 삽입한다.
                                            // ex) 999.99 에서 소숫점 2자리에서 반올림하여 1자리로 만드는 경우 1000.0 이 되어야 한다.
                                            for( i = Saved; i >= 0; i--)
											{
                                            	Result[i+1] = Result[i];
                                            }
                                            j = 0;
                                            Result[j] = '1';
                                            Saved ++;
                                        }
                                        else
										{
                                        	Borrow = true;
										}
                                    }
                                    else
									{
                                		Result[j] ++;
									}
                                } while(Borrow == true);

                                j = Saved;
                            }
                        }
                    }
                }
            	break;
        }

        if( conv[i] != 0)
        	i++;
        j++;
    }

    Result[j] = 0;
}

void S2String::MakePointFix( REAL64 Value, wchar_t *Result, INT32 UnderCount, bool FillZero)
{ 
	wchar_t conv[128];
    int i, j, FractCount;
    bool UnderFlag, fFindNull;

    S2String::Format( conv, 128, L"%f", Value);

    FractCount = 0;
    UnderFlag = false;		// 소숫점이 발견되었으며, 현재 소숫점 이하 자리를 처리하고 있음을 나타낸다.
    fFindNull = false;

    for( i = 0, j = 0; (UnderFlag != true) || (FractCount < UnderCount);) {
    	switch( conv[i]) {
        	case '.' :
            	UnderFlag = true;
                Result[j] = '.';
                break;

            case 0 :
            	if( FillZero == false) {
                	UnderFlag = true;
                    FractCount = UnderCount;
                    fFindNull = true;
                }
                break;

            default :
            	if( UnderFlag == false) {
                	Result[j] = conv[i];
                }
                else {
                   	if( fFindNull == true)
                       	Result[j] = '0';
                    else
                       	Result[j] = conv[i];

                    FractCount ++;

                    if( FractCount >= UnderCount) {
                    	if( fFindNull != true) {
                        	if( conv[i+1] >= '5') {
                            	bool Borrow;
                            	int Saved = j;

                                do {
                                	Borrow = false;

                                    if( Result[j] == '.') {
                                    	Borrow = true;		// for cheat...
                                        j--;
                                    	continue;
                                    }

                                	if( Result[j] == '9') {
                                    	Result[j] = '0';

                                        j--;
                                        if( j < 0) {
                                        	// 반올림이 계속되어 정수자리를 넘어 단위자체가 올라가야 하는 경우...
                                            // 전체 문자열을 한칸 뒤로 밀고 첫자리에 '1'을 삽입한다.
                                            // ex) 999.99 에서 소숫점 2자리에서 반올림하여 1자리로 만드는 경우 1000.0 이 되어야 한다.
                                            for( i = Saved; i >= 0; i--) {
                                            	Result[i+1] = Result[i];
                                            }
                                            j = 0;
                                            Result[j] = '1';
                                            Saved ++;
                                        }
                                        else
                                        	Borrow = true;
                                    }
                                    else
                                		Result[j] ++;
                                } while(Borrow == true);

                                j = Saved;
                            }
                        }
                    }
                }
            	break;
        }

        if( conv[i] != 0)
        	i++;
        j++;
    }

    Result[j] = 0;
}

/*
/////////////////////////////////////////////////////////////////////////////////////////
// 값을 Comma(,)를 삽입한 포맷으로 문자열 변환한다.
// 
// LONG Value		: 문자열로 변환할 값
// LPTSTR StrBuf	: 결과 문자열을 반환할 버퍼의 포인터
// int Length		: 제한할 최대 길이
// 
// return 
//			= 0		: Error
//			> 0		: 변환된 문자열의 총 길이(Bytes)
//
// ex)	123456780 => 1,234,567,890
//
// 작성자			: 이진균, 차재천
// 작성일자			: 1999.6.1, 2005.4.4
INT32 S2String::ValueToStr( INT32 Value, char * szOut, INT32 Length)
{
	int i, MaxLen, StrLength;
	char conv[128];

	sprintf( (PSTR) conv, (PSTR) "%ld", Value);
	StrLength = (int) strlen( conv);

	if( StrLength <= 0)
	{
		return 0;
	}

	MaxLen = StrLength + ( StrLength - 1 ) / 3;
	if( MaxLen >= Length )
	{
		return 0;
	}

	*(szOut + MaxLen) = 0;

	char *pIn = conv + StrLength - 1;
	char *pOut = szOut + MaxLen - 1;
	for( i = 0; i < MaxLen; i++ )
	{
		if( (i + 1) % 4 == 0 )
		{
			*pOut = ',';
		}
		else
		{
			*pOut = *pIn;
			pIn--;
		}
		pOut--;
	}

	return MaxLen;
}

////////////////////////////////////////////////////////////////////////////////////////
// 문자열에서 처음 발견되는 valid character(출력가능한 문자)의 위치를 포인터로 반환한다.
//
// return		: 문자열에서 처음 발견되는 문자의 포인터
// 
// note) white character로 판별하는 경우
//		 space, tab, carrige return(\r), new line(\n)
//		
//		만약 문자열이 빈 문자열일 경우 올바르게 동작하지 않을 수 있다.
//
// 작성자		: 이진균
// 작성일자		: 1999.6.1
const char * S2String::GetFirstValidCharater( const char * lpszStr)
{
	const char *p = lpszStr;

	while( (*p == ' ') || (*p == '\t') || (*p == '\r') || (*p == '\n'))
		p++;

	return p;
}

////////////////////////////////////////////////////////////////////////////////////////
// 문자열에서 특정 인덱스의 문자가 영문, 한글 첫번째 Byte, 또는 한굴 두번째 Byte인지를
// 판별하여 반환한다.
// 
// char *str	: 문자열의 포인터
// int index	: 판별하려는 문자의 인덱스 (0 based)
//
// return
//			영문			: 0
//			한글 1st byte	: 1
//			한글 2nd byte	: 2
//
// 작성자		: 이진균
// 작성일자		: 1999.6.1
INT32 S2String::WhatIsCode( const char * str, INT32 index)
{
	int i, rv;

	rv = 0;

	for( i = 0; i <= index; i++) {
		if( rv == 1)
			rv = 2;
		else if( *str & 0x80) 
			rv = 1;
		else 
			rv = 0;

		str++;
	}

	return rv;
}

///////////////////////////////////////////////////////////////////////////////////////
// 문자열의 특정 위치에 다른 문자열을 삽입한다.
//
// char * Dest	: 문자열을 삽입할 문자열
// char * Src	: 삽입될 문자열
// int Pos		: 문자열을 삽입할 인덱스 (0 based)
// int Len		: 삽입할 문자열의 길이
//
// Return		: 삽입된 문자열의 선두 포인터
//
// 작성자		: 이진균
// 작성일자		: 1999.6.1
char * S2String::InsertStr( char * Dest, const char * Src, INT32 Pos, INT32 Len)
{
	char *p, *q;
	INT32 Count, i;

	Count = (INT32)strlen( Dest) - Pos;

	p = Dest;
	while( *p) p++;

	q = p + Len;

	for( i = 0; i < Count+1; i++) {
		*q = *p;
		p--;
		q--;
	}

	p = Dest + Pos;
	const char * r = Src;

	for( i = 0; i < Len; i++) {
		*p = *r;
		p++;
		r++;
	}

	return Dest; 
}	

WCHAR16 * S2String::InsertStrW( WCHAR16 * Dest, const WCHAR16 * Src, INT32 Pos, INT32 Len)
{
	WCHAR16 *p, *q;
	int Count, i;

	Count = (int)Length( Dest) - Pos;

	p = Dest;
	while( *p) p++;

	q = p + Len;

	for( i = 0; i < Count+1; i++) {
		*q = *p;
		p--;
		q--;
	}

	p = Dest + Pos;
	const WCHAR16 * r = Src;

	for( i = 0; i < Len; i++) {
		*p = *r;
		p++;
		r++;
	}

	return Dest; 
}	

//////////////////////////////////////////////////////////////////////////////////////
// 문자열의 앞부분에서부터 공백 문자를 제거한다.
// 출력-가능 문자가 발견되면 처리를 종료한다. (뒷부분의 공백문자를 처리하지 않음)
//
// char *str	: 공백문자를 삭제할 문자열
// 
// Return		: None
//
// 작성자		: 이진균
// 작성일자		: 1999.6.1
void S2String::RemoveBlankFromHead( char * str)
{
	unsigned char *sp, *tp;

	sp = (unsigned char *) str;
	while((*sp <= 32) && (*sp != 0))
	{
		sp ++;
	}

	if( *sp == 0)
		return;

	tp = (unsigned char *) str;

	while( *sp) {
		*tp = *sp;
		tp++;
		sp++;
	}
	*tp = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// 문자열의 뒷부분에서부터 공백 문자를 삭제한다.
// 출력-가능한 문자가 발견되면 처리를 종료한다. (앞부분의 공백문자를 처리하지 않음)
//
// char *str	: 공백문자를 제거할 문자열의 포인터
//
// Return		: none
//
// 작성자		: 이진균
// 작성일자		: 1999.6.1
void S2String::RemoveBlankFromTail( char * str)
{
	unsigned char *sp;
	int len;

	len = (int)strlen(str);
	sp = (unsigned char *)(str + (len - 1));

	while(len && *sp < 0x21)
	{
		sp--;
		len--;	// 길이 만큼만 처리하여 index underflow를 막는다. - praptor
	}

	sp++;
	*sp = 0;
}

void S2String::RemoveAllBlank( char *str)
{
	unsigned char *p;

	p = (unsigned char *)str;

	while( *p) {
		if( !isspace( *p) || (*p > 127)) {
			*str = *p;
			str++;
		}
		p++;
	}
	*str = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// 문자열에 포함된 특정 문자를 모두 삭제한다.
// (영문일 경우 대소문자 구분함.)
//
// char *str	: 특정 문자를 제거할 문자열의 포인터
//
// Return		: none
//
// 작성자		: 백성민
// 작성일자		: 2008.8.22
void S2String::RemoveAllChar( char *str, char RemoveChar)
{
	unsigned char *p = (unsigned char *) str;

	while( *p) {
		if( *p != RemoveChar) {
			*str = *p;
			str++;
		}
		p++;
	}
	*str = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// 파일명을 담은 문자열에서 뒷부부분의 확장자를 제거한 문자열로 만든다.
//
// char *fname	: 문자열의 포인터
//
// Return		: none
//
// 작성자		: 이진균
// 작성일자		: 1999.6.1
void S2String::RemoveExt( char *fname)
{
	char *p;

	p = fname + strlen(fname);

	while((p > fname) && (*p != '.')) {
		p--;
	}

	if( *p == '.') {
		*p = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
// 파일명을 담은 문자열에서 뒷부부분의 확장자를 새로운 확장자로 교체한다.
///
/// char *szPath			: 문자열의 포인터
/// const char *szNewExt	: 새로운 확장자		예) "txt"
///
/// \Return		: none
//
// 작성자		: 차재천
// 작성일자		: 2005.8.19
void S2String::SwapExt( char *szOut, const INT32 nOutMax, const char *szIn, const char *szNewExt )
{
	if( szOut != szIn )
	{
		Copy( szOut, szIn, nOutMax );
	}
	RemoveExt( szOut );
	Concat( szOut, "." );
    Concat( szOut, szNewExt );
}

char * S2String::SplitFileName( const char *fname, char *Buf, BOOL WithExt)
{
	// considerable cases
	// 1. C:\\Directory\\FileName.EXT
	// 2. ..\Directory\\FileName.EXT
	// 3. FileName.EXT
	// 4. FileName
	INT32 len = (INT32)strlen( fname);
	char *pStart, *pResult;

	if( len <= 0)
		return (char *) fname;

	pStart = (char *)(fname + len - 1);
	while((pStart > fname) && (*pStart != '\\') && (*pStart != '/')) 
		pStart --;

	if((*pStart == '\\') || (*pStart == '/'))
		pStart++;

	pResult = pStart;

	while( *pStart) {
		if( Buf != NULL) {
			*Buf = *pStart;

			Buf++;
		}

		pStart++;

		if( WithExt == FALSE) 
			if( *pStart == '.') {

				if( Buf != NULL)
					*Buf = 0;
				return pResult;
			}
	}

	if( Buf != NULL)
		*Buf = 0;

	return pResult;
}

void S2String::SplitPath( const char * pszSrc, char * pszDest)
{
	char * pszTemp, *pszEnd;

	pszEnd = (char *) pszSrc + strlen( pszSrc);

	// Path의 끝점을 찾는다.
	while( (pszEnd > pszSrc))
	{
		if( (*pszEnd == '\\') || (*pszEnd == '/'))
			break;

		pszEnd --;
	}

	// 앞쪽에서부터 pszEnd 위치까지 복사
	pszTemp = (char *) pszSrc;

	while( pszTemp < pszEnd)
	{
		*pszDest = *pszTemp;
		pszDest++;
		pszTemp++;
	}

	*pszDest = 0;
}


////////////////////////////////////////////////////////////////////////////////////////
// 파일명을 담은 문자열에서 뒷부부분의 확장자만을 문자열로 만든다.
//
// char *fname	: 문자열의 포인터
// char *ExtBuf : 확장자를 담을 문자열 포인터(NULL일 경우 개수만 리턴)
//
// Return		: 확장자 문자열의 문자 개수
//
// 작성자		: 경주현
// 작성일자		: 1999.6.21
INT32 S2String::SplitFileExt( const char *fname, char *BufExt, int ) //BufExtSize )
{
	int Count = 0;

	for (int i = (int)strlen(fname) - 1; i >= 0; i--) {

		if ( fname[i] == '.' ) {

			if (BufExt != NULL)
			{
				i3mem::Copy( BufExt, (void *)(fname + i + 1), Count);

				BufExt[ Count] = 0;
			}

			break;
		}
		else
			Count++;
	}

	return Count;
}

void S2String::RemoveDriveChar( char *path)
{
	char *pTemp;

	pTemp = path;

	while( *pTemp != 0) {
		if( *pTemp == ':')
			break;

		pTemp++;
	}

	if( *pTemp == 0)
		return;

	pTemp++;

	while( *pTemp != 0) {
		*path = *pTemp;

		path++;
		pTemp++;
	}

	*path = 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
// 문자열에 있는 모든 공백을 제거한다.
//
// char *str	: 문자열의 포인터
// char *Buf    : 제거한 문자열을 저장할 버퍼(NULL일 경우 제외)
//
// Return		: 공백을 제거한 문자열의 문자 개수
//
// 작성자		: 경주현
// 작성일자		: 1999.7.19
INT32 S2String::EraseSpace(const char *str, char *Buf)
{
	unsigned int Count = 0;

	while( *str )
	{
		if( *str != ' ' )
		{
			*Buf = *str;
			Buf++;
			Count++;
		}
		str++;
	}

	*Buf = 0;

	return Count;
}

bool S2String::IsDigitString( const char *str)
{
	while( *str) {
		if( ! (isdigit( *str) || (*str == '.') || (*str == '+') || (*str == ':') || (*str == '-'))) {
			return false;
		}

		str++;
	}

	return true;
}

void S2String::ParseMultiSelectFileName( char *FileName, S2StringList *List)
{
	char *p;
	char conv[MAX_PATH];
	int idx;

	p = FileName;

	while( *p) {

		idx = 0;
		while( *p) {
			conv[idx] = *p;

			idx++;
			p++;
		}

		conv[idx] = 0;

		if( idx > 0)
			List->Add( conv);

		p++;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// 파일명에 접미사를 붙인다.
//
// 예) "Test.txt" + "_KOR" => "Test_KOR.txt"
//
void S2String::AddPostfix( char *szOut, const INT32 nOutMax, const char *szIn, const char *szPostfix, char chDot )
{
	char szTemp[MAX_PATH];

	INT32 nDotPos;
	nDotPos = ContainCharRev( szIn, chDot );
	if( nDotPos < 0 )
	{	// chDot 이 없는 경우, 그냥 맨 뒤에다가 붙인다.
		Copy( szTemp, szIn, MAX_PATH );
		Concat( szTemp, szPostfix );
	}
	else if( nDotPos == 0 )
	{	// chDot 이 맨처음에 하나만 있는 경우, 접미사를 복사하고 그 뒤에다가 나머지를 붙인다.
		Copy( szTemp, szPostfix, MAX_PATH );
		Concat( szTemp, szIn );
	}
	else
	{	// chDot 이전의 내용을 복사
		NCopy( szTemp, szIn, nDotPos );
		// 접미사 붙이기
		Concat( szTemp, szPostfix );
		// 나머지 부분 붙이기
		Concat( szTemp, &szIn[nDotPos] );
	}

	Copy( szOut, szTemp, nOutMax );
}

////////////////////////////////////////////////////////////////////////////////////////
// 주어진 경로의 끝에 역슬레쉬(\)가 존재하는 경우, 역슬레쉬를 없앤다.
// 
// LPSTR Path	: 경로명을 저장한 문자열
//
// Return		: None
//
// 작성자		: 이진균
// 작성일자		: 1999.6.1
void S2String::NormalizePath( char* Path)
{
	char* p;

    p = Path;
    while( *p) p++;

    do {
    	*p = 0;
        p--;
    } while( ( *p == '\\' ) || ( *p == '/' ) );
}

void S2String::MakeUnixPath( char *szPath )
{
	I3ASSERT( szPath != NULL );
	char *p = szPath;
	while( *p != 0 )
	{
		if( *p == '\\' )
		{
			*p = '/';
		}
		p++;
	}
}

/// 파일명을 담은 문자열에서 파일명을 제거하고 디렉토리 부분만을 문자열로 만든다.
void S2String::RemoveFileName( char *Path )
{	
	char* p;

	p = Path;
	if( *p == 0 ) return;

	while( *p) p++;

	do
	{
		*p = 0;
		p--;
	}
	while( (*p != '\\') && (*p != '/') && (p > Path) );
	*p = 0;
}

INT32 S2String::Compare( const char* str1, const char* str2)
{
#ifdef USE_C_LIBRARY_FUNCS

	return _stricmp( str1, str2 );

#else
	INT32 C1, C2;

	if ( str1 == NULL )
	{
		if ( str2 == NULL )
			return 0;
		else
			return -1;
	}
	else if ( str2 == NULL )
	{
		return 1;
	}

	do
	{
		C1 = *str1++;
		C2 = *str2++;

		if( C1 != C2)
		{		
			//	특수 문자 때문에 대문자를 소문자로 바꿔서 연산하는 방식으로 수정함. 다른 함수도 테스트후 수정 필요. by 2005.09.30 (KOMET)
			if((C1 >= 'A') && (C1 <= 'Z'))		
				C1 += 'a' - 'A';

			if((C2 >= 'A') && (C2 <= 'Z'))
				C2 += 'a' - 'A';
			
			if( C1 != C2)
				return C1 < C2 ? -1 : 1;
		}
	}
	while( C1 );

	return 0;
#endif
}

INT32 S2String::Compare( const wchar_t* str1, const wchar_t* str2)
{
#ifdef USE_C_LIBRARY_FUNCS
	return _wcsicmp( str1, str2);
#else
	WCHAR16 C1, C2;
	do
	{
		C1 = *str1;
		C2 = *str2;

		if((C1 >= WCHAR16('a') ) && (C1 <= WCHAR16('z')))
			C1 += WCHAR16('A' - 'a');

		if((C2 >= WCHAR16('a')) && (C2 <= WCHAR16('z')))
			C2 += WCHAR16('A' - 'a');

		if( C1 != C2)
			return C1 - C2;

		str1++;
		str2++;
	}
	while((*str1 != 0) && (*str2 != 0));

	C1 = *str1;
	C2 = *str2;

	return C1 - C2;
#endif
}

INT32 S2String::NCompare( const char* str1, const char* str2, INT32 size)
{
#ifdef USE_C_LIBRARY_FUNCS

	return _strnicmp( str1, str2, size );

#else
	INT32	C1, C2;
	INT32	nSize = 0;

	if ( str1 == NULL )
	{
		if ( str2 == NULL )
			return 0;
		else
			return -1;
	}
	else if ( str2 == NULL )
	{
		return 1;
	}

	do 
	{
		C1 = *str1;
		C2 = *str2;

		if((C1 >= 'a') && (C1 <= 'z'))
			C1 += 'A' - 'a';

		if((C2 >= 'a') && (C2 <= 'z'))
			C2 += 'A' - 'a';

		if( C1 != C2)
			return C1 - C2;

		str1++;
		str2++;
		nSize++;
	} 
	while((*str1 != 0) && (*str2 != 0) && (nSize < size));

	if(*str1 == 0)
		return *str1 - *str2;

	return 0;
#endif
}

INT32 S2String::NCompareW( const WCHAR16* str1, const WCHAR16* str2, INT32 size)
{
#ifdef USE_C_LIBRARY_FUNCS
	return _wcsnicmp( str1, str2, size);
#else
	WCHAR16	C1, C2;
	INT32 nSize = 0;

	do 
	{
		C1 = *str1;
		C2 = *str2;

		if((C1 >= WCHAR16('a')) && (C1 <= WCHAR16('z')))
			C1 += 'A' - 'a';

		if((C2 >= WCHAR16('a')) && (C2 <= WCHAR16('z')))
			C2 += WCHAR16('A') - WCHAR16('a');

		if( C1 != C2)
			return C1 - C2;

		str1++;
		str2++;
		nSize++;
	}
	while((*str1 != 0) && (nSize < size));

	if(*str1 == 0)
		return *str1 - *str2;

	return 0;
#endif
}

INT32 S2String::CompareCase( const char* str1, const char* str2)
{
#ifdef USE_C_LIBRARY_FUNCS

	return strcmp( str1, str2 );

#else
	char C1, C2;

	do
	{
		C1 = *str1;
		C2 = *str2;

		if( C1 != C2)
			return C1 - C2;

		str1++;
		str2++;
	} while((*str1 != 0) && (*str2 != 0));

	C1 = *str1;
	C2 = *str2;

	return C1 - C2;
#endif
}

INT32 S2String::CompareCaseW( const WCHAR16 * str1, const WCHAR16 * str2 )
{
	WCHAR16 C1, C2;

	do
	{
		C1 = *str1;
		C2 = *str2;

		if( C1 != C2)
		{
			return C1 - C2;
		}

		str1++;
		str2++;
	}
	while( (*str1 != 0) && (*str2 != 0) );

	C1 = *str1;
	C2 = *str2;

	return C1 - C2;
}

// pStr 문자열 안에 ch가 있으면 그 위치를 반환한다.
// 앞에서 부터 찾는다.
// 없으면 -1 반환
INT32 S2String::ContainChar( const char * pStr, char ch )
{
	INT32 idx = 0;

	const char *pCur = pStr;

	for( pCur = pStr, idx = 0; *pCur != 0; pCur++, idx++ )
	{
		if( *pCur == ch )
		{
			return idx;
		}
	}

	return -1;
}

// pStr 문자열 안에 ch가 있으면 그 위치를 반환한다.
// 앞에서 부터 찾는다.
// 없으면 -1 반환
INT32 S2String::ContainChar( const wchar_t * pStr, wchar_t ch )
{
	INT32 idx = 0;

	const wchar_t *pCur = pStr;

	for( pCur = pStr, idx = 0; *pCur != 0; pCur++, idx++ )
	{
		if( *pCur == ch )
		{
			return idx;
		}
	}

	return -1;
}

// pStr 문자열 안에 ch가 있으면 그 위치를 반환한다.
// 뒤에서 부터 찾는다.
// 없으면 -1 반환
INT32 S2String::ContainCharRev( const char * pStr, char ch )
{
	INT32 nLen = Length( pStr );
	if( nLen <= 0 ) return -1;

	INT32 i;
	for( i = nLen - 1; i >= 0; i-- )
	{
		if( pStr[i] == ch )
		{
			return i;
		}
	}

	return -1;
}

INT32 S2String::Contain( const char * pszStr, const char * pszKey)
{
	INT32 Len;
	INT32 idxKey = 0, idxStr = 0, StartIdx = 0;

	Len = Length( pszKey);

	while( pszStr[ idxStr])
	{
		if( pszStr[idxStr] == pszKey[idxKey])
		{
			if( idxKey == 0)
				StartIdx = idxStr;

			idxKey++;

			if( idxKey >= Len)
				return StartIdx;
		}
		else
		{
			if( idxKey != 0 )
				idxStr--;

			idxKey = 0;
		}

		idxStr++;
	}

	return -1;
}

// BF-Search 와 같음..
// Contain 하고 있는 경우 0보다 크거나 같은값(starting index)을 return
INT32 S2String::ContainNew( const char* pStr1, const char* pStr2)
{
	INT32 i = 0;	// index of str1
	INT32 j = 0;	// index of str2
	
	INT32 nFirstCharIdx = 0;
	INT32 nLen = Length( pStr2);

	while( pStr1[i] != 0)
	{
		if( pStr1[i] == pStr2[j])
		{
			if( j == 0)
				nFirstCharIdx = i;
			
			if( j == nLen-1)
				return nFirstCharIdx;

			j++;
		}
		else
		{
			if( j != 0)
			{
				i = nFirstCharIdx;
				j = 0;
			}
		}

		i++;
	}

	return -1;
}

INT32 S2String::ContainNoCase( const char * pszStr, const char * pszKey)
{
	INT32 Len;
	INT32 idxKey = 0, idxStr = 0, StartIdx = 0;
	INT32 C1 = 0, C2 = 0;

	Len = Length( pszKey);

	while( pszStr[ idxStr])
	{
		C1 = pszStr[idxStr];
		C2 = pszKey[idxKey];

		if( C1 != C2)
		{		
			//	대소문자 구분 없앰.
			if((C1 >= 'A') && (C1 <= 'Z'))		
				C1 += 'a' - 'A';

			if((C2 >= 'A') && (C2 <= 'Z'))
				C2 += 'a' - 'A';
		}

		if( C1 == C2)
		{
			if( idxKey == 0)
				StartIdx = idxStr;

			idxKey++;

			if( idxKey >= Len)
				return StartIdx;
		}
		else
		{
			if( idxKey != 0 )
				idxStr --;

			idxKey = 0;
		}

		idxStr++;
	}

	return -1;
}

INT32 S2String::ContainNative( const char * pStr)
{
	INT32 Idx = 0;

	while( *pStr)
	{
		if( *pStr & 0x80)
		{
			return Idx;
		}

		Idx++;
		pStr++;
	}

	return -1;
}

INT32 S2String::ContainW( const WCHAR16 * pStr1, const WCHAR16 * pStr2)
{
	INT32 Len;
	INT32 Idx = 0, StartIdx = 0;

	Len = Length( pStr2);

	while( *pStr1)
	{
		if( *pStr1 == pStr2[Idx])
		{
			if( Idx == 0)
				StartIdx = Idx;

			Idx++;

			if( Idx >= Len)
				return StartIdx;
		}
		else
		{
			if( Idx != 0 )
			{
				pStr1--;
			}
			Idx = 0;
		}

		pStr1++;
	}

	return -1;
}

char	S2String::ToUpper(char c)
{
	if ( c >= 'a' && c <= 'z' )
		return static_cast<char>(c - ALPHABET_INTERVAL);
	return c;
}

void S2String::ToUpper( char * pStr)
{
	while( *pStr)
	{
		if((*pStr >= 'a') && (*pStr <= 'z'))
		{
			*pStr = *pStr - ('a' - 'A');
		}

		pStr++;
	}
}

// nCount이후부터 대문자로 바꾸는 함수
void S2String::ToUpperAfterN(char* pStr, INT32 nCount)
{
	INT32 i = 0;

	while(*pStr)
	{
		i++;
		if(i >= nCount)
		{
			break;
		}
	}
	
	ToUpper(pStr);
}

// nCount전까지 대문자로 바꾸는 함수
void S2String::ToUpperBeforeN(char* pStr, INT32 nCount)
{
	INT32 i = 0;
	
	while( *pStr)
	{
		if(i >= nCount)
		{
			break;
		}
		
		if((*pStr >= 'a') && (*pStr <= 'z'))
		{
			*pStr = *pStr - ('a' - 'A');
		}

		pStr++;
	}
}

char	S2String::ToLower(char c)
{
	if ( c >= 'A' && c <= 'Z' )
		return static_cast<char>(c + ALPHABET_INTERVAL);
	return c;
}

void	S2String::ToLower( char * pStr)
{
	while( *pStr)
	{
		if((*pStr >= 'A') && (*pStr <= 'Z'))
			*pStr += ALPHABET_INTERVAL;

		pStr++;
	}
}

void	S2String::ToLower( char * pDest, UINT32 DestSize, const char * pSrc)
{
	I3ASSERT(pDest != NULL);
	I3ASSERT(pSrc != NULL);
	I3ASSERT(DestSize != 0);
	if ( ! pDest || ! pSrc || DestSize == 0)
		return;


	UINT32 i = 0;
	while ( (*pSrc != '\0') && (i < DestSize) )
	{
		if ( *pSrc >= 'A' && *pSrc <= 'Z' )
			pDest[i] = static_cast<char>(*pSrc + ALPHABET_INTERVAL);
		else
			pDest[i] = *pSrc;

		++pSrc;
		++i;
	}

	if ( i > DestSize )
		pDest[DestSize-1] = NULL;
	else
		pDest[i] = NULL;
}

//void S2String::Copy( char * pDest, const char * pSrc)
//{
//	if( pDest == NULL || pSrc == NULL)
//		return;
//
//	while( *pSrc)
//	{
//		*pDest = *pSrc;
//
//		pDest++;
//		pSrc++;
//	}
//
//	*pDest = 0;
//}
//
//void S2String::Copy( char * pDest, INT32 DestSize, const char * pSrc)
//{
//	if ( DestSize <= 0 )
//		return;
//
//	while( *pSrc && --DestSize > 0 )
//	{
//		*pDest = *pSrc;
//
//		pDest++;
//		pSrc++;
//	}
//
//	*pDest = 0;
//}
//
//void S2String::CopyW( WCHAR16 * pDest, const WCHAR16 * pSrc)
//{
//	while( *pSrc)
//	{
//		*pDest = *pSrc;
//
//		pDest++;
//		pSrc++;
//	}
//
//	*pDest = 0;
//}
//
//void S2String::CopyW( WCHAR16 * pDest, INT32 DestSize, const WCHAR16 * pSrc)
//{
//	if ( DestSize <= 0 )
//	return;
//
//	while( *pSrc && --DestSize > 0 )
//	{
//		*pDest = *pSrc;
//
//		pDest++;
//		pSrc++;
//	}
//
//	*pDest = 0;
//}



void S2String::CopyASCIItoWCHAR( WCHAR16 * pDest, const char * pSrc )
{
	//while( *pSrc )
	//{
	//	*pDest = WCHAR16( *pSrc ); 
	//	pDest++;
	//	pSrc++;
	//}
	//*pDest = 0;

	INT32 length = Length( pSrc);
	INT32 wLength = MultiByteToWideChar( i3Language::GetCurrentCodePage(), 0, pSrc, length, NULL, 0);

	MultiByteToWideChar( i3Language::GetCurrentCodePage(), 0, pSrc, length, pDest, wLength);
	pDest[wLength] = 0;
}

void S2String::CopyWCHARtoASCII( char * pDest, INT32 nDestLength, const WCHAR16 * pSrc, INT32 nSrcLength)
{
	WideCharToMultiByte( i3Language::GetCurrentCodePage(), 0, pSrc, nSrcLength, pDest, nDestLength, NULL, NULL);
}

void S2String::NCopy( char * pDest, const char *pSrc, INT32 Length)
{
	INT32 Count = 0;

	while( *pSrc)
	{
		*pDest = *pSrc;

		pDest++;
		pSrc++;
		Count++;

		if( Count >= Length)
			break;
	}

	*pDest = 0;
}

void S2String::NCopy( WCHAR16 * pDest, const WCHAR16 *pSrc, INT32 Length )
{
	INT32 Count = 0;

	while( *pSrc)
	{
		*pDest = *pSrc;

		pDest++;
		pSrc++;
		Count++;

		if( Count >= Length)
			break;
	}

	*pDest = 0;
}

// UCS2 -> UTF8 encode
//
// UTF-8로 변환된 내용을 파일로 저장할려면, 해더로 3바이트를 써 주어야 됨.
// 0xEF 0xBB 0xBF
// 
int S2String::UCS2toUTF8( char *pDest, const WCHAR16 *pSrc, int nDestBufSize )
{
	if( pSrc == NULL ) return 0;

	int count = (int)( S2String::Length( pSrc ) );
	WCHAR16 unicode;
	unsigned char bytes[4];
	int nbytes;
	int len = 0;

	for( int i = 0; i < count; i++ )
	{
		unicode = pSrc[i];
		if ( unicode < 0x80 )
		{
			nbytes = 1;
			bytes[0] = (unsigned char) unicode;							//  0xxxxxxx
		}
		else if ( unicode < 0x0800 )
		{
			nbytes = 2;
			bytes[1] = (unsigned char) ((unicode & 0x3f) | 0x80);		// 10xxxxxx
			bytes[0] = (unsigned char) ((unicode >> 6) & 0x1f | 0xc0) ;	// 110xxxxx
		}
		else
		{
			nbytes = 3;
			bytes[2] = (unsigned char) ((unicode & 0x3f) | 0x80);		// 10xxxxxx
			bytes[1] = (unsigned char) ((unicode >> 6) & 0x3f | 0x80);	// 10xxxxxx
			bytes[0] = (unsigned char) ((unicode >> 12) & 0x0f | 0xe0);	// 1110xxxx
		}

		if( (len + nbytes) >= nDestBufSize )
		{
			I3WARN("Need more buffer size.\n");
			//I3TRACE( "[%s] : Need more buffer size.\n", __FUNCTION__ );
			I3ASSERT_0;
			break;
		}

		for( int j = 0; j < nbytes; j++ )
		{
			pDest[len] = bytes[j];
			len++;
		}
	}

	pDest[len] = 0x00;

	return len;
}

// UTF-8포맷을 유니코드 스트링으로 디코딩
//
// UTF-8로 변환된 내용을 파일로 저장할려면, 해더에 다음의 2바이트를 써 주어야 됨.
// 0xFF 0xFE
//
int S2String::UTF8toUCS2( WCHAR16 *pDest, const char *_pSrc, int nDestBufSize )
{
	unsigned char *pSrc = ( unsigned char * )_pSrc;
	unsigned char c;
	WCHAR16 unicode;
	int count = 0;
	int i = 0;
	//int len = Length( pSrc );
	int len = (int)strlen( _pSrc );

	for( i = 0; i < len; )
	{
		if( ( count + 1 ) >= nDestBufSize )
		{
			I3WARN("Need more buffer size.\n");
			//I3TRACE( "[%s] : Need more buffer size.\n", __FUNCTION__ );
			I3ASSERT_0;
			break;
		}

		c = pSrc[i] & 0xe0;

		if( c < 0x80 )
		{
			unicode = WCHAR16( pSrc[i] );
			i++;
		}
		else if ( c < 0xe0 )
		{
			unicode =	WCHAR16( pSrc[i] & 0x1f );
			i++;
			unicode =	unicode << 6;
			unicode |=	WCHAR16( pSrc[i] & 0x3f );
			i++;
		}
		else if ( c < 0xf0 )
		{
			unicode =	WCHAR16( pSrc[i] & 0x0f );
			i++;
			unicode =	unicode << 6;
			unicode |=	WCHAR16( pSrc[i] & 0x3f );
			i++;
			unicode =	unicode << 6;
			unicode |=	WCHAR16( pSrc[i] & 0x3f );
			i++;
		}
		else
		{
			I3WARN("Invalid character.\n");
			//I3TRACE( "[%s] : Invalid character.\n", __FUNCTION__ );
			I3ASSERT_0;
			break;
		}

		pDest[count] = unicode;
		count++;
	}

	pDest[count] = 0x0000;

	return count;
}

void S2String::ftoa( REAL32 val, char * pszStr, INT32 fractLen)
{
	char * pszTemp;

	if( fractLen == -1)
	{
		sprintf( (PSTR)pszStr, (PSTR)"%f", val);
	}
	else
	{
		char format[64];

		sprintf( format, "%%.%df", fractLen);
		sprintf( pszStr, format, val);
	}

	pszTemp = pszStr;
	while( *pszTemp)
		pszTemp++;

	do
	{
		pszTemp--;

		if( *pszTemp == '0')
			*pszTemp = 0;

	} while( *pszTemp == 0);

	if( *pszTemp == '.')
		*pszTemp = 0;
}


/// szIn과 같은 내용의 문자열을 생성해서 리턴한다.
char* S2String::NewString( const char* szIn )
{
	I3ASSERT( szIn != NULL );
	INT32 nLen = Length( szIn );
	I3ASSERT( nLen >= 0 );
	
	char *szOut = (char *)i3MemAlloc( nLen + 1 );
	I3ASSERT( szOut != NULL );

#if 0
	{
		// 메모리 누수 탐지
		void *pCheck = szOut;
		static int nCount = 0;
		static int nCheck = -1;
		I3ASSERT( nCheck != nCount );
		I3WARN("%d, %p\n", nCount, pCheck );
		//I3TRACE( __FUNCTION__ " : %d, %p\n", nCount, pCheck );
		nCount++;
	}
#endif

	Copy( szOut, szIn, nLen + 1 );

	return szOut;
}

void S2String::ResolvePathToken( const char * pszPath, S2StringList * pList)
{
	char * pTemp;
	char szBuf[MAX_PATH];
	INT32 i;

	for( i = 0, pTemp = (char *) pszPath; *pTemp != 0; pTemp++)
	{
		if( *pTemp & 0x80)
		{
			szBuf[i] = *pTemp;
			i++;
			pTemp ++;
		}
		else
		{
			if((*pTemp == '\\') || (*pTemp == '/'))
			{
				szBuf[i] = 0;
				pList->Add( szBuf);
				
				i = 0;
				continue;
			}
		}

		szBuf[i] = *pTemp;
		i++;
	}

	if( i > 0)
	{
		szBuf[i] = 0;
		pList->Add( szBuf);
	}
}

void S2String::MakeRelativePath( const char * pszBasePath, const char * pszAbsPath, char * pszRelPath, const INT32 nRelPathMax)
{
	S2StringList BaseToken, AbsToken;
	char * pszBase, *pszAbs;
	INT32 sidx, didx;
	bool bProcess, bDiff = false;

	ResolvePathToken( pszBasePath, &BaseToken);
	ResolvePathToken( pszAbsPath, &AbsToken);

	if( BaseToken.GetCount() <= 0)
	{
		S2String::Copy( pszRelPath, pszAbsPath, nRelPathMax);
		return;
	}

	if( AbsToken.GetCount() <= 0)
		return;

	#if defined( I3_WINDOWS)
		// Drive 명을 우선 확인한다.
		// 만약 다른 Drive라면 아무런 처리 없이 그냥 Relative Path에 저장하고
		// 반환한다.
		if( S2String::Compare( BaseToken.Items[0], AbsToken.Items[0]) != 0)
		{
			S2String::Copy( pszRelPath, pszAbsPath, nRelPathMax);
			return;
		}
	#endif

	sidx = 0;
	didx = 0;
	pszRelPath[0] = 0;

	while( didx < AbsToken.GetCount())
	{
		bProcess = true;

		pszAbs = AbsToken.Items[didx];

		if( sidx < BaseToken.GetCount())
		{
			pszBase = BaseToken.Items[sidx];

			if( (bDiff == false) && (S2String::Compare( pszBase, pszAbs) == 0))
			{
				bProcess = false;
				didx++;
			}
			else
			{
				pszAbs = "..";
				bDiff = true;
			}

			sidx++;
		}
		else
		{
			didx++;
		}

		if( bProcess)
		{
			S2String::Concat( pszRelPath, pszAbs);

			if( didx < AbsToken.GetCount())
				S2String::Concat( pszRelPath, "\\");
		}
	}
}

void S2String::MakeCStylePath( char * pszDest, const char * pszSrc)
{
	I3ASSERT( pszDest != NULL);
	I3ASSERT( pszSrc != NULL);

	while( *pszSrc != 0)
	{
		*pszDest = *pszSrc;

		if( *pszSrc == '\\')
		{
			pszDest++;
			*pszDest = '\\';
		}

		pszDest++;
		pszSrc++;
	}

	*pszDest = 0;
}

enum ATOI_MODE
{
	ATOI_DECIMAL = 0,
	ATOI_HEXA,
};

INT32 S2String::ToInt( const char * pszStr)
{
	ATOI_MODE mode = ATOI_DECIMAL;
	INT32 rv = 0, sign = 1;
	char ch;

	if((pszStr[0] == '0') && ((pszStr[1] == 'x') || (pszStr[1] == 'X')))
	{
		mode = ATOI_HEXA;
		pszStr += 2;
	}

	while( * pszStr)
	{
		switch( *pszStr)
		{
			case '-' :	sign = -1;	break;
			case '+' :				break;
			default :

				if( mode == ATOI_DECIMAL)
				{
					if( (*pszStr >= '0') && (*pszStr <= '9'))
					{
						rv *= 10;
						rv += *pszStr - '0';
					}
					else
						return 0;
				}
				else
				{
					rv <<= 4;

					if( (*pszStr >= '0') && (*pszStr <= '9'))
					{
						rv += *pszStr - '0';
					}
					else 
					{
						if( (*pszStr >= 'a') && (*pszStr <= 'f'))
							ch = *pszStr - 'a';
						else if((*pszStr >= 'A') && (*pszStr <= 'F'))
							ch = *pszStr - 'A';
						else
							return 0;

						rv += ch + 10;
					}
				}
				break;
		}

		pszStr++;
	}

	return rv * sign;
}

static I3_SYMBOLTABLE_INFO *	_FindSymbol( I3_SYMBOLTABLE_INFO * pTable, INT32 symbolCount, char * pszStr)
{
	INT32 i;

	for( i = 0; i < symbolCount; i++)
	{
		if( S2String::Compare( pTable[i].m_szSymbol, pszStr) == 0)
		{
			return & pTable[i];
		}
	}

	return NULL;
}

I3_EXPORT_BASE 
void	S2String::PreprocessMacro( char * pszDest, INT32 Length, const char * pszSrc, I3_SYMBOLTABLE_INFO * pTable, INT32 symbolCount)
{
	char * pszTemp = (char *) pszSrc;
	char szSym[65];
	INT32 symAccm = 0;
	bool bInSymbol = false;
	I3_SYMBOLTABLE_INFO * pSymbol;

	while( *pszTemp != 0)
	{
		switch( *pszTemp)
		{
			case '%' :
				if( bInSymbol)
				{
					if( symAccm == 0)
					{
						// %% 의 경우
						*pszDest = '%';
						pszDest++;
					}
					else
					{
						szSym[ symAccm] = 0;
						symAccm = 0;

						pSymbol = _FindSymbol( pTable, symbolCount, szSym);

						if( pSymbol != NULL)
						{
							INT32 len = (INT32)strlen( pSymbol->m_szValue);

							memcpy( pszDest, pSymbol->m_szValue, len);
							pszDest += len;
						}
						else
						{
							I3TRACE( "올바르지 않은 Symbol : %s\n", szSym);
						}
					}

					bInSymbol = false;
				}
				else
				{
					bInSymbol = true;
					symAccm = 0;
				}
				break;

			default :
				if( bInSymbol)
				{
					szSym[ symAccm ] = *pszTemp;
					symAccm ++;
				}
				else
				{
					*pszDest = *pszTemp;
					pszDest++;
				}
				break;
		}

		pszTemp++;
	}

	*pszDest = 0;
}

bool _scanWildcard( char * pszName, char * pszRull)
{
	while( (*pszRull != 0) && (*pszName != 0))
	{
		switch( *pszRull)
		{
			case '*' :			return true;

			case '?' :
				break;

			default :
				if( toupper( *pszName) != toupper( *pszRull))
					return false;
				break;
		}

		pszRull++;
		pszName++;
	}

	if( *pszRull != *pszName)
		return false;

	return true;
}

I3_EXPORT_BASE
bool S2String::IsMatchedWildcard( char * pszFileName, char * pszRull)
{
	char	szRullName[ MAX_PATH], szRullExt[ MAX_PATH];
	char	szName[ MAX_PATH], szExt[ MAX_PATH];

	S2String::SplitFileName( pszRull, szRullName, FALSE);
	S2String::SplitFileExt( pszRull, szRullExt, sizeof(szRullExt));

	S2String::SplitFileName( pszFileName, szName, FALSE);
	S2String::SplitFileExt(	 pszFileName, szExt, sizeof(szExt) - 1);

	if( szRullName[0] != 0)
	{
		if( _scanWildcard( szName, szRullName) == false)
			return false;
	}

	if( szRullExt[0] != 0)
	{
		if( _scanWildcard( szExt, szRullExt) == false)
			return false;
	}

	return true;
}

I3_EXPORT_BASE
void S2String::GetPrefixName( char * pszDest, const  char * pszStr)
{
	INT32 i;

	// 우선 문자열의 복사
	for( i = 0; pszStr[i] != 0; i++)
	{
		pszDest[i] = pszStr[i];
	}

	while( isdigit( pszDest[i - 1]))
	{
		// 이전 문자가 digit 문자라면 앞으로 이동한다.
		i--;
	}

	// 현재 i의 이전 문자는 digit 문자가 아니기때문에
	// 끝을 표시한다.
	pszDest[i] = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// 문자열에 포함된 특정 문자열을 변경한다.
// (영문일 경우 대소문자 구분함.)
//
// char *str	: 특정 문자를 변경할 문자열의 시작 포인터
// char *before	: 변경전 문자열
// char *affer	: 변경후 문자열
//
// Return		: 변경 성공 여부
//
// 작성자		: 백성민
// 작성일자		: 2008.8.22
I3_EXPORT_BASE
BOOL S2String::Replace(char * str, INT32 bufLen, const char * before, const char * affer)
{
	const INT32 cbufMax = 4096;
	char buf[cbufMax] = "", * pbuf = buf;
	unsigned char * p = (unsigned char*) str;
	unsigned char * bf = (unsigned char*) before;
	unsigned char * af = (unsigned char*) affer;
	INT32 len = 0;
	BOOL bResult = FALSE;

	if( str == NULL || *str == 0) return FALSE;
	if( before == NULL || *before == 0) return FALSE;
	if( affer == NULL || *affer == 0) return FALSE;

	INT32 maxLen = MIN( bufLen, cbufMax);

	while( *p) 
	{
		//	같은 문자열을 찾는다
		if( *p == *bf)
		{
			INT32 count = 0;
			unsigned char * c = p;

			//	문자열을 비교해본다.
			while( *c != 0 && *c == *bf)
			{
				bf++;
				c++;
				count++;
			}		

			//	찾아야할 모든 문자열 검색이 끝났다.
			if( *bf == 0)
			{				
				af = (unsigned char*) affer;

				//	변경할 문자열로 교체한다.
				while( *af)
				{
					*(pbuf++) = *(af++);

					len++;

					//	버퍼가 가득찼으므로 진행 종료
					if( len >= maxLen-1)
					{
						*pbuf = 0;
						goto LABEL_EXIT;
					}
				}

				p = c;	//	교체후 포인터 점프.	

				bResult = TRUE;
			}
			//	비교 실패. 원본 그대도 복사
			else
			{
				for(INT32 i=0; i < count; i++)
				{
					*(pbuf++) = *(p++);
					len++;

					//	버퍼가 가득찼으므로 진행 종료
					if( len >= maxLen-1)
					{
						*pbuf = 0;
						goto LABEL_EXIT;
					}
				}
			}

			//	다음으로 계속 진행되도록
			bf = (unsigned char*) before;
		}
		//	다른 문자다
		else
		{
			*(pbuf++) = *(p++);
			len++;

			//	버퍼가 가득찼으므로 진행 종료
			if( len >= maxLen-1)
			{
				*pbuf = 0;
				goto LABEL_EXIT;
			}		
		}
	}

LABEL_EXIT:

	//	최종 변경된것 복사
	if( buf[0] != 0)
	{
		len = MIN( len, maxLen-1);

        S2String::NCopy( str, buf, len);

		str[len] = 0;
	}

	return bResult;
}

I3_EXPORT_BASE
BOOL S2String::ReplaceNoCase(char * str, INT32 bufLen, const char * before, const char * affer)
{
	const INT32 cbufMax = 4096;
	char buf[cbufMax] = "", * pbuf = buf;
	unsigned char * p = (unsigned char*) str;
	unsigned char * bf = (unsigned char*) before;
	unsigned char * af = (unsigned char*) affer;
	INT32 len = 0;
	BOOL bResult = FALSE;

	if( str == NULL || *str == 0) return FALSE;
	if( before == NULL) return FALSE;
	if( affer == NULL) return FALSE;

	INT32 maxLen = MIN( bufLen, cbufMax);

	while( *p) 
	{
		//	같은 문자열을 찾는다
		if( toupper(*p) == toupper(*bf))
		{
			INT32 count = 0;
			unsigned char * c = p;

			//	문자열을 비교해본다.
			while( *c != 0 && (toupper(*c) == toupper(*bf)))
			{
				bf++;
				c++;
				count++;
			}		

			//	찾아야할 모든 문자열 검색이 끝났다.
			if( *bf == 0)
			{				
				af = (unsigned char*) affer;

				//	변경할 문자열로 교체한다.
				while( *af)
				{
					*(pbuf++) = *(af++);

					len++;

					//	버퍼가 가득찼으므로 진행 종료
					if( len >= maxLen-1)
					{
						*pbuf = 0;
						goto LABEL_EXIT;
					}
				}

				p = c;	//	교체후 포인터 점프.	

				bResult = TRUE;
			}
			//	비교 실패. 원본 그대도 복사
			else
			{
				for(INT32 i=0; i < count; i++)
				{
					*(pbuf++) = *(p++);
					len++;

					//	버퍼가 가득찼으므로 진행 종료
					if( len >= maxLen-1)
					{
						*pbuf = 0;
						goto LABEL_EXIT;
					}
				}
			}

			//	다음으로 계속 진행되도록
			bf = (unsigned char*) before;
		}
		//	다른 문자다
		else
		{
			*(pbuf++) = *(p++);
			len++;

			//	버퍼가 가득찼으므로 진행 종료
			if( len >= maxLen-1)
			{
				*pbuf = 0;
				goto LABEL_EXIT;
			}		
		}
	}

LABEL_EXIT:

	//	최종 변경된것 복사
	if( buf[0] != 0)
	{
		len = MIN( len, maxLen-1);

        S2String::NCopy( str, buf, len);

		str[len] = 0;
	}

	return bResult;
}



/////////////////////////////////////////////////////////////////////////////////////////////
//	THAI language -- 소스는 gpgstudy에서 가져옴 (2008.7.31 komet)
/////////////////////////////////////////////////////////////////////////////////////////////

// Unicode of thai : 0x0E00~0x0E7F (Direct Mapping)
// Thai Character -> Unicode    (char+0x0E00)-0xA0 =    char+0x0D60
// Unicode -> Thai Character (wchar_t-0x0E00)+0xA0 = wchar_t-0x0D60

enum THAI_CLASS		//	Thai character classification
{	
	CTRL= 0,		//	Control characters
	NON= 1,			//	Non-composibles
	CONS= 2,		//	Consonants
	LV= 3,			//	Leading Vowels
	FV1= 4,			//	Following Vowels
	FV2= 5,
	FV3= 6,
	BV1= 7,			//	Below Vowels
	BV2= 8,
	BD= 9,			//	Below Diacritic
	TONE=10,		//	Tonemarks
	AD1=11,			//	Above Diacritics
	AD2=12,
	AD3=13,
	AV1=14,			//	Above Vowels
	AV2=15,
	AV3=16,
};

THAI_CLASS 
THAI_CLASS_TABLE[256] = 
{
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, CTRL, 
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
	CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
	NON, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, 
	CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, 
	CONS, CONS, CONS, CONS,  FV3, CONS,  FV3, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS,  NON, 
	FV1,  AV2,  FV1,  FV1,  AV1,  AV3,  AV2,  AV3,  BV1,  BV2,   BD,  NON,  NON,  NON,  NON,  NON, 
	LV,   LV,   LV,   LV,   LV,  FV2,  NON,  AD2, TONE, TONE, TONE, TONE,  AD1,  AD1,  AD3,  NON, 
	NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, CTRL, 
};

enum THAI_INPUT_TYPE { IA, IC, IS, IR, IX };

BOOL THAI_INPUT_COMPOSIBLE[3][5] = {
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ TRUE, TRUE, TRUE,FALSE, TRUE },
	{ TRUE, TRUE,FALSE,FALSE, TRUE },
};

THAI_INPUT_TYPE 
THAI_INPUT_STATE_CHECK[17][17] = {
	{ IX,IA,IA,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IA,IS,IA,IC,IC,IC,IC,IC,IC,IC,IC,IC,IC },
	{ IX,IS,IA,IS,IS,IS,IS,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IS,IA,IS,IA,IS,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IA,IS,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IA,IS,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IA,IS,IA,IR,IR,IR,IC,IC,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IC,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IA,IA,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IR,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IC,IC,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IC,IR,IR,IR,IR,IR,IR },
	{ IX,IA,IA,IA,IS,IS,IA,IR,IR,IR,IC,IR,IC,IR,IR,IR,IR },
};

enum THAI_OUTPUT_TYPE { ON, OC, OX };

THAI_OUTPUT_TYPE 
THAI_OUTPUT_STATE_CHECK[17][17] = {
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,OC,OC,OC,OC,OC,OC,OC,OC,OC,OC,OC,OC,OC },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,OC,OC,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,OC,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,OC,OC,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,OC,ON,ON,ON,ON,ON,ON },
	{ OX,ON,ON,ON,ON,ON,ON,ON,ON,ON,OC,ON,OC,ON,ON,ON,ON },
};

I3_EXPORT_BASE
BOOL S2String::IsComposite(INT32 nCodePage, const char prev, const char curr, INT32 mode)
{
	switch( nCodePage)
	{
	case I3_LANG_CODE_THAI:	//	THAI
		{
			if( mode > 2)	
				return FALSE;
			else 
			{
				BYTE prev_stream = (BYTE)prev;

				BYTE curr_stream = (BYTE)curr;

				THAI_CLASS prev_class = THAI_CLASS_TABLE[prev_stream];
				I3ASSERT( prev_class >= 0 && prev_class < 17);

				THAI_CLASS curr_class = THAI_CLASS_TABLE[curr_stream];
				I3ASSERT( curr_class >= 0 && curr_class < 17);

				THAI_INPUT_TYPE state_type = THAI_INPUT_STATE_CHECK[prev_class][curr_class];

				return THAI_INPUT_COMPOSIBLE[mode][state_type];
			}
		}
		break;
	default:
		I3ASSERT_0;
		break;
	}

	return FALSE;
}

I3_EXPORT_BASE
INT32 S2String::LengthChar(INT32 nCodePage, const char * pszStr)
{
	I3ASSERT( pszStr != NULL && nCodePage != 0 && nCodePage != 1);

	INT32 nCount = 0;
	const char * pszCurr = pszStr;
	while( *pszCurr)
	{
		pszCurr = CharNext(nCodePage, pszCurr);
		nCount++;
	}	

	return nCount;
}

I3_EXPORT_BASE
const char * S2String::CharNext(INT32 nCodePage, const char * pszStr)
{
	I3ASSERT( pszStr != NULL && nCodePage != 0 && nCodePage != 1);

	switch( nCodePage)
	{
	case I3_LANG_CODE_THAI:		//	THAI			태국은 조합형 MBCS 체계 (1~4바이트 가변 바이트 ) 주의! 태국어는 2바이트가 아니다!
		{
			const BYTE * stream = (const BYTE*) pszStr;
			
			for(;;)
			{
				THAI_CLASS c1 = THAI_CLASS_TABLE[ stream[0]];
				I3ASSERT( c1 >= 0 && c1 < 17);

				THAI_CLASS c2 = THAI_CLASS_TABLE[ stream[1]];			
				I3ASSERT( c2 >= 0 && c2 < 17);

				if( THAI_OUTPUT_STATE_CHECK[c1][c2] == OC)
					++stream;
				else
					break;
			}

			return (const char*)(stream+1);
		}
		break;
	case I3_LANG_CODE_KOREAN:					//	HANGUL			한글 
	case I3_LANG_CODE_JAPANESE:					//	SHIFTJIS		일본어
	case I3_LANG_CODE_CHINESE_SIMPLIFIED:		//	GB2312			중국어 간체
	case I3_LANG_CODE_CHINESE_TRADITIONAL:		//	CHINESEBIG5		중국어 번체
		{
#if I3_WINDOWS
			return ::CharNextExA((WORD) nCodePage, pszStr, 0);
#else
			//	CJK(한중일) 언어는 DBCS 시스템 (2바이트) 
			if( *pszStr & 0x80)		//	lead byte		ascii 영역은 0 ~ 127(0x7E) 이며 0x80 부터 극동 언어(DBCS)로 인식된다.
			{
				pszStr++;
			}

			return (*pszStr != 0 ? pszStr++ : pszStr);
#endif
		}
		break;
	default:
		{
#if I3_WINDOWS
			return ::CharNextExA((WORD) nCodePage, pszStr, 0);
#else
			return pszStr++;		//	코드 페이지가 정의되지 않았다.
#endif
		}
		break;
	}
}

I3_EXPORT_BASE
const char * S2String::CharPrev(INT32 nCodePage, const char * pszStart, const char * pszCurrent)
{
	I3ASSERT( pszStart != NULL && pszCurrent != NULL && nCodePage != 0 && nCodePage != 1);

	switch( nCodePage)
	{
	case I3_LANG_CODE_THAI:		//	THAI			태국은 조합형 MBCS 체계 (1~4바이트 가변 바이트 )
		{
			while( pszStart < pszCurrent)
			{
				const char * pszNext = CharNext(nCodePage, pszStart);

				if( pszNext == pszCurrent)
					return pszStart;
				else if( pszNext < pszCurrent)
					pszStart = pszNext;
				else
					break;
			}

			return pszStart;
		}
		break;
	case I3_LANG_CODE_KOREAN:					//	HANGUL			한글 
	case I3_LANG_CODE_JAPANESE:					//	SHIFTJIS		일본어
	case I3_LANG_CODE_CHINESE_SIMPLIFIED:		//	GB2312			중국어 간체
	case I3_LANG_CODE_CHINESE_TRADITIONAL:		//	CHINESEBIG5		중국어 번체
		{
#if I3_WINDOWS
			return ::CharPrevExA((WORD) nCodePage, pszStart, pszCurrent, 0);
#else
			while( pszStart < pszCurrent)
			{
				const char * pszNext = CharNext(nCodePage, pszStart);

				if( pszNext < pszCurrent) 
					pszStart = pszNext;
				else
					return pszNext;
			}

			return pszStart;
#endif
		}
		break;
	default:
		{
#if I3_WINDOWS
			return ::CharPrevExA((WORD) nCodePage, pszStart, pszCurrent, 0);
#else
			return pszStr;		//	코드 페이지가 정의되지 않았다.
#endif
		}
		break;
	}
}




//Brute Force algorithm
//
//http://www-igm.univ-mlv.fr/~lecroq/string/node3.html#SECTION0030
//
//Features
//- no preprocessing phase. 
//- constant extra space needed. 
//- always shifts the window by exactly 1 position to the right. 
//- comparisons can be done in any order. 
//- searching phase in O(mn) time complexity. 
//- 2n expected text characters comparisons.

INT32 BF_search(char *dest, char *src)
{
	INT32 i = 0,j = 0;

	if( dest == NULL || src == NULL)
		return -1;

	INT32 srcLen = S2String::Length(src);
	INT32 destLen = S2String::Length(dest);

	 if( srcLen == 0 || destLen == 0)
		 return -1;

	for(j=0; j<=destLen-srcLen; j++)
	{
		for(i=0; i<srcLen && src[i] == dest[i+j]; i++);

		if( i >= srcLen)
			return j;
	}

	return -1;
}


//Knuth-Morris-Pratt algorithm
//
//http://www-igm.univ-mlv.fr/~lecroq/string/node8.html#SECTION0080
//
//Features
//- performs the comparisons from left to right. 
//- preprocessing phase in O(m) space and time complexity. 
//- searching phase in O(n+m) time complexity (independent from the alphabet size). 
//- delay bounded by log(m) where  is the golden ratio (  ).

INT32 * KMP_init(char *src)
{
	INT32 i=0, j=0;
	INT32 srcLen=S2String::Length(src);

	INT32 * next = (INT32*) i3MemAlloc( sizeof(INT32) * (srcLen+1));

	next[0] = -1;
	
	for(i=0, j=-1; i<srcLen; i++, j++, next[i] = j)
	{
		while((j>=0) && (src[i] != src[j]))
			j= next[j];
	}	
	
	return next;
}

INT32 KMP_search(char *dest, char *src)
{
	INT32 i=0, j=0;

	if( dest == NULL || src == NULL)
		return -1;

	INT32 destLen = S2String::Length(dest);
	INT32 srcLen = S2String::Length(src);

	if( srcLen == 0 || destLen == 0)
		return -1;

	//	Preprocessing
	INT32 * next = KMP_init( src);
	I3ASSERT( next != NULL);

	//	searching
	for( i=0, j=0; j<srcLen && i<destLen; i++, j++)
	{
		while((j >= 0) && (dest[i] != src[j])) 
			j = next[j];
	}

	if( next)
	{
		I3MEM_SAFE_FREE( next);
	}

	if( j == srcLen)	return i-srcLen;
	else				return -1;
}

//Boyer-Moore algorithm
//
//http://www-igm.univ-mlv.fr/~lecroq/string/node14.html#SECTION0014
//
//Features
//- performs the comparisons from right to left. 
//- preprocessing phase in O(m+σ) time and space complexity. 
//- searching phase in O(mn) time complexity. 
//- 3n text character comparisons in the worst case when searching for a non periodic pattern. 
//- O(n / m) best performance.
//
//ascii 검색만 fast 알고리즘이 작동하며 멀티바이트는 일반 선형 검색 방식으로 작동.
//내부서 메모리 할당이 일어나므로 잦은 검색 또는 단문의 검색은 오히려 S2String::Contain 보다 느리다.
//짧은 내용의 문자 검색보다는 긴 장문의 내용을 검색할수록 여타 알고리즘보다 성능이 향상된다.

#define IS_ASCII_CHAR(ch)		(ch>=0 && ch<256)

// bad-character shift array
void preBmBc( char *src, INT32 srcLen, INT32 * bmBc, INT32 bmBcLen)
{
	INT32 i=0;

	for( i=0; i<bmBcLen; i++)
	{
		bmBc[i] = srcLen;
	}

	for( i=0; i<srcLen-1; i++)
	{
		// ascii code 내에 있는 것만 처리
        // multi byte 등은 그대로 둠
		if( IS_ASCII_CHAR( src[i]))
			bmBc[ src[i]] = srcLen-1-i;
	}
}

//	brief make suffix for good-suffix shift array
void suffixes( char * src, INT32 srcLen, INT32 *suff)
{
	INT32 f=0,g=0,i=0;

	suff[ srcLen-1] = srcLen;

	g = srcLen-1;

	for(i=srcLen-2; i>=0; i--)
	{
		if( i>g && suff[ i+srcLen-1-f] < i-g)
		{
			suff[i] = suff[ i+srcLen-1-f];
		}
		else
		{
			if( i<g)	g = i;

			f=i;
		
			while( g>=0 && src[g] == src[ g+srcLen-1-f])
				--g;

			suff[i] = f-g;
		}
	}
}

// make good-suffix shift array
void preBmGs( char* src, INT32 srcLen, INT32 * bmGs, INT32 bmGsLen)
{
	INT32 i=0,j=0;
	INT32 * suff = (INT32*) i3MemAlloc( sizeof(INT32) * (srcLen+1));
	
	I3ASSERT( suff != NULL);

	// make suffix
	suffixes( src, srcLen, suff);

	for( i=0; i<srcLen; i++)
	{
		bmGs[ i] = srcLen;
	}

	for( i=srcLen-1; i >= 0; i--)
	{
		if( suff[ i] == i+1)
		{
			for( ; j<srcLen-1-i; j++)
			{
				if( bmGs[ j] == srcLen)
				{
					bmGs[ j] = srcLen-1-i;
				}
			}
		}
	}

	for( i=0; i<=srcLen-2; i++)
	{
		bmGs[ srcLen-1-suff[i]] = srcLen-1-i;
	}

	I3MEM_SAFE_FREE( suff);
}

INT32 BM_search( char * dest, char* src)
{
	INT32 i=0,j=0;
	BOOL find = FALSE;

	if( dest == NULL || src == NULL)
		return -1;

	INT32 srcLen = S2String::Length( src);
	INT32 destLen = S2String::Length( dest);

	if( srcLen == 0 || destLen == 0)
		return -1;

	INT32 * bmGs = (INT32*) i3MemAlloc( sizeof( INT32) * (srcLen+1));
	INT32 bmBc[256];	//	 ascii code 크기인 256이다. 

	//	Preprocessing	
	preBmGs( src, srcLen, bmGs, (srcLen+1));	// make good-suffix shift array	
	preBmBc( src, srcLen, bmBc, 256);			// make bad-character shift

	//	Searching
	while( j<= destLen - srcLen)
	{
		for( i=srcLen-1; i>=0 && src[i] == dest[i+j]; i--);

		if(i<0)
		{
			find = TRUE;

			goto _EXIT;
			//j+=bmGs[0];
		}
		else
		{
			INT32 skip = 1;

			// ascii code 내에 있는 것만 처리
			if( IS_ASCII_CHAR( dest[i+j]))
			{
				skip = bmBc[ dest[i+j]];
			}

			j+=MAX( bmGs[i], skip - srcLen + 1 + i);
		}
	}


_EXIT:

	if( bmGs)
	{
		I3MEM_SAFE_FREE( bmGs);
	}

	if( find)
		return j;
	else
		return -1;
}


//Quick Search algorithm
//
//http://www-igm.univ-mlv.fr/~lecroq/string/node19.html#SECTION00190
//
//Features
//- simplification of the Boyer-Moore algorithm. 
//- uses only the bad-character shift. 
//- easy to implement. 
//- preprocessing phase in O(m+σ) time and O(σ) space complexity. 
//- searching phase in O(mn) time complexity. 
//- very fast in practice for short patterns and large alphabets.
//
//Boyer Moore algorithm과 마찬가지로 ascii 검색만 fast 알고리즘이 작동하며 멀티바이트는 일반 선형 검색 방식으로 작동.

void preQsBc( char* src, INT32 srcLen, INT32 * qsBc, INT32 qsBcLen)
{
	INT32 i=0;

	for( i=0; i<qsBcLen; i++)
	{
		qsBc[i] = srcLen + 1;
	}

	for( i=0; i<srcLen; i++)
	{
		// ascii code 내에 있는 것만 처리
        // multi byte 등은 그대로 둠
		if( IS_ASCII_CHAR( src[i]))
			qsBc[ src[i]] = srcLen - i;
	}
}

INT32 Q_search( char* dest, char* src)
{
	INT32 j=0;
	INT32 qsBc[256];	//	 ascii code 크기인 256이다.

	if( dest == NULL || src == NULL)
		return -1;

	INT32 srcLen = S2String::Length(src);
	INT32 destLen = S2String::Length(dest);

	if( srcLen == 0 || destLen == 0)
		return -1;

	//	Preprocessing
	preQsBc( src, srcLen, qsBc, 256);

	//	Searching
	while( j <= destLen-srcLen)
	{
		//	find
#if defined (I3_WINDOWS)
		if( memcmp( src, dest+j, srcLen) == 0)
			return j;
#else
		if( S2String::NCompare( src, dest+j, srcLen) == 0)
			return j;
#endif
			
		
		INT32 skip = 1;

		// ascii code 내에 있는 것만 처리
		if( IS_ASCII_CHAR( dest[j+srcLen]))
		{
			skip = qsBc[ dest[j+srcLen]];
		}

		//	skip string
		j+=skip;
	}

	return -1;
}


//Rabin-Karp algorithm
//
//http://www-igm.univ-mlv.fr/~lecroq/string/node5.html#SECTION0050
//
//Features
//- uses an hashing function. 
//- preprocessing phase in O(m) time complexity and constant space. 
//- searching phase in O(mn) time complexity. 
//- O(n+m) expected running time. 

#define REHASH(a, b, h)		((((h)-(a)*d)<<1)+(b))

INT32 RK_search( char* dest, char* src)
{
	INT32 d=0, hx=0, hy=0, i=0, j=0;

	if( dest == NULL || src == NULL)
		return -1;

	INT32 srcLen = S2String::Length(src);
	INT32 destLen = S2String::Length(dest);

	if( srcLen == 0 || destLen == 0)
		return -1;

	//	Preprocessing
	for(d=i=1; i<srcLen; i++)
		d = (d<<1);

	for( hy=hx=i=0; i<srcLen; i++)
	{
		hx = ((hx<<1) + src[i]);
		hy = ((hy<<1) + dest[i]);
	}

	//	Searching
	while( j <= destLen-srcLen)
	{
#if defined (I3_WINDOWS)
		if( hx==hy && memcmp(src, dest+j, srcLen) == 0)
			return j;
#else
		if( hx==hy && S2String::NCompare( src, dest+j, srcLen) == 0)
			return j;
#endif		

		hy = REHASH( dest[j], dest[j+srcLen], hy);
		j++;
	}

	return -1;
}

I3_EXPORT_BASE INT32 S2String::Search(char * pszDest, char * pszSrc, ALGORITHM type)
{
	switch( type)
	{
	case BF:	return BF_search( pszDest, pszSrc);	
	case KMP:	return KMP_search( pszDest, pszSrc);
	case BM:	return BM_search( pszDest, pszSrc);
	case QUICK:	return Q_search( pszDest, pszSrc);
	case RK:	return RK_search( pszDest, pszSrc);
	}

	return -1;
}


I3_EXPORT_BASE bool S2String::NReplace( char* pszSrc, INT32 nStart, INT32 nEnd, INT32 nSrcSize, const char* pszDest, INT32 nLen)
{
	// not enough buffer
	if( nSrcSize <= Length(pszSrc) + nLen - (nEnd-nStart+1))
		return false;
	
	// copies back part of pszSrc
	INT32 nBufSize = nSrcSize - nEnd - 1;
	char* pBuf = new char[nBufSize];
	NCopy( pBuf, &pszSrc[nEnd+1], nBufSize);

	// move to nStart
	INT32 i = nStart;

	// replace
	while( i < nStart+nLen)
	{
		pszSrc[i] = pszDest[i-nStart];
		i++;
	}

	INT32 j = 0;
	
	while( j < nBufSize && i+j < nSrcSize)
	{
		pszSrc[i+j] = pBuf[j];
		j++;
	}

	delete[] pBuf;

	return true;
}


void S2String::substr( char* dest, const char* src, INT32 Pos, INT32 Count)
{
	if (Count == 0)
		Count = Length(src) - Pos;
	
	int nCnt = 0;
	int k = 0;
	while(*src)
	{
		if (nCnt >= Pos && nCnt < (Pos+Count))
			dest[k++] = *src;
		
		nCnt++;
		src++;
	}
	dest[k] = '\0';
}	
*/