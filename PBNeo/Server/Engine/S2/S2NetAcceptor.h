#ifndef __S2_NET_ACCEPTOR_H__
#define __S2_NET_ACCEPTOR_H__

class S2NetServerSocket; 

class S2_EXPORT_BASE S2NetAcceptor 
{
	INT32					m_i32SocketCount;		
	S2NetServerSocket **	m_ppServerSocket;		

public: 	
	S2NetAcceptor(); 	
	virtual ~S2NetAcceptor(); 

	BOOL					Create( INT32 i32SocketCount, UINT32 * pIpAddress, UINT16 * pPortNo );
	void					Destroy( void );

	SOCKET					Update( struct sockaddr_in * pFromAddr );		
};

#endif