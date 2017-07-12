#include "stdafx.h"
#include "S2Net.h"

BOOL S2Net::Create(void)
{
	WSADATA wsaData;		
	if( ::WSAStartup( MAKEWORD(2,2), &wsaData ) != 0 )
	{
		S2TRACE( _T("WSAStartup Failed. \n") );
		return FALSE; 
	}

	if( wsaData.wVersion != MAKEWORD(2,2) )
	{
		::WSACleanup();
		S2TRACE( _T("WSAStart Version Failed. \n") );
		return FALSE; 
	}

	return TRUE; 
}

//BOOL S2Net::GetMyAddress( TCHAR * pAdress)
//{
//	struct	hostent *pHosten = NULL;
//	TCHAR	strHostName[ 30 ];
//	if( SOCKET_ERROR != gethostname( strHostName, 30 ) )
//	{
//		pHosten = gethostbyname((const char FAR*)szHostName);
//		if(pHosten)
//		{	
//			memcpy( pAdress ,pHosten->h_addr,4);
//		}else return FALSE; 
//	}else return FALSE; 
//	return TRUE; 
//}

void S2Net::DestroySocket( SOCKET hSocket )
{
	if( INVALID_SOCKET != hSocket )
	{
		::shutdown(hSocket, SD_BOTH);
		::closesocket(hSocket);
		hSocket = INVALID_SOCKET;
	}
}

UINT32 S2Net::GetIPToLong( char* strIP )
{
	UINT32 ui32IP	= 0;
	INT32 i32Shift	= 0;	
	char strTemp[ 4 ];
	char* pTemp = strTemp;
	while( *strIP )
	{
		if( '.' == *strIP )
		{
			*pTemp = '\0';
			ui32IP |= (atoi( strTemp ) << (4*i32Shift));
			pTemp = strTemp;
			i32Shift+= 2;
		}
		else
		{
			*pTemp = *strIP;
			pTemp++;
		}
		strIP++;
	}

	*pTemp = '\0';
	ui32IP |= (atoi( strTemp ) << (4*i32Shift));

	return ui32IP;
}

UINT32 S2Net::GetIPToLong( wchar_t* wstrIP )
{
	UINT32 ui32IP = 0;
	INT32 i32Shift = 0;	
	wchar_t strTemp[ 4 ];
	wchar_t* pTemp = strTemp;
	while( *wstrIP )
	{
		if( '.' == *wstrIP )
		{
			*pTemp = '\0';
			ui32IP |= (_wtoi( strTemp ) << (4*i32Shift));
			pTemp = strTemp;
			i32Shift+= 2;
		}
		else
		{
			*pTemp = *wstrIP;
			pTemp++;
		}
		wstrIP++;
	}

	*pTemp = '\0';
	ui32IP |= (_wtoi( strTemp ) << (4*i32Shift));

	return ui32IP;
}

void S2Net::GetIPToLongA( UINT32 ui32IP, char* strIP )
{
	INT32 i32Count = 0;
	INT32 i32Count2;
	UINT32 ui32Temp;
	char strTemp[ 4 ];

	for( INT32 i = 0 ; i < 4 ; i++ )
	{
		ui32Temp = (ui32IP >> (8 * i)) & 0x000000FF;

		i32Count2 = 0;
		do
		{
			strTemp[ i32Count2 ] = (ui32Temp % 10) + L'0';
			ui32Temp /= 10;
			i32Count2++;
		} while( 0 < ui32Temp );
		
		do
		{
			i32Count2--;
			strIP[ i32Count ] = strTemp[ i32Count2 ];
			i32Count++;
		} while( 0 < i32Count2 );
		strIP[ i32Count ] = '.';
		i32Count++;
	}
	strIP[ i32Count-1 ] = L'\0';
}

void S2Net::GetIPToLongW( UINT32 ui32IP, wchar_t* wstrIP )
{
	INT32 i32Count = 0;
	INT32 i32Count2;
	UINT32 ui32Temp;
	wchar_t wstrTemp[ 4 ];

	for( INT32 i = 0 ; i < 4 ; i++ )
	{
		ui32Temp = (ui32IP >> (8 * i)) & 0x000000FF;

		i32Count2 = 0;
		do
		{
			wstrTemp[ i32Count2 ] = (ui32Temp % 10) + L'0';
			ui32Temp /= 10;
			i32Count2++;
		} while( 0 < ui32Temp );
		
		do
		{
			i32Count2--;
			wstrIP[ i32Count ] = wstrTemp[ i32Count2 ];
			i32Count++;
		} while( 0 < i32Count2 );
		wstrIP[ i32Count ] = '.';
		i32Count++;
	}
	wstrIP[ i32Count-1 ] = L'\0';
}

