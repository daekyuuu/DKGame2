#ifndef __S2_NET_SERVER_SOCKET_H__
#define __S2_NET_SERVER_SOCKET_H__

class S2_EXPORT_BASE S2NetServerSocket 
{	
private:
	SOCKET				m_hListenSock;

protected: 
	
public:
	S2NetServerSocket(); 
	virtual ~S2NetServerSocket(); 
	
	BOOL				OnCreate( UINT32 ui32IP, UINT16 ui16Port, int i32BacklogCount = SOMAXCONN );
	void				Destroy();

	SOCKET				AcceptConnection( struct timeval * ptimeout, struct sockaddr_in * pAddr = NULL, INT32 * pSize = NULL); 
	SOCKET				GetListenSocket()											{ return m_hListenSock; }
	
};

#endif