#include "stdafx.h"
#include "S2NetServerSocket.h"
#include "S2Memory.h"
#include "S2Error.h"

S2NetServerSocket::S2NetServerSocket()
{
	m_hListenSock = INVALID_SOCKET; 
	return; 
}

S2NetServerSocket::~S2NetServerSocket()
{
	Destroy();
}

BOOL S2NetServerSocket::OnCreate( UINT32 ui32IP, UINT16 ui16Port, int i32BacklogCount )
{
	struct sockaddr_in sockAddr;

	S2_FILLZERO( &sockAddr, sizeof( sockAddr ) );
	sockAddr.sin_family			= AF_INET;
	sockAddr.sin_addr.s_addr	= ui32IP;
	sockAddr.sin_port			= ::htons( ui16Port );
 
	//Set Socket 
	m_hListenSock	= ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );	
	if( INVALID_SOCKET == m_hListenSock ) 
	{
		S2TRACE(  _T("[S2NetServerSocket::OnCreate] m_ListenSock is Invalid Socket\n") );
		return FALSE; 
	}

	//Sets a socket option
	INT32 i32Reuse = 1;		
	::setsockopt( m_hListenSock, SOL_SOCKET, SO_REUSEADDR, ( char* )&i32Reuse, sizeof( i32Reuse ) );

	//Controls the I/O mode of a socket 비동기식으로 설정 
	UINT32 ui32Argp = 1;
	::ioctlsocket( m_hListenSock, FIONBIO, (u_long*)&ui32Argp );

	//Bind Socket 
	if( ::bind( m_hListenSock, ( struct sockaddr * )&sockAddr, sizeof( sockAddr ) ) == SOCKET_ERROR)
	{	
		S2TRACE(  _T("[S2NetServerSocket::OnCreate] Error Bind\n") );
		return FALSE; 
	}

	//Listen Socket
	if(	::listen( m_hListenSock, i32BacklogCount ) == SOCKET_ERROR ) 
	{
		S2TRACE(  _T("[S2NetServerSocket::OnCreate]Error Listen \n") );
		return FALSE; 
	}

	return TRUE; 
}

void S2NetServerSocket::Destroy(void)
{
	if( m_hListenSock != INVALID_SOCKET )::closesocket( m_hListenSock );
	m_hListenSock = INVALID_SOCKET; 
}

SOCKET	S2NetServerSocket::AcceptConnection( struct timeval * ptimeout, struct sockaddr_in * pAddr, INT32 * pSize )
{
	SOCKET AcceptSocket = INVALID_SOCKET;

	fd_set fds; 
	FD_ZERO(&fds); 
// 우리 문제가 아니기에 warning을 막습니다.
#pragma warning(disable: 4127)
	FD_SET( m_hListenSock, &fds );
#pragma warning(default: 4127)
	if(::select( 0, &fds, NULL, NULL, ptimeout) == SOCKET_ERROR ) 
	{
		return AcceptSocket; 
	}

	if( FD_ISSET( m_hListenSock, &fds ) )
	{
		AcceptSocket = ::accept( m_hListenSock, (struct sockaddr *)pAddr, pSize);

		//if( S_OK != AcceptSocket)	return INVALID_SOCKET;
	}

	return AcceptSocket; 
}
