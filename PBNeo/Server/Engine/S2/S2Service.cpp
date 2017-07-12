#include "stdafx.h"
#include "S2Service.h"
#include <stdio.h>

void S2Service::Install( TCHAR* strPath, TCHAR* strName )
{
	SC_HANDLE hSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_CREATE_SERVICE ); 
	if( 0 == hSCManager ) 
	{
		UINT32 ui32Error = GetLastError();
		TCHAR strTemp[ S2_STRING_COUNT ];
		S2String::Format( strTemp, S2_STRING_COUNT, _T("OpenSCManager failed, error code = %d\n"), ui32Error );
		_tprintf( _T("%s\n"), strTemp );

		return;
	}
	
	SC_HANDLE hService = CreateService
						( 
							hSCManager,					/* SCManager database      */ 
							strName,					/* name of service         */ 
							strName,					/* service name to display */ 
							SERVICE_ALL_ACCESS,			/* desired access          */ 
							SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS , /* service type            */ 
							SERVICE_DEMAND_START,		/* start type              */ 
							SERVICE_ERROR_NORMAL,		/* error control type      */ 
							strPath,					/* service's binary        */ 
							NULL,						/* no load ordering group  */ 
							NULL,						/* no tag identifier       */ 
							NULL,						/* no dependencies         */ 
							NULL,						/* LocalSystem account     */ 
							NULL						/* no password             */ 
						); 

	if( 0 == hService ) 
	{
		UINT32 ui32Error =  GetLastError();
		TCHAR strTemp[ S2_STRING_COUNT ];
		S2String::Format( strTemp, S2_STRING_COUNT, _T("Failed to create service %s, error code = %d\n"), strName, ui32Error );
		_tprintf( _T("%s\n"), strTemp ); 
	}
	else
	{
		//¼º°ø 
		TCHAR strDes[ S2_STRING_COUNT ];
		S2String::Copy( strDes, _T("Point Blank Clan Server"), S2_STRING_COUNT );

		SERVICE_DESCRIPTION lpDes;
		lpDes.lpDescription = strDes;
		ChangeServiceConfig2( hService, SERVICE_CONFIG_DESCRIPTION, &lpDes );

		TCHAR strTemp[ S2_STRING_COUNT ];
		S2String::Format( strTemp, S2_STRING_COUNT, _T("Service %s installed\n"), strName );
		_tprintf( _T("%s\n"), strTemp );
		CloseServiceHandle( hService );
	}

	CloseServiceHandle( hSCManager );
}

void S2Service::UnInstall( TCHAR* strName )
{
	SC_HANDLE hSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if( 0 == hSCManager ) 
	{
		UINT32 ui32Error = GetLastError();
		TCHAR strTemp[ S2_STRING_COUNT ];
		S2String::Format( strTemp, S2_STRING_COUNT, _T("OpenSCManager failed, error code = %d\n"), ui32Error );
		_tprintf( _T("%s\n"), strTemp );

		return;
	}
		
	SC_HANDLE hService = OpenService( hSCManager, strName, SERVICE_ALL_ACCESS);
	if( 0 == hService ) 
	{
		UINT32 ui32Error = GetLastError();
		TCHAR strTemp[ S2_STRING_COUNT ];
		S2String::Format( strTemp, S2_STRING_COUNT, _T("OpenService failed, error code = %d\n"), ui32Error );
		_tprintf( _T("%s\n"), strTemp );
	}
	else
	{
		if(!DeleteService( hService ))
		{
			TCHAR strTemp[ S2_STRING_COUNT ];
			S2String::Format( strTemp, S2_STRING_COUNT, _T("Failed to delete service %s\n"), strName );
			_tprintf( _T("%s\n"), strTemp );
		}
		else
		{
			TCHAR strTemp[ S2_STRING_COUNT ];
			S2String::Format( strTemp, S2_STRING_COUNT, _T("Service %s removed\n"), strName );
			_tprintf( _T("%s\n"), strTemp );
		}
		CloseServiceHandle( hService );
	}
	CloseServiceHandle( hSCManager );
}

BOOL S2Service::RunService( TCHAR* strName )
{
	// run service with given name
	SC_HANDLE hSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS ); 
	if( 0 == hSCManager ) 
	{
		UINT32 ui32Error = GetLastError();
		TCHAR strTemp[ S2_STRING_COUNT ];
		S2String::Format( strTemp, S2_STRING_COUNT, _T("OpenSCManager failed, error code = %d\n"), ui32Error );
		_tprintf( _T("%s\n"), strTemp );

		return FALSE;
	}
	
	// open the service
	SC_HANDLE hService = OpenService( hSCManager, strName, SERVICE_ALL_ACCESS );
	if( 0 == hService ) 
	{
		UINT32 ui32Error = GetLastError();
		TCHAR strTemp[ S2_STRING_COUNT ];
		S2String::Format( strTemp, S2_STRING_COUNT, _T("OpenService failed, error code = %d\n"), ui32Error );
		_tprintf( _T("%s\n"), strTemp );

		CloseServiceHandle( hSCManager );

		return FALSE;
	}
	
	// call StartService to run the service
	if( FALSE == StartService( hService, 0, (const TCHAR**)NULL ) )
	{
		UINT32 ui32Error = GetLastError();
		TCHAR strTemp[ S2_STRING_COUNT ];
		S2String::Format( strTemp, S2_STRING_COUNT, _T("StartService failed, error code = %d\n"), ui32Error );
		_tprintf( _T("%s\n"), strTemp );

		CloseServiceHandle( hService ); 
		CloseServiceHandle( hSCManager ); 

		return FALSE;
	}

	CloseServiceHandle( hService ); 
	CloseServiceHandle( hSCManager ); 

	return TRUE;
}

BOOL S2Service::StopService( TCHAR* strName )
{
	// Stop service with given name
	SC_HANDLE hSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS ); 
	if( 0 == hSCManager ) 
	{
		UINT32 ui32Error = GetLastError();
		TCHAR strTemp[ S2_STRING_COUNT ];
		S2String::Format( strTemp, S2_STRING_COUNT, _T("OpenSCManager failed, error code = %d\n"), ui32Error );
		_tprintf( _T("%s\n"), strTemp );

		return FALSE;
	}
	
	// open the service
	SC_HANDLE hService = OpenService( hSCManager, strName, SERVICE_ALL_ACCESS );
	if( 0 == hService ) 
	{
		UINT32 ui32Error = GetLastError();
		TCHAR strTemp[ S2_STRING_COUNT ];
		S2String::Format( strTemp, S2_STRING_COUNT, _T("OpenService failed, error code = %d\n"), ui32Error );
		_tprintf( _T("%s\n"), strTemp );

		CloseServiceHandle( hSCManager );

		return FALSE;
	}

	// call ControlService to kill the given service
	SERVICE_STATUS status;
	if( FALSE == ControlService( hService, SERVICE_CONTROL_STOP, &status ) )
	{
		UINT32 ui32Error = GetLastError();
		TCHAR strTemp[ S2_STRING_COUNT ];
		S2String::Format( strTemp, S2_STRING_COUNT, _T("ControlService failed, error code = %d\n"), ui32Error );
		_tprintf( _T("%s\n"), strTemp );

		CloseServiceHandle( hService ); 
		CloseServiceHandle( hSCManager );

		return FALSE;
	}
	
	CloseServiceHandle( hService ); 
	CloseServiceHandle( hSCManager ); 
	
	return TRUE;
}