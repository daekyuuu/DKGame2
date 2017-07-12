#ifndef __S2_SERVICE_H__
#define __S2_SERVICE_H__

namespace S2Service
{
	void					Install( TCHAR* strPath, TCHAR* strName );
	void					UnInstall( TCHAR* strName );
	BOOL					RunService( TCHAR* strName );
	BOOL					StopService( TCHAR* strName );
};

#endif