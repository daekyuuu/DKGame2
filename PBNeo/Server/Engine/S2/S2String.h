#ifndef __S2_STRING_H__
#define __S2_STRING_H__

#define S2_STRING_COUNT			256

namespace S2String
{	
	// ���ڿ��� �����Ѵ�.
	S2_EXPORT_BASE void Copy( char * strDest,		const char * strSrc,	INT32 i32DestLength );
	S2_EXPORT_BASE void Copy( wchar_t * strDest,	const wchar_t * strSrc, INT32 i32DestLength );

	/// pDest ���ڿ��� pSrc�� �����δ�.
	S2_EXPORT_BASE void Concat( char * strDest,		const char * strSrc,	INT32 i32DestLength );
	S2_EXPORT_BASE void Concat( wchar_t * strDest,	const wchar_t * strSrc, INT32 i32DestLength ); 

	// ���ڿ��� ���̸� ���Ѵ�.
	S2_EXPORT_BASE INT32 Length( const char * strStr );
	S2_EXPORT_BASE INT32 Length( const wchar_t * strStr );

	S2_EXPORT_BASE INT32 Format( char * strDest,	INT32 i32Length, const char * strFormat, ... );
	S2_EXPORT_BASE INT32 Format( wchar_t * strDest, INT32 i32Length, const wchar_t * strFormat, ... );

	//////////////////////////////////////////////////////////////////////////////////////
	/// �Ǽ��� ���ڿ��� ��ȯ�Ѵ�.
	///
	/// REAL64 Value		: ��ȯ�Ϸ��� �Ǽ���
	/// char *Result		: ���ڿ� ����� �ޱ� ���� ������ ������
	/// INT32 UnderCount	: ���ڿ��� ��ȯ�� ��, ���� �Ҽ��� �Ʒ��� ����
	/// bool FillZero	: �Ҽ��� �Ʒ��ڸ��� UnderCount�� ��ġ�� ���� ���, ������ �ڸ���
	///					  0���� ä�� ������ �������� �����ϴ� ����
	///					  ex.) 1.2�� �Ҽ��� 3�ڸ����� ���鶧
	///						   FillZero = false -> 1.2
	///						   FillZero = true  -> 1.200
	//
	// �ۼ���			: ������
	// �ۼ�����			: 1999.6.1
	S2_EXPORT_BASE void MakePointFix( REAL64 r64Value, char *strResult,		INT32 i32UnderCount, bool bFillZero );
	S2_EXPORT_BASE void MakePointFix( REAL64 r64Value, wchar_t *strResult,	INT32 i32UnderCount, bool bFillZero );

	/*
	/////////////////////////////////////////////////////////////////////////////////////////
	/// ���� Comma(,)�� ������ �������� ���ڿ� ��ȯ�Ѵ�.
	/// 
	/// LONG Value		: ���ڿ��� ��ȯ�� ��
	/// LPTSTR StrBuf	: ��� ���ڿ��� ��ȯ�� ������ ������
	/// INT32 Length		: ������ �ִ� ����
	/// 
	/// return 
	///			= 0		: Error
	///			> 0		: ��ȯ�� ���ڿ��� �� ����(Bytes)
	///
	/// ex)	123456780 => 1,234,567,890
	//
	// �ۼ���			: ������
	// �ۼ�����			: 1999.6.1
	S2_EXPORT_BASE INT32 ValueToStr( INT32 Value, char * szOut, INT32 Length);

	////////////////////////////////////////////////////////////////////////////////////////
	/// ���ڿ����� ó�� �߰ߵǴ� valid character(��°����� ����)�� ��ġ�� �����ͷ� ��ȯ�Ѵ�.
	///
	/// return		: ���ڿ����� ó�� �߰ߵǴ� ������ ������
	/// 
	/// note) white character�� �Ǻ��ϴ� ���
	///		 space, tab, carrige return(\r), new line(\n)
	///		
	///		���� ���ڿ��� �� ���ڿ��� ��� �ùٸ��� �������� ���� �� �ִ�.
	//
	// �ۼ���		: ������
	// �ۼ�����		: 1999.6.1
	S2_EXPORT_BASE const char *GetFirstValidCharater( const char * lpszStr);

	////////////////////////////////////////////////////////////////////////////////////////
	/// ���ڿ����� Ư�� �ε����� ���ڰ� ����, �ѱ� ù��° Byte, �Ǵ� �ѱ� �ι�° Byte������
	/// �Ǻ��Ͽ� ��ȯ�Ѵ�.
	/// 
	/// char *str	: ���ڿ��� ������
	/// INT32 index	: �Ǻ��Ϸ��� ������ �ε��� (0 based)
	///
	/// return
	///			����			: 0
	///			�ѱ� 1st byte	: 1
	///			�ѱ� 2nd byte	: 2
	//
	// �ۼ���		: ������
	// �ۼ�����		: 1999.6.1
	S2_EXPORT_BASE INT32 WhatIsCode( const char * str, INT32 index);

	///////////////////////////////////////////////////////////////////////////////////////
	/// ���ڿ��� Ư�� ��ġ�� �ٸ� ���ڿ��� �����Ѵ�.
	///
	/// char * Dest	: ���ڿ��� ������ ���ڿ�
	/// char * Src	: ���Ե� ���ڿ�
	/// INT32 Pos		: ���ڿ��� ������ �ε��� (0 based)
	/// INT32 Len		: ������ ���ڿ��� ����
	///
	/// \Return		: ���Ե� ���ڿ��� ���� ������
	//
	// �ۼ���		: ������
	// �ۼ�����		: 1999.6.1
	S2_EXPORT_BASE char * InsertStr( char * Dest, const char * Src, INT32 Pos, INT32 Len);
	S2_EXPORT_BASE WCHAR16 * InsertStrW( WCHAR16 * Dest, const WCHAR16 * Src, INT32 Pos, INT32 Len);

	//////////////////////////////////////////////////////////////////////////////////////
	/// ���ڿ��� �պκп������� ���� ���ڸ� �����Ѵ�.
	/// ���-���� ���ڰ� �߰ߵǸ� ó���� �����Ѵ�. (�޺κ��� ���鹮�ڸ� ó������ ����)
	///
	/// char *str	: ���鹮�ڸ� ������ ���ڿ�
	/// 
	/// \Return		: None
	//
	// �ۼ���		: ������
	// �ۼ�����		: 1999.6.1
	S2_EXPORT_BASE void RemoveBlankFromHead( char * str);

	/////////////////////////////////////////////////////////////////////////////////////////
	/// ���ڿ��� �޺κп������� ���� ���ڸ� �����Ѵ�.
	/// ���-������ ���ڰ� �߰ߵǸ� ó���� �����Ѵ�. (�պκ��� ���鹮�ڸ� ó������ ����)
	///
	/// char *str	: ���鹮�ڸ� ������ ���ڿ��� ������
	///
	/// \Return		: none
	//
	// �ۼ���		: ������
	// �ۼ�����		: 1999.6.1
	S2_EXPORT_BASE void RemoveBlankFromTail( char * str);

	/////////////////////////////////////////////////////////////////////////////////////////
	/// ���ڿ� ��ü���� ��� ���鹮�ڸ� �����Ѵ�.
	/// ���-���鹮��(Space, Tab, Newline, ...)���� ��� ������ ���ڿ�
	///
	/// char *str	: ���鹮�ڸ� ������ ���ڿ��� ������
	///
	/// \Return		: none
	//
	// �ۼ���		: ������
	// �ۼ�����		: 1999.6.1
	S2_EXPORT_BASE void RemoveAllBlank( char *str);
	
	/////////////////////////////////////////////////////////////////////////////////////////
	// ���ڿ��� ���Ե� Ư�� ���ڸ� ��� �����Ѵ�.
	// (������ ��� ��ҹ��� ������.)
	//
	// char *str	: Ư�� ���ڸ� ������ ���ڿ��� ������
	//
	// Return		: none
	//
	// �ۼ���		: �鼺��
	// �ۼ�����		: 2008.8.22
	S2_EXPORT_BASE void RemoveAllChar( char *str, char RemoveChar);

	/////////////////////////////////////////////////////////////////////////////////////////
	// ���ϸ��� ���� ���ڿ����� �޺κκ��� Ȯ���ڸ� ������ ���ڿ��� �����.
	///
	/// char *fname	: ���ڿ��� ������
	///
	/// \Return		: none
	//
	// �ۼ���		: ������
	// �ۼ�����		: 1999.6.1
	S2_EXPORT_BASE void RemoveExt( char *fname);

	/////////////////////////////////////////////////////////////////////////////////////////
	// ���ϸ��� ���� ���ڿ����� �޺κκ��� Ȯ���ڸ� ���ο� Ȯ���ڷ� ��ü�Ѵ�.
	///
	/// char *szOut				: ����� ���ڿ�
	/// const INT32 nOutMax		: ����� ���ڿ��� �ִ� ũ��
	/// char *szIn				: ���� ���ڿ�
	/// const char *szNewExt	: ���ο� Ȯ����
	///
	/// \Return		: none
	//
	// �ۼ���		: ����õ
	// �ۼ�����		: 2005.8.19
	S2_EXPORT_BASE void SwapExt( char *szOut, const INT32 nOutMax, const char *szIn, const char *szNewExt );


	S2_EXPORT_BASE char * SplitFileName( const char *fname, char *Buf, BOOL WithExt);

	S2_EXPORT_BASE void	  SplitPath( const char * pszSrc, char * pszDest);

	/////////////////////////////////////////////////////////////////////////////////////////
	/// ���ϸ��� ���� ���ڿ����� �޺κκ��� Ȯ���ڸ��� ���ڿ��� �����.
	///
	/// char *fname	: ���ڿ��� ������
	/// char *ExtBuf : Ȯ���ڸ� ���� ���ڿ� ������(NULL�� ��� ������ ����)
	///
	/// \Return		: Ȯ���� ���ڿ��� ���� ����
	//
	// �ۼ���		: ������
	// �ۼ�����		: 1999.6.21
	S2_EXPORT_BASE INT32 SplitFileExt( const char *fname, char *BufExt = NULL, int BufExtSize = 0 );

	S2_EXPORT_BASE void RemoveDriveChar( char *path);

	S2_EXPORT_BASE void ResolvePathToken( const char * pszPath, i3StringList * pList);

	/////////////////////////////////////////////////////////////////////////////////////////
	/// �־��� ��θ� �������� ��� ��η� ��ȯ�Ͽ� ��ȯ�Ѵ�.
	///
	///
	/// �ۼ���		: ������
	S2_EXPORT_BASE void MakeRelativePath( const char * pszBase, const char * pszAbsPath, char * pszRelPath, const INT32 nRelPathMax);

	S2_EXPORT_BASE void MakeCStylePath( char * pszDest, const char * pszSrc);

	/////////////////////////////////////////////////////////////////////////////////////////
	/// ���ڿ��� �ִ� ��� ������ �����Ѵ�.
	///
	/// char *str	: ���ڿ��� ������
	/// char *Buf    : ������ ���ڿ��� ������ ����(NULL�� ��� ����)
	///
	/// \Return		: ������ ������ ���ڿ��� ���� ����
	//
	// �ۼ���		: ������
	// �ۼ�����		: 1999.7.19
	S2_EXPORT_BASE INT32 EraseSpace(const char *str, char *Buf);

	S2_EXPORT_BASE bool IsDigitString( const char *str);

	S2_EXPORT_BASE void ParseMultiSelectFileName( char *FileName, i3StringList *List);

	////////////////////////////////////////////////////////////////////////////////////////
	// ���ϸ� ���̻縦 ���δ�.
	//
	// ��) "Test.txt" + "_KOR" => "Test_KOR.txt"
	//
	S2_EXPORT_BASE void AddPostfix( char *szOut, const INT32 nOutMax, const char *szIn, const char *szPostfix, char chDot = '.' );

	////////////////////////////////////////////////////////////////////////////////////////
	/// �־��� ����� ���� ��������(\)�� �����ϴ� ���, ���������� ���ش�.
	/// 
	/// LPSTR Path	: ��θ��� ������ ���ڿ�
	///
	/// \Return		: None
	//
	// �ۼ���		: ������
	// �ۼ�����		: 1999.6.1
	S2_EXPORT_BASE void NormalizePath( char* Path);

	////////////////////////////////////////////////////////////////////////////////////////
	/// �־��� ��� ���ڿ��� ��������(\)�� �����ϴ� ���, ������(/)�� �ٲ۴�.
	S2_EXPORT_BASE void MakeUnixPath( char *szPath );

	////////////////////////////////////////////////////////////////////////////////////////
	S2_EXPORT_BASE void	PreprocessMacro( char * pszDest, INT32 Length, const char * pszSrc, I3_SYMBOLTABLE_INFO * pTable, INT32 symbolCount);

	/////////////////////////////////////////////////////////////////////////////////////////
	/// ���ϸ��� ���� ���ڿ����� ���ϸ��� �����ϰ� ���丮 �κи��� ���ڿ��� �����.
	///
	/// char *Path	: ���ڿ��� ������
	///
	//
	// �ۼ���		: ����õ
	// �ۼ�����		: 2005.01.24
	S2_EXPORT_BASE void RemoveFileName( char *Path );

	S2_EXPORT_BASE bool	IsMatchedWildcard( char * pszFileName, char * pszRull);

	/// 2���� ���ڿ� �� (��ҹ��� �� ����.)
	/// 
	/// \Return		: ���̰� (������ 0 )
	S2_EXPORT_BASE INT32 Compare( const char* str1, const char* str2);
	S2_EXPORT_BASE INT32 Compare( const wchar_t* str1, const wchar_t* str2);
	S2_EXPORT_BASE INT32 NCompare( const char* str1, const char* str2, INT32 size);		//	by KOMET (2005.09.29)
	S2_EXPORT_BASE INT32 NCompareW( const WCHAR16* str1, const WCHAR16* str2, INT32 size);		// modified by jaeha.lee (2010.12)

	/// 2���� ASCII ���ڿ� �� (��ҹ��� ����.)
	/// 
	/// \Return		: ���̰� (������ 0 )
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

	/// pDest�� pSrc�� ������ �����Ѵ�.
	//S2_EXPORT_BASE void Copy( char * pDest, const char * pSrc);
	//S2_EXPORT_BASE void Copy( char * pDest, INT32 DestSize, const char * pSrc);
	//S2_EXPORT_BASE void CopyW( WCHAR16 * pDest, const WCHAR16 * pSrc);
	//S2_EXPORT_BASE void CopyW( WCHAR16 * pDest, INT32 DestSize, const WCHAR16 * pSrc);
	

	//pSrc�� �����߿� Length ��ŭ pDest�� �����Ѵ�.(�ΰ� ����)
	//���� : pSrc�� ���� ũ�Ⱑ Length ���� ���� ��� pSrc ���� ��ŭ ����ȴ�.(�ΰ� ����)
	//���� : pSrc�� ���� ũ�Ⱑ pDest�� �޸� ���� ũ��� ���� ��� �ΰ��� �����ϱ� ������ �޸� ħ�� �Ѵ�.
	//Length : pDest �޸� ���� ���� �۾ƾ� �Ѵ�.
	S2_EXPORT_BASE void NCopy( char * pDest, const char *pSrc, INT32 Length);
	S2_EXPORT_BASE void NCopy( WCHAR16 * pDest, const WCHAR16 *pSrc, INT32 Length);

	S2_EXPORT_BASE void CopyASCIItoWCHAR( WCHAR16 * pDest, const char * pSrc );
	S2_EXPORT_BASE void CopyWCHARtoASCII( char * pDest, INT32 nDestLength, const WCHAR16 * pSrc, INT32 nSrcLength);

	// �����ڵ� ��Ʈ���� UTF-8�������� ���ڵ�
	S2_EXPORT_BASE int UCS2toUTF8( char *pDest, const WCHAR16 *pSrc, int nDestBufSize );

	// UTF-8������ �����ڵ� ��Ʈ������ ���ڵ�
	S2_EXPORT_BASE int UTF8toUCS2( WCHAR16 *pDest, const char *pSrc, int nDestBufSize );	

	// pStr ���ڿ� �ȿ� ch�� ������ �� ��ġ�� ��ȯ�Ѵ�.
	// �տ��� ���� ã�´�.
	// ������ -1 ��ȯ
	S2_EXPORT_BASE INT32 ContainChar( const char * pStr, char ch );

	// pStr ���ڿ� �ȿ� ch�� ������ �� ��ġ�� ��ȯ�Ѵ�.
	// �տ��� ���� ã�´�.
	// ������ -1 ��ȯ
	S2_EXPORT_BASE INT32 ContainChar( const wchar_t * pStr, wchar_t ch );

	// pStr ���ڿ� �ȿ� ch�� ������ �� ��ġ�� ��ȯ�Ѵ�.
	// �ڿ��� ���� ã�´�.
	// ������ -1 ��ȯ
	S2_EXPORT_BASE INT32 ContainCharRev( const char * pStr, char ch );

	/// pStr1 ���ڿ� �ȿ� pStr2�� ��� ������ �� ��ġ�� �ε����� �����ش�.
	// ������ -1 ��ȯ
	S2_EXPORT_BASE INT32 Contain( const char * pszStr, const char * pszKey);
	S2_EXPORT_BASE INT32 ContainNew( const char* pStr1, const char* pStr2);
	S2_EXPORT_BASE INT32 ContainW( const WCHAR16 * pStr1, const WCHAR16 * pStr2);
	S2_EXPORT_BASE INT32 ContainNative( const char * pStr);	//	���ڿ��ȿ� ASCII�� ������ �ٱ�� ��� ������ ��ġ ��ȯ
	S2_EXPORT_BASE INT32 ContainNoCase( const char * pszStr, const char * pszKey);
		

	/// szIn�� ���� ������ ���ڿ��� �����ؼ� �����Ѵ�.
	//S2_EXPORT_BASE char* NewString( const char* szIn, I3HMEM *pMemID = NULL );
	S2_EXPORT_BASE char* NewString( const char* szIn );

	S2_EXPORT_BASE void	ftoa( REAL32 fval, char * pszStr, INT32 fractLen = -1);

	S2_EXPORT_BASE INT32	ToInt( const char * pszStr);

	S2_EXPORT_BASE void		GetPrefixName( char * pszDest, const  char * pszStr);

	// ���ڿ��� ���Ե� Ư�� ���ڿ��� �����Ѵ�. (��ҹ��� ����)
	S2_EXPORT_BASE BOOL Replace(char * str, INT32 bufLen, const char * before, const char * affer);
	S2_EXPORT_BASE BOOL ReplaceNoCase(char * str, INT32 bufLen, const char * before, const char * affer);

	///	ANSI ���ڿ����� ����Ʈ ������ �ƴ� ���� ������ �����Ͱ� �̵��մϴ�. komet 2008.8.31
	//	nCodePage�� ���� �ڵ� �������� ��Ÿ���� �ڼ��� ���� MSDN ����
	S2_EXPORT_BASE const char *	CharNext(INT32 nCodePage, const char * pszStr);
	S2_EXPORT_BASE const char *	CharPrev(INT32 nCodePage, const char * pszStart, const char * pszCurrent);
	S2_EXPORT_BASE INT32 LengthChar(INT32 nCodePage, const char * pszStr);
	S2_EXPORT_BASE BOOL IsComposite(INT32 nCodePage, const char prev, const char curr, INT32 mode);

	

	enum ALGORITHM
	{
		BF = 0,		//	Brute Force			(���� �ܼ���)
		KMP,		//	Knuth-Morris-Pratt	(���ο��� ���� �޸��Ҵ��� �߻�)
		BM,			//	Boyer-Moore			(���ο��� ���� �޸��Ҵ��� �߻�)
		QUICK,		//	Quick Search		(Boyer-Moore �淮��)
		RK,			//	Rabin-Karp			(�ؽø� �̿�)
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