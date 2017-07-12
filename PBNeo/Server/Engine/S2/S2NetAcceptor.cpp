#include "stdafx.h"
#include "S2NetAcceptor.h"
#include "S2NetServerSocket.h"

S2NetAcceptor::S2NetAcceptor(void)
{	
	m_ppServerSocket	= NULL;
	m_i32SocketCount	= 0;
}

S2NetAcceptor::~S2NetAcceptor(void)
{
	Destroy();	
} 

BOOL S2NetAcceptor::Create( INT32 i32SocketCount, UINT32 * pui32IP, UINT16 * pui16Port )
{
	//Set Value 
	m_i32SocketCount = i32SocketCount; 

	//Create Server Socket 
	m_ppServerSocket	= new S2NetServerSocket*[ m_i32SocketCount ];	
	if( NULL == m_ppServerSocket )													return FALSE;
	for( INT32 i = 0; i < m_i32SocketCount; i++)
	{
		m_ppServerSocket[i] = new S2NetServerSocket; 
		if( NULL == m_ppServerSocket[i] )											return FALSE;
		if( FALSE == m_ppServerSocket[i]->OnCreate( pui32IP[i], pui16Port[i]))		return FALSE; 
	}

	return TRUE; 
}

void S2NetAcceptor::Destroy()
{
	if( m_ppServerSocket )
	{
		for( INT32 i = 0; i < m_i32SocketCount; i++)
		{
			SAFE_DELETE( m_ppServerSocket[i] );
		}
		SAFE_DELETE_ARRAY( m_ppServerSocket );
	}
}

SOCKET S2NetAcceptor::Update( struct sockaddr_in * pFromAddr )
{	
	struct	timeval timeout; 
	SOCKET	hSocketAccept;
	INT32  AddrSize; 

	for( INT32 i = 0; i < m_i32SocketCount; i++)
	{
		timeout.tv_sec  = 0;
		timeout.tv_usec = 1;

		AddrSize = sizeof(sockaddr_in); 

		hSocketAccept = m_ppServerSocket[i]->AcceptConnection( &timeout, pFromAddr, &AddrSize );
		if( INVALID_SOCKET != hSocketAccept )	return hSocketAccept; 
	}

	return INVALID_SOCKET; 
}
