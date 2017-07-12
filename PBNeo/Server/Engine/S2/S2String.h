#ifndef __S2_STRING_H__
#define __S2_STRING_H__

#define S2_STRING_COUNT			256

namespace S2String
{	
	// 문자열을 복사한다.
	S2_EXPORT_BASE void Copy( char * strDest,		const char * strSrc,	INT32 i32DestLength );
	S2_EXPORT_BASE void Copy( wchar_t * strDest,	const wchar_t * strSrc, INT32 i32DestLength );

	/// pDest 문자열에 pSrc를 덧붙인다.
	S2_EXPORT_BASE void Concat( char * strDest,		const char * strSrc,	INT32 i32DestLength );
	S2_EXPORT_BASE void Concat( wchar_t * strDest,	const wchar_t * strSrc, INT32 i32DestLength ); 

	// 문자열의 길이를 구한다.
	S2_EXPORT_BASE INT32 Length( const char * strStr );
	S2_EXPORT_BASE INT32 Length( const wchar_t * strStr );

	S2_EXPORT_BASE INT32 Format( char * strDest,	INT32 i32Length, const char * strFormat, ... );
	S2_EXPORT_BASE INT32 Format( wchar_t * strDest, INT32 i32Length, const wchar_t * strFormat, ... );

	//////////////////////////////////////////////////////////////////////////////////////
	/// 실수를 문자열로 변환한다.
	///
	/// REAL64 Value		: 변환하려는 실수값
	/// char *Result		: 문자열 결과를 받기 위한 버퍼의 포인터
	/// INT32 UnderCount	: 문자열로 변환할 때, 남길 소숫점 아래의 개수
	/// bool FillZero	: 소숫점 아래자리가 UnderCount에 미치지 않을 경우, 나머지 자리를
	///					  0으로 채울 것인지 말것인지 결정하는 인자
	///					  ex.) 1.2를 소숫점 3자리까지 만들때
	///						   FillZero = false -> 1.2
	///						   FillZero = true  -> 1.200
	//
	// 작성자			: 이진균
	// 작성일자			: 1999.6.1
	S2_EXPORT_BASE void MakePointFix( REAL64 r64Value, char *strResult,		INT32 i32UnderCount, bool bFillZero );
	S2_EXPORT_BASE void MakePointFix( REAL64 r64Value, wchar_t *strResult,	INT32 i32UnderCount, bool bFillZero );

	/*
	/////////////////////////////////////////////////////////////////////////////////////////
	/// 값을 Comma(,)를 삽입한 포맷으로 문자열 변환한다.
	/// 
	/// LONG Value		: 문자열로 변환할 값
	/// LPTSTR StrBuf	: 결과 문자열을 반환할 버퍼의 포인터
	/// INT32 Length		: 제한할 최대 길이
	/// 
	/// return 
	///			= 0		: Error
	///			> 0		: 변환된 문자열의 총 길이(Bytes)
	///
	/// ex)	123456780 => 1,234,567,890
	//
	// 작성자			: 이진균
	// 작성일자			: 1999.6.1
	S2_EXPORT_BASE INT32 ValueToStr( INT32 Value, char * szOut, INT32 Length);

	////////////////////////////////////////////////////////////////////////////////////////
	/// 문자열에서 처음 발견되는 valid character(출력가능한 문자)의 위치를 포인터로 반환한다.
	///
	/// return		: 문자열에서 처음 발견되는 문자의 포인터
	/// 
	/// note) white character로 판별하는 경우
	///		 space, tab, carrige return(\r), new line(\n)
	///		
	///		만약 문자열이 빈 문자열일 경우 올바르게 동작하지 않을 수 있다.
	//
	// 작성자		: 이진균
	// 작성일자		: 1999.6.1
	S2_EXPORT_BASE const char *GetFirstValidCharater( const char * lpszStr);

	////////////////////////////////////////////////////////////////////////////////////////
	/// 문자열에서 특정 인덱스의 문자가 영문, 한글 첫번째 Byte, 또는 한굴 두번째 Byte인지를
	/// 판별하여 반환한다.
	/// 
	/// char *str	: 문자열의 포인터
	/// INT32 index	: 판별하려는 문자의 인덱스 (0 based)
	///
	/// return
	///			영문			: 0
	///			한글 1st byte	: 1
	///			한글 2nd byte	: 2
	//
	// 작성자		: 이진균
	// 작성일자		: 1999.6.1
	S2_EXPORT_BASE INT32 WhatIsCode( const char * str, INT32 index);

	///////////////////////////////////////////////////////////////////////////////////////
	/// 문자열의 특정 위치에 다른 문자열을 삽입한다.
	///
	/// char * Dest	: 문자열을 삽입할 문자열
	/// char * Src	: 삽입될 문자열
	/// INT32 Pos		: 문자열을 삽입할 인덱스 (0 based)
	/// INT32 Len		: 삽입할 문자열의 길이
	///
	/// \Return		: 삽입된 문자열의 선두 포인터
	//
	// 작성자		: 이진균
	// 작성일자		: 1999.6.1
	S2_EXPORT_BASE char * InsertStr( char * Dest, const char * Src, INT32 Pos, INT32 Len);
	S2_EXPORT_BASE WCHAR16 * InsertStrW( WCHAR16 * Dest, const WCHAR16 * Src, INT32 Pos, INT32 Len);

	//////////////////////////////////////////////////////////////////////////////////////
	/// 문자열의 앞부분에서부터 공백 문자를 제거한다.
	/// 출력-가능 문자가 발견되면 처리를 종료한다. (뒷부분의 공백문자를 처리하지 않음)
	///
	/// char *str	: 공백문자를 삭제할 문자열
	/// 
	/// \Return		: None
	//
	// 작성자		: 이진균
	// 작성일자		: 1999.6.1
	S2_EXPORT_BASE void RemoveBlankFromHead( char * str);

	/////////////////////////////////////////////////////////////////////////////////////////
	/// 문자열의 뒷부분에서부터 공백 문자를 삭제한다.
	/// 출력-가능한 문자가 발견되면 처리를 종료한다. (앞부분의 공백문자를 처리하지 않음)
	///
	/// char *str	: 공백문자를 제거할 문자열의 포인터
	///
	/// \Return		: none
	//
	// 작성자		: 이진균
	// 작성일자		: 1999.6.1
	S2_EXPORT_BASE void RemoveBlankFromTail( char * str);

	/////////////////////////////////////////////////////////////////////////////////////////
	/// 문자열 전체에서 모든 공백문자를 제거한다.
	/// 출력-공백문자(Space, Tab, Newline, ...)등을 모두 제거한 문자열
	///
	/// char *str	: 공백문자를 제거할 문자열의 포인터
	///
	/// \Return		: none
	//
	// 작성자		: 이진균
	// 작성일자		: 1999.6.1
	S2_EXPORT_BASE void RemoveAllBlank( char *str);
	
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
	S2_EXPORT_BASE void RemoveAllChar( char *str, char RemoveChar);

	/////////////////////////////////////////////////////////////////////////////////////////
	// 파일명을 담은 문자열에서 뒷부부분의 확장자를 제거한 문자열로 만든다.
	///
	/// char *fname	: 문자열의 포인터
	///
	/// \Return		: none
	//
	// 작성자		: 이진균
	// 작성일자		: 1999.6.1
	S2_EXPORT_BASE void RemoveExt( char *fname);

	/////////////////////////////////////////////////////////////////////////////////////////
	// 파일명을 담은 문자열에서 뒷부부분의 확장자를 새로운 확장자로 교체한다.
	///
	/// char *szOut				: 저장될 문자열
	/// const INT32 nOutMax		: 저장될 문자열의 최대 크기
	/// char *szIn				: 원본 문자열
	/// const char *szNewExt	: 새로운 확장자
	///
	/// \Return		: none
	//
	// 작성자		: 차재천
	// 작성일자		: 2005.8.19
	S2_EXPORT_BASE void SwapExt( char *szOut, const INT32 nOutMax, const char *szIn, const char *szNewExt );


	S2_EXPORT_BASE char * SplitFileName( const char *fname, char *Buf, BOOL WithExt);

	S2_EXPORT_BASE void	  SplitPath( const char * pszSrc, char * pszDest);

	/////////////////////////////////////////////////////////////////////////////////////////
	/// 파일명을 담은 문자열에서 뒷부부분의 확장자만을 문자열로 만든다.
	///
	/// char *fname	: 문자열의 포인터
	/// char *ExtBuf : 확장자를 담을 문자열 포인터(NULL일 경우 개수만 리턴)
	///
	/// \Return		: 확장자 문자열의 문자 개수
	//
	// 작성자		: 경주현
	// 작성일자		: 1999.6.21
	S2_EXPORT_BASE INT32 SplitFileExt( const char *fname, char *BufExt = NULL, int BufExtSize = 0 );

	S2_EXPORT_BASE void RemoveDriveChar( char *path);

	S2_EXPORT_BASE void ResolvePathToken( const char * pszPath, i3StringList * pList);

	/////////////////////////////////////////////////////////////////////////////////////////
	/// 주어진 경로를 기준으로 상대 경로로 변환하여 반환한다.
	///
	///
	/// 작성자		: 이진균
	S2_EXPORT_BASE void MakeRelativePath( const char * pszBase, const char * pszAbsPath, char * pszRelPath, const INT32 nRelPathMax);

	S2_EXPORT_BASE void MakeCStylePath( char * pszDest, const char * pszSrc);

	/////////////////////////////////////////////////////////////////////////////////////////
	/// 문자열에 있는 모든 공백을 제거한다.
	///
	/// char *str	: 문자열의 포인터
	/// char *Buf    : 제거한 문자열을 저장할 버퍼(NULL일 경우 제외)
	///
	/// \Return		: 공백을 제거한 문자열의 문자 개수
	//
	// 작성자		: 경주현
	// 작성일자		: 1999.7.19
	S2_EXPORT_BASE INT32 EraseSpace(const char *str, char *Buf);

	S2_EXPORT_BASE bool IsDigitString( const char *str);

	S2_EXPORT_BASE void ParseMultiSelectFileName( char *FileName, i3StringList *List);

	////////////////////////////////////////////////////////////////////////////////////////
	// 파일명에 접미사를 붙인다.
	//
	// 예) "Test.txt" + "_KOR" => "Test_KOR.txt"
	//
	S2_EXPORT_BASE void AddPostfix( char *szOut, const INT32 nOutMax, const char *szIn, const char *szPostfix, char chDot = '.' );

	////////////////////////////////////////////////////////////////////////////////////////
	/// 주어진 경로의 끝에 역슬레쉬(\)가 존재하는 경우, 역슬레쉬를 없앤다.
	/// 
	/// LPSTR Path	: 경로명을 저장한 문자열
	///
	/// \Return		: None
	//
	// 작성자		: 이진균
	// 작성일자		: 1999.6.1
	S2_EXPORT_BASE void NormalizePath( char* Path);

	////////////////////////////////////////////////////////////////////////////////////////
	/// 주어진 경로 문자열에 역슬래쉬(\)가 존재하는 경우, 슬래쉬(/)로 바꾼다.
	S2_EXPORT_BASE void MakeUnixPath( char *szPath );

	////////////////////////////////////////////////////////////////////////////////////////
	S2_EXPORT_BASE void	PreprocessMacro( char * pszDest, INT32 Length, const char * pszSrc, I3_SYMBOLTABLE_INFO * pTable, INT32 symbolCount);

	/////////////////////////////////////////////////////////////////////////////////////////
	/// 파일명을 담은 문자열에서 파일명을 제거하고 디렉토리 부분만을 문자열로 만든다.
	///
	/// char *Path	: 문자열의 포인터
	///
	//
	// 작성자		: 차재천
	// 작성일자		: 2005.01.24
	S2_EXPORT_BASE void RemoveFileName( char *Path );

	S2_EXPORT_BASE bool	IsMatchedWildcard( char * pszFileName, char * pszRull);

	/// 2개의 문자열 비교 (대소문자 안 가림.)
	/// 
	/// \Return		: 차이값 (같으면 0 )
	S2_EXPORT_BASE INT32 Compare( const char* str1, const char* str2);
	S2_EXPORT_BASE INT32 Compare( const wchar_t* str1, const wchar_t* str2);
	S2_EXPORT_BASE INT32 NCompare( const char* str1, const char* str2, INT32 size);		//	by KOMET (2005.09.29)
	S2_EXPORT_BASE INT32 NCompareW( const WCHAR16* str1, const WCHAR16* str2, INT32 size);		// modified by jaeha.lee (2010.12)

	/// 2개의 ASCII 문자열 비교 (대소문자 가림.)
	/// 
	/// \Return		: 차이값 (같으면 0 )
	S2_EXPORT_BASE INT32 CompareCase( const char* str1, const char* str2);
	S2_EXPORT_BASE INT32 CompareCaseW( const WCHAR16* str1, const WCHAR16* str2);

	enum {ALPHABET_INTERVAL = 'a' - 'A', };

	S2_EXPORT_BASE char	ToUpper(char c);
	S2_EXPORT_BASE void ToUpper( char * pStr);
	S2_EXPORT_BASE void ToUpperAfterN ( char * pStr, INT32 nCount);
	S2_EXPORT_BASE void ToUpperBeforeN( char * pStr, INT32 nCount);

	S2_EXPORT_BASE char	ToLower(char c);
	S2_EXPORT_BASE void ToLower( char * Str);
	S2_EXPORT_BASE void ToLower( char * pDest, UINT32 DestSize, const char * pSrc);

	/// pDest에 pSrc의 내용을 복사한다.
	//S2_EXPORT_BASE void Copy( char * pDest, const char * pSrc);
	//S2_EXPORT_BASE void Copy( char * pDest, INT32 DestSize, const char * pSrc);
	//S2_EXPORT_BASE void CopyW( WCHAR16 * pDest, const WCHAR16 * pSrc);
	//S2_EXPORT_BASE void CopyW( WCHAR16 * pDest, INT32 DestSize, const WCHAR16 * pSrc);
	

	//pSrc의 내용중에 Length 만큼 pDest에 복사한다.(널값 포함)
	//설명 : pSrc의 내용 크기가 Length 보다 작을 경우 pSrc 내용 만큼 복사된다.(널값 포함)
	//주위 : pSrc의 내용 크기가 pDest의 메모리 버퍼 크기와 같은 경우 널값을 포함하기 때문에 메모리 침범 한다.
	//Length : pDest 메모리 버퍼 보다 작아야 한다.
	S2_EXPORT_BASE void NCopy( char * pDest, const char *pSrc, INT32 Length);
	S2_EXPORT_BASE void NCopy( WCHAR16 * pDest, const WCHAR16 *pSrc, INT32 Length);

	S2_EXPORT_BASE void CopyASCIItoWCHAR( WCHAR16 * pDest, const char * pSrc );
	S2_EXPORT_BASE void CopyWCHARtoASCII( char * pDest, INT32 nDestLength, const WCHAR16 * pSrc, INT32 nSrcLength);

	// 유니코드 스트링을 UTF-8포맷으로 인코딩
	S2_EXPORT_BASE int UCS2toUTF8( char *pDest, const WCHAR16 *pSrc, int nDestBufSize );

	// UTF-8포맷을 유니코드 스트링으로 디코딩
	S2_EXPORT_BASE int UTF8toUCS2( WCHAR16 *pDest, const char *pSrc, int nDestBufSize );	

	// pStr 문자열 안에 ch가 있으면 그 위치를 반환한다.
	// 앞에서 부터 찾는다.
	// 없으면 -1 반환
	S2_EXPORT_BASE INT32 ContainChar( const char * pStr, char ch );

	// pStr 문자열 안에 ch가 있으면 그 위치를 반환한다.
	// 앞에서 부터 찾는다.
	// 없으면 -1 반환
	S2_EXPORT_BASE INT32 ContainChar( const wchar_t * pStr, wchar_t ch );

	// pStr 문자열 안에 ch가 있으면 그 위치를 반환한다.
	// 뒤에서 부터 찾는다.
	// 없으면 -1 반환
	S2_EXPORT_BASE INT32 ContainCharRev( const char * pStr, char ch );

	/// pStr1 문자열 안에 pStr2가 들어 있으면 그 위치를 인덱스로 돌려준다.
	// 없으면 -1 반환
	S2_EXPORT_BASE INT32 Contain( const char * pszStr, const char * pszKey);
	S2_EXPORT_BASE INT32 ContainNew( const char* pStr1, const char* pStr2);
	S2_EXPORT_BASE INT32 ContainW( const WCHAR16 * pStr1, const WCHAR16 * pStr2);
	S2_EXPORT_BASE INT32 ContainNative( const char * pStr);	//	문자열안에 ASCII를 제외한 다국어가 들어 있으면 위치 반환
	S2_EXPORT_BASE INT32 ContainNoCase( const char * pszStr, const char * pszKey);
		

	/// szIn과 같은 내용의 문자열을 생성해서 리턴한다.
	//S2_EXPORT_BASE char* NewString( const char* szIn, I3HMEM *pMemID = NULL );
	S2_EXPORT_BASE char* NewString( const char* szIn );

	S2_EXPORT_BASE void	ftoa( REAL32 fval, char * pszStr, INT32 fractLen = -1);

	S2_EXPORT_BASE INT32	ToInt( const char * pszStr);

	S2_EXPORT_BASE void		GetPrefixName( char * pszDest, const  char * pszStr);

	// 문자열에 포함된 특정 문자열을 변경한다. (대소문자 구분)
	S2_EXPORT_BASE BOOL Replace(char * str, INT32 bufLen, const char * before, const char * affer);
	S2_EXPORT_BASE BOOL ReplaceNoCase(char * str, INT32 bufLen, const char * before, const char * affer);

	///	ANSI 문자열에서 바이트 단위가 아닌 문자 단위로 포인터가 이동합니다. komet 2008.8.31
	//	nCodePage는 국가 코드 페이지를 나타내며 자세한 것은 MSDN 참조
	S2_EXPORT_BASE const char *	CharNext(INT32 nCodePage, const char * pszStr);
	S2_EXPORT_BASE const char *	CharPrev(INT32 nCodePage, const char * pszStart, const char * pszCurrent);
	S2_EXPORT_BASE INT32 LengthChar(INT32 nCodePage, const char * pszStr);
	S2_EXPORT_BASE BOOL IsComposite(INT32 nCodePage, const char prev, const char curr, INT32 mode);

	

	enum ALGORITHM
	{
		BF = 0,		//	Brute Force			(가장 단순함)
		KMP,		//	Knuth-Morris-Pratt	(내부에서 동적 메모리할당이 발생)
		BM,			//	Boyer-Moore			(내부에서 동적 메모리할당이 발생)
		QUICK,		//	Quick Search		(Boyer-Moore 경량판)
		RK,			//	Rabin-Karp			(해시를 이용)
	};

	//	string searching algorithm
	S2_EXPORT_BASE INT32 Search(char * pszDest, char * pszSrc, ALGORITHM type);


	// Replace pszSrc[nStart]~pszSrc[nEnd] to pszDest[0]~pszDest[nLen-1]
	// returns false if nSrcSize is not big enough for replaced string(pszSrc will not be changed)
	// pszSrc must be a null terminated string.
	// 2010.7.2 iloveno9
	S2_EXPORT_BASE bool NReplace( char* pszSrc, INT32 nStart, INT32 nEnd, INT32 nSrcSize, const char* pszDest, INT32 nLen);

	S2_EXPORT_BASE void substr(char* dest, const char* src, INT32 Pos, INT32 Count = 0);
	*/
};

#endif