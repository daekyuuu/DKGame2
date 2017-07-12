#include "stdafx.h"
#include "S2JsonFile.h"

S2JsonFile::S2JsonFile()
{
};

S2JsonFile::~S2JsonFile()
{
	OnDestroy();
}

S2Json* S2JsonFile::Create( TCHAR* strFilePath )
{
	S2Json* pJson	= NULL;
	TCHAR*	strFile = NULL;

	// Create New File
	HANDLE hFile = ::CreateFile( strFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( INVALID_HANDLE_VALUE == hFile )
	{			
		goto e_Exit;
	}

	INT32 i32FileSize = GetFileSize( hFile, NULL);
	if( 0 == i32FileSize )
	{
		goto e_Exit;
	}

	INT32 i32ReadSize = 0;

	TCHAR strFirst;
	::ReadFile( hFile, &strFirst, sizeof(TCHAR), (DWORD*)&i32ReadSize, NULL );
	// unicode(빅엔디언) 인지 체크
	if( 0xFEFF != strFirst )
	{
		goto e_Exit;
	}

	i32FileSize -= sizeof(TCHAR);

	INT32 i32BuffSize = i32FileSize/2;	

	strFile = new TCHAR[ i32BuffSize+1 ];
	if( NULL == strFile )
	{
		goto e_Exit;
	}

	::ReadFile( hFile, strFile, i32FileSize, (DWORD*)&i32ReadSize, NULL );
	strFile[ i32BuffSize ] = '\0';

	m_Json.Parse( strFile );
	if( false == m_Json.IsObject() )
	{
		goto e_Exit;
	}

	pJson = &m_Json;

e_Exit:

	SAFE_DELETE_ARRAY( strFile );
	::CloseHandle( hFile );
	hFile = INVALID_HANDLE_VALUE; 

	return pJson;
}

void S2JsonFile::OnDestroy()
{
	m_Json.RemoveAllMembers();
}